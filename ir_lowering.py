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
        self.no_return_builtins = {"printh", "set_tabvalue"}
        self.direct_call_builtins = {"flr", "printh"}

    def lower(self, ast: Block) -> tuple[list[str], list[CFunctionDef], set[str]]:
        """Lower the entire AST to IR, returning (globals, functions, escaping_var_names)"""
        self.current_scope_id = self.global_scope.scope_id

        main_body = self._lower_block(ast)
        main_func = CFunctionDef(
            name="_lua_main",
            params=[],
            body=main_body
        )
        self.c_functions.append(main_func)

        escaping_names = {v.name for v in self.escaping_vars}
        return (self.globals, self.c_functions, escaping_names)

    def _new_temp(self) -> str:
        """Generate a new temporary variable name"""
        name = f"tmp_{self.next_temp}"
        self.next_temp += 1
        return name

    def _needs_cleanup(self, expr: CExpr) -> bool:
        """Check if expression result needs GC cleanup"""
        if isinstance(expr, CFunctionCall):
            if expr.func_name in self.no_return_builtins:
                return False
            if expr.func_name == "__call" and len(expr.args) > 0:
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
                    stmts.append(CDeclare(CVar(name, TVALUE), value_expr))
                return stmts

            case Assign(targets, values):
                stmts = []
                for i, target in enumerate(targets):
                    value = values[i] if i < len(values) else Nil()
                    value_expr = self._lower_expr(value)

                    match target:
                        case NameRef(name, resolved):
                            if resolved and resolved.kind == VarKind.GLOBAL:
                                if name not in self.globals:
                                    self.globals.append(name)
                            stmts.append(CAssign(CVar(name, TVALUE), value_expr))
                        case TableAccess(table, key, is_dot):
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

                return []

            case If(condition, then_block, elseif_parts, else_block):
                cond_expr = self._lower_expr(condition)
                cond_expr = CFunctionCall("__bool", [cond_expr])

                then_stmts = self._lower_block(then_block)
                else_stmts = []

                # Handle elseifs as nested ifs in else clause
                for elif_cond, elif_block in elseif_parts:
                    elif_cond_expr = self._lower_expr(elif_cond)
                    elif_cond_expr = CFunctionCall("__bool", [elif_cond_expr])
                    elif_then = self._lower_block(elif_block)
                    else_stmts = [CIf(elif_cond_expr, elif_then, else_stmts)]

                if else_block:
                    else_stmts.extend(self._lower_block(else_block))

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
                stmts.append(CDeclare(CVar(var, TVALUE), start_expr))

                stop_var = self._new_temp()
                stmts.append(CDeclare(CVar(stop_var, TVALUE), stop_expr))

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
                # TODO: Implement iterator protocol
                # For now, just add a comment
                return [CExprStmt(CLiteral("/* for-in not implemented yet */", VOID))]

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
                if '.' in value or 'e' in value.lower():
                    return CLiteral(f"TNUM(fix32_from_float({value}f))", TVALUE)
                else:
                    return CLiteral(f"TNUM({value})", TVALUE)

            case String(value):
                escaped = value.replace('\\', '\\\\').replace('"', '\\"')
                # TSTR macro calls make_str at runtime
                return CLiteral(f'TSTR("{escaped}")', TVALUE)

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
                operand_expr = self._lower_expr(operand)
                return self._lower_unop(op, operand_expr)

            case TableConstructor(fields):
                stmts = []
                table_var = self._new_temp()

                # Create table with size hint
                size_hint = len(fields) if fields else 0
                result = CFunctionCall("make_table", [CLiteral(str(size_hint), INT)])

                # For each field, call set_tabvalue
                # TODO: This needs to be done in statements, not in an expression
                # For now, just return the table creation
                return result

            case TableAccess(table, key, is_dot):
                table_expr = self._lower_expr(table)
                key_expr = self._lower_expr(key)
                return CFunctionCall("get_tabvalue", [table_expr, key_expr])

            case FunctionCall(func, args):
                arg_exprs = [self._lower_expr(arg) for arg in args]

                if isinstance(func, NameRef) and func.name in self.direct_call_builtins:
                    return CFunctionCall(func.name, arg_exprs)

                func_expr = self._lower_expr(func)
                return CFunctionCall("__call", [func_expr] + arg_exprs)

            case MethodCall(obj, method, args):
                obj_expr = self._lower_expr(obj)
                method_key = CLiteral(f'TSTR("{method}")', TVALUE)
                func_expr = CFunctionCall("get_tabvalue", [obj_expr, method_key])
                arg_exprs = [obj_expr] + [self._lower_expr(arg) for arg in args]
                # TODO: Pack args into TVSlice_t before calling __call
                return CFunctionCall("__call", [func_expr] + arg_exprs)

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

        return CLiteral("LUA_NIL", TVALUE)

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
            '==': 'equal',
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
