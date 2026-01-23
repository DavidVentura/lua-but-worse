from ast_nodes import *
from ir_nodes import *


class IRLowering:
    """Lower Lua AST to C IR

    This pass converts high-level Lua constructs to a C-like IR that can be
    directly translated to C code.
    """

    def __init__(self, scopes: dict[int, Scope], global_scope: Scope, escaping_vars: set[VarInfo]):
        self.scopes = scopes
        self.global_scope = global_scope
        self.escaping_vars = escaping_vars
        self.c_functions: list[CFunctionDef] = []
        self.current_scope_id: Optional[int] = None
        self.next_temp = 0
        self.globals: list[str] = []
        self.no_return_builtins = {"printh", "set_tabvalue", "setmetatable"}
        self.direct_call_builtins = {"flr", "printh", "setmetatable", "getmetatable", "all", "pairs", "ipairs"}
        self.string_constants: dict[str, str] = {}  # value -> var_name mapping

    def _get_string_constant(self, value: str) -> str:
        """Get or create a global variable name for a string constant"""
        if value in self.string_constants:
            return self.string_constants[value]

        # Create a safe variable name from the string
        # Use first few chars if alphanumeric, otherwise use generic name
        safe_chars = ''.join(c if c.isalnum() else '_' for c in value[:8])
        if not safe_chars or not safe_chars[0].isalpha():
            safe_chars = 'str'

        # Generate unique name
        idx = len(self.string_constants)
        var_name = f"__str_ct_{safe_chars}_{idx}"
        self.string_constants[value] = var_name
        return var_name

    def lower(self, ast: Block) -> tuple[list[str], list[CFunctionDef], set[str], dict[str, str]]:
        """Lower the entire AST to IR, returning (globals, functions, escaping_var_names, string_constants)"""
        self.current_scope_id = self.global_scope.scope_id

        main_body = self._lower_block(ast)
        main_func = CFunctionDef(
            name="_lua_main",
            params=[],
            body=main_body
        )
        self.c_functions.append(main_func)

        escaping_names = {v.name for v in self.escaping_vars}
        return (self.globals, self.c_functions, escaping_names, self.string_constants)

    def _new_temp(self) -> str:
        """Generate a new temporary variable name"""
        name = f"tmp_{self.next_temp}"
        self.next_temp += 1
        return name

    def _make_for_loop(self, idx_var: str, idx_type: CType, start: str, end_var: str, op: str, body: list[CStmt]) -> CFor:
        """Helper to create a numeric for loop: for(type idx=start; idx op end_var; idx++)"""
        init_stmt = CDeclare(CVar(idx_var, idx_type), CLiteral(start, idx_type), direct_init=True)
        condition = CBinOp(op, CVarRef(CVar(idx_var, idx_type)), CVarRef(CVar(end_var, idx_type)))
        increment = CExprStmt(CLiteral(f"{idx_var}++", CType("void")), needs_cleanup=False)
        return CFor(init_stmt, condition, increment, body)

    def _needs_cleanup(self, expr: CExpr) -> bool:
        """Check if expression result needs GC cleanup"""
        if isinstance(expr, CFunctionCall):
            if expr.func_name in self.no_return_builtins:
                return False
            if expr.func_name == "CALL" and len(expr.args) > 0:
                first_arg = expr.args[0]
                if isinstance(first_arg, CVarRef) and first_arg.var.name in self.no_return_builtins:
                    return False
        return True

    def _lower_block(self, block: Block) -> list[CStmt]:
        """Lower a block of statements"""
        stmts = []
        for stmt in block.stmts:
            stmts.extend(self._lower_stmt(stmt))
        return stmts

    def _lower_stmt(self, stmt: Stmt) -> list[CStmt]:
        """Lower a single statement to one or more C statements"""
        match stmt:
            case LocalDecl(names, values):
                stmts = []
                for i, name in enumerate(names):
                    value = values[i] if i < len(values) else Nil()
                    value_expr = self._lower_expr(value)
                    stmts.append(CDeclare(CVar(name, TVALUE, CVarQualifier.GC), value_expr))
                return stmts

            case Assign(targets, values):
                stmts = []
                for i, target in enumerate(targets):
                    value = values[i] if i < len(values) else Nil()

                    match target:
                        case NameRef(name, resolved):
                            if resolved and resolved.kind == VarKind.GLOBAL:
                                if name not in self.globals:
                                    self.globals.append(name)

                            value_expr = self._lower_expr(value)
                            stmts.append(CAssign(CVar(name, TVALUE), value_expr))

                        case TableAccess(table, key, is_dot):
                            value_expr = self._lower_expr(value)
                            table_expr = self._lower_expr(table)
                            key_expr = self._lower_expr(key)
                            call = CFunctionCall("set_tabvalue", [table_expr, key_expr, value_expr])
                            stmts.append(CExprStmt(call, needs_cleanup=False))
                return stmts

            case CompoundAssign(target, op, value):
                # a += b  ->  a = a + b
                match target:
                    case NameRef(name, resolved):
                        left = CVarRef(CVar(name, TVALUE))
                        right = self._lower_expr(value)
                        result = self._lower_binop(op, left, right)
                        return [CAssign(CVar(name, TVALUE), result)]
                return []

            case FunctionDef(name_parts, is_method, params, body, scope_id):
                func_name = "_".join(name_parts)

                # Get captures from scope
                captures = []
                if scope_id is not None:
                    scope = self.scopes[scope_id]
                    captures = [v.name for v in scope.captures]

                # Lower function body
                prev_scope = self.current_scope_id
                self.current_scope_id = scope_id
                func_body = self._lower_block(body)
                self.current_scope_id = prev_scope

                # Create function def
                func_def = CFunctionDef(
                    name=func_name,
                    params=params,
                    body=func_body,
                    captures=captures
                )
                self.c_functions.append(func_def)

                # If function has dotted name (e.g., vector.new or vector:method),
                # generate assignment: table.key = TFUN(func_name)
                if len(name_parts) > 1:
                    # Build table access from all but last part
                    table = NameRef(name_parts[0])
                    for part in name_parts[1:-1]:
                        table = TableAccess(table, String(part), True)

                    # Key is the last part
                    key = String(name_parts[-1])

                    # Value is TFUN(func_name)
                    func_value = CLiteral(f"TFUN({func_name})", TVALUE)

                    # Generate set_tabvalue call
                    table_expr = self._lower_expr(table)
                    key_expr = self._lower_expr(key)
                    call = CFunctionCall("set_tabvalue", [table_expr, key_expr, func_value])
                    return [CExprStmt(call, needs_cleanup=False)]

                return []

            case If(condition, then_block, elseif_parts, else_block):
                cond_expr = self._lower_expr(condition)
                cond_expr = CFunctionCall("__bool", [cond_expr])

                then_stmts = self._lower_block(then_block)

                # Start with the final else block (or empty)
                else_stmts = self._lower_block(else_block) if else_block else []

                # Build nested ifs from elseifs in reverse order
                for elif_cond, elif_block in reversed(elseif_parts):
                    elif_cond_expr = self._lower_expr(elif_cond)
                    elif_cond_expr = CFunctionCall("__bool", [elif_cond_expr])
                    elif_then = self._lower_block(elif_block)
                    else_stmts = [CIf(elif_cond_expr, elif_then, else_stmts)]

                return [CIf(cond_expr, then_stmts, else_stmts)]

            case While(condition, body):
                cond_expr = self._lower_expr(condition)
                cond_expr = CFunctionCall("__bool", [cond_expr])
                body_stmts = self._lower_block(body)
                return [CWhile(cond_expr, body_stmts)]

            case ForNum(var, start, stop, step, body, scope_id):
                # For now, lower to while loop with manual iteration
                # TODO: optimize to actual C for loop
                start_expr = self._lower_expr(start)
                stop_expr = self._lower_expr(stop)
                step_expr = self._lower_expr(step) if step else CLiteral("TNUM8(1)", TVALUE)

                stmts = []
                stmts.append(CDeclare(CVar(var, TVALUE, CVarQualifier.GC), start_expr))

                stop_var = self._new_temp()
                stmts.append(CDeclare(CVar(stop_var, TVALUE, CVarQualifier.GC), stop_expr))

                # Condition: var <= stop (or >= if step < 0)
                cond = CFunctionCall("_leq", [CVarRef(CVar(var, TVALUE)), CVarRef(CVar(stop_var, TVALUE))])
                cond = CFunctionCall("__bool", [cond])

                body_stmts = self._lower_block(body)
                # Increment: var = var + step
                increment = CFunctionCall("_add", [CVarRef(CVar(var, TVALUE)), step_expr])
                body_stmts.append(CAssign(CVar(var, TVALUE), increment))

                stmts.append(CWhile(cond, body_stmts))
                return stmts

            case ForIn(vars, iterator, body, scope_id):
                is_kv_iterator = len(vars) > 1

                # Determine iterator type
                iter_func = None
                table_expr = iterator
                if isinstance(iterator, FunctionCall) and isinstance(iterator.func, NameRef):
                    iter_func = iterator.func.name
                    if iter_func in ("all", "pairs", "ipairs") and len(iterator.args) > 0:
                        table_expr = iterator.args[0]

                iter_var = "_iter"
                idx_var = "__i"
                stmts = []

                # TValue_t gc _iter = table;
                iter_expr = self._lower_expr(table_expr)
                stmts.append(CDeclare(CVar(iter_var, TVALUE, CVarQualifier.GC), iter_expr))

                if is_kv_iterator:
                    if iter_func == "pairs":
                        # pairs(t): iterate all internal slots
                        # Table_t* _tab = GETTAB(_iter);
                        tab_var = "_tab"
                        stmts.append(CDeclare(
                            CVar(tab_var, CType("Table_t", is_pointer=True)),
                            CFunctionCall("GETTAB", [CVarRef(CVar(iter_var, TVALUE))]),
                            direct_init=True
                        ))

                        # uint16_t max = _tab->kvp.len;
                        max_var = self._new_temp()
                        stmts.append(CDeclare(
                            CVar(max_var, CType("uint16_t")),
                            CLiteral(f"{tab_var}->kvp.len", CType("uint16_t")),
                            direct_init=True
                        ))

                        # Body: TValue_t k = _get_key_at(_tab, __i);
                        #       if(k.tag != NUL) { TValue_t v = _get_val_at(_tab, __i); ... }
                        for_body = []

                        # Declare key (no gc - just reading from table internal storage)
                        key_var = vars[0]
                        for_body.append(CDeclare(
                            CVar(key_var, TVALUE),
                            CLiteral(f"_get_key_at({tab_var}, {idx_var})", TVALUE),
                            direct_init=True
                        ))

                        # if(k.tag != NUL) { ... }
                        if_condition = CBinOp("!=",
                                             CFieldAccess(CVarRef(CVar(key_var, TVALUE)), "tag"),
                                             CLiteral("NUL", CType("int")))

                        if_body = []
                        # Declare value (no gc - just reading from table internal storage)
                        val_var = vars[1] if len(vars) > 1 else "_"
                        if_body.append(CDeclare(
                            CVar(val_var, TVALUE),
                            CLiteral(f"_get_val_at({tab_var}, {idx_var})", TVALUE),
                            direct_init=True
                        ))

                        # Original loop body
                        if_body.extend(self._lower_block(body))

                        for_body.append(CIf(if_condition, if_body, []))

                        stmts.append(self._make_for_loop(idx_var, CType("uint16_t"), "0", max_var, "<", for_body))
                    else:
                        # ipairs(t) with k,v: iterate sequential numeric keys
                        max_var = self._new_temp()
                        stmts.append(CDeclare(
                            CVar(max_var, CType("int16_t")),
                            CFunctionCall("_sequential_until", [CVarRef(CVar(iter_var, TVALUE))]),
                            direct_init=True
                        ))

                        # Body: TValue_t gc k = TNUM(__i); TValue_t gc v = get_tabvalue(_iter, k);
                        for_body = []
                        key_var = vars[0]
                        val_var = vars[1]

                        # k = TNUM(__i)
                        for_body.append(CDeclare(
                            CVar(key_var, TVALUE, CVarQualifier.GC),
                            CLiteral(f"TNUM({idx_var})", TVALUE)
                        ))

                        # v = get_tabvalue(_iter, k)
                        for_body.append(CDeclare(
                            CVar(val_var, TVALUE, CVarQualifier.GC),
                            CFunctionCall("get_tabvalue", [CVarRef(CVar(iter_var, TVALUE)), CVarRef(CVar(key_var, TVALUE))])
                        ))

                        # Original loop body
                        for_body.extend(self._lower_block(body))

                        stmts.append(self._make_for_loop(idx_var, CType("int16_t"), "1", max_var, "<=", for_body))

                else:
                    # all(t) or ipairs(t): iterate sequential numeric keys
                    # int16_t max = _sequential_until(_iter);
                    max_var = self._new_temp()
                    stmts.append(CDeclare(
                        CVar(max_var, CType("int16_t")),
                        CFunctionCall("_sequential_until", [CVarRef(CVar(iter_var, TVALUE))]),
                        direct_init=True
                    ))

                    # Body: TValue_t gc item = get_tabvalue(_iter, TNUM(__i));
                    for_body = []
                    item_var = vars[0]
                    for_body.append(CDeclare(
                        CVar(item_var, TVALUE, CVarQualifier.GC),
                        CFunctionCall("get_tabvalue", [
                            CVarRef(CVar(iter_var, TVALUE)),
                            CLiteral(f"TNUM({idx_var})", TVALUE)
                        ])
                    ))

                    # Original loop body
                    for_body.extend(self._lower_block(body))

                    stmts.append(self._make_for_loop(idx_var, CType("int16_t"), "1", max_var, "<=", for_body))

                return [CBlock(stmts)]

            case Return(values):
                if not values:
                    return [CReturn(CLiteral("T_NULL", TVALUE))]
                elif len(values) == 1:
                    return [CReturn(self._lower_expr(values[0]))]
                else:
                    # Multiple returns: create array
                    # TODO: handle multiple returns properly
                    return [CReturn(self._lower_expr(values[0]))]

            case ExprStmt(expr):
                lowered = self._lower_expr(expr)
                return [CExprStmt(lowered, needs_cleanup=self._needs_cleanup(lowered))]

            case FunctionCall(func, args):
                lowered = self._lower_expr(stmt)
                return [CExprStmt(lowered, needs_cleanup=self._needs_cleanup(lowered))]

            case MethodCall(obj, method, args):
                lowered = self._lower_expr(stmt)
                return [CExprStmt(lowered, needs_cleanup=self._needs_cleanup(lowered))]

        return []

    def _lower_expr(self, expr: Expr) -> CExpr:
        """Lower an expression to C IR"""
        match expr:
            case NameRef(name, resolved):
                return CVarRef(CVar(name, TVALUE))

            case Number(value):
                # Check if it's a hexadecimal floating point literal (e.g., 0x0.8000)
                if value.lower().startswith('0x') and '.' in value:
                    parts = value.split('.')
                    integer_part = parts[0]
                    frac_part = parts[1].lower().rstrip('f')

                    int_val = int(integer_part, 16)
                    # Fractional part is left-aligned in 16 bits
                    # 0x1 becomes 0x1000, 0x8000 stays 0x8000
                    frac_bits = int(frac_part, 16) << (16 - len(frac_part) * 4)

                    return CLiteral(f"TNUM(fix32_from_parts({int_val}, 0x{frac_bits:04x}))", TVALUE)

                elif '.' in value or 'e' in value.lower():
                    return CLiteral(f"TNUM(fix32_from_float({value}f))", TVALUE)
                else:
                    return CLiteral(f"TNUM({value})", TVALUE)

            case String(value):
                # Use hoisted string constant
                var_name = self._get_string_constant(value)
                return CVarRef(CVar(var_name, TVALUE))

            case Bool(value):
                # Use compile-time constants T_TRUE/T_FALSE
                return CLiteral("T_TRUE" if value else "T_FALSE", TVALUE)

            case Nil():
                return CLiteral("T_NULL", TVALUE)

            case BinOp(op, left, right):
                left_expr = self._lower_expr(left)
                right_expr = self._lower_expr(right)
                return self._lower_binop(op, left_expr, right_expr)

            case UnOp(op, operand):
                # Optimize unary minus on numeric literals at compile time
                if op == '-' and isinstance(operand, Number):
                    value = operand.value
                    # Add minus sign to the literal
                    # Check if it's a hexadecimal floating point literal
                    if value.lower().startswith('0x') and '.' in value:
                        parts = value.split('.')
                        integer_part = parts[0]
                        frac_part = parts[1].lower().rstrip('f')

                        int_val = int(integer_part, 16)
                        frac_bits = int(frac_part, 16) << (16 - len(frac_part) * 4)

                        return CLiteral(f"TNUM(fix32_from_parts(-{int_val}, 0x{frac_bits:04x}))", TVALUE)

                    elif '.' in value or 'e' in value.lower():
                        return CLiteral(f"TNUM(fix32_from_float(-{value}f))", TVALUE)
                    else:
                        return CLiteral(f"TNUM(-{value})", TVALUE)

                operand_expr = self._lower_expr(operand)
                return self._lower_unop(op, operand_expr)

            case TableConstructor(fields):
                # After normalization, all tables are hoisted with fields=[]
                assert not fields, "Non-empty tables should be normalized away"
                return CFunctionCall("make_table", [CLiteral("0", INT)])

            case TableAccess(table, key, is_dot):
                table_expr = self._lower_expr(table)
                key_expr = self._lower_expr(key)
                return CFunctionCall("get_tabvalue", [table_expr, key_expr])

            case FunctionCall(func, args):
                arg_exprs = [self._lower_expr(arg) for arg in args]

                if isinstance(func, NameRef) and func.name in self.direct_call_builtins:
                    return CFunctionCall(func.name, arg_exprs)

                func_expr = self._lower_expr(func)
                return CFunctionCall("CALL", [func_expr] + arg_exprs)

            case MethodCall(obj, method, args):
                obj_expr = self._lower_expr(obj)
                method_key = CLiteral(f'TSTR("{method}")', TVALUE)
                func_expr = CFunctionCall("get_tabvalue", [obj_expr, method_key])
                arg_exprs = [obj_expr] + [self._lower_expr(arg) for arg in args]
                return CFunctionCall("CALL", [func_expr] + arg_exprs)

            case AnonymousFunction(params, body, scope_id):
                # Generate unique name for anonymous function
                anon_name = f"_anon_{self.next_temp}"
                self.next_temp += 1

                # Get captures
                captures = []
                if scope_id is not None:
                    scope = self.scopes[scope_id]
                    captures = [v.name for v in scope.captures]

                # Lower function body
                prev_scope = self.current_scope_id
                self.current_scope_id = scope_id
                func_body = self._lower_block(body)
                self.current_scope_id = prev_scope

                # Create function def
                func_def = CFunctionDef(
                    name=anon_name,
                    params=params,
                    body=func_body,
                    captures=captures
                )
                self.c_functions.append(func_def)

                # Return reference to function
                return CLiteral(f"/* closure {anon_name} */", TVALUE)

        assert False, f"{expr} unhandled"

    def _lower_binop(self, op: str, left: CExpr, right: CExpr) -> CExpr:
        """Lower binary operation to runtime call"""
        op_map = {
            '+': '_add',
            '-': '_sub',
            '*': '_mult',
            '/': '_div',
            '%': '_mod',
            '\\': '_floor_div',
            '^': '_pow',
            '..': '_concat',
            '==': '_equal',
            '~=': '_notequal',
            '<': '_lt',
            '<=': '_leq',
            '>': '_gt',
            '>=': '_geq',
            'and': '_and',
            'or': '_or',
        }

        func_name = op_map.get(op, f'_binop_{op}')
        return CFunctionCall(func_name, [left, right])

    def _lower_unop(self, op: str, operand: CExpr) -> CExpr:
        """Lower unary operation to runtime call"""
        op_map = {
            '-': '_invert_sign',
            'not': '_not',
            '#': '_length',
        }

        func_name = op_map.get(op, f'_unop_{op}')
        return CFunctionCall(func_name, [operand])
