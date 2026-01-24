import string
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
        self.no_return_builtins = {"printh", "set_tabvalue", "setmetatable", "foreach", "__internal_debug_assert_eq"}
        self.underscore_builtins = {"sqrt", "ceil", "sin", "cos", "atan2", "abs", "time", "min", "max"}
        self.direct_call_builtins = {"flr", "printh", "setmetatable", "getmetatable", "all", "pairs", "ipairs", "add", "del", "foreach", "__internal_debug_assert_eq", "__internal_debug_tables_used", "tostring"} | self.underscore_builtins
        self.string_constants: dict[str, str] = {}  # value -> var_name mapping
        self.capture_indices: dict[tuple[int, str], str] = {}  # (scope_id, var_name) -> capture_idx_var
        self.captured_ptr_vars: dict[int, set[str]] = {}  # scope_id -> set of captured pointer var names

    def _get_string_constant(self, value: str) -> str:
        """Get or create a global variable name for a string constant"""
        if value in self.string_constants:
            return self.string_constants[value]

        # Create a safe variable name from the string
        # Use first few chars if alphanumeric, otherwise use generic name
        _safe_chars = ''.join(string.ascii_letters + string.digits + '_')
        safe_chars = ''.join(c if c in _safe_chars else '_' for c in value[:8])
        if not safe_chars:
            safe_chars = 'str'
        else:
         if not safe_chars[0].isalpha() and safe_chars[0] != '_':
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

    def _get_captured_vars_in_scope(self, scope_id: int) -> set[str]:
        """Get all variables from this scope that are captured by any descendant functions"""
        if scope_id is None:
            return set()

        captured_vars = set()

        # Look at ALL scopes (not just direct children) and see what they capture from this scope
        for other_scope_id, other_scope in self.scopes.items():
            if other_scope.is_function:  # Only check function scopes
                for captured_var in other_scope.captures:
                    if captured_var.scope_id == scope_id:
                        captured_vars.add(captured_var.name)

        return captured_vars

    def _get_var_type(self, name: str) -> CType:
        """Get the type of a variable (TVALUE or TValue_t* for captured pointers)"""
        if self.current_scope_id in self.captured_ptr_vars and name in self.captured_ptr_vars[self.current_scope_id]:
            return CType("TValue_t*")
        return TVALUE

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

    def _get_captured_params(self, scope_id: int, params: list[str]) -> list[str]:
        """Get list of parameters that are captured by nested functions"""
        if scope_id is None:
            return []

        captured_params = []
        # Look at ALL function scopes to see what they capture from this scope
        for other_scope_id, other_scope in self.scopes.items():
            if other_scope.is_function:
                for captured_var in other_scope.captures:
                    if captured_var.scope_id == scope_id and captured_var.name in params:
                        if captured_var.name not in captured_params:
                            captured_params.append(captured_var.name)
        return captured_params

    def _generate_captured_param_setup(self, params: list[str], captured_params: list[str]) -> list[CStmt]:
        """Generate statements to move captured parameters into the captured arena"""
        stmts = []
        for i, param in enumerate(params):
            if param in captured_params:
                # This parameter is captured - extract from args and store in arena
                cap_idx_var = f"_cap_idx_{param}"
                self.capture_indices[(self.current_scope_id, param)] = cap_idx_var

                # Track that this variable is a pointer
                if self.current_scope_id not in self.captured_ptr_vars:
                    self.captured_ptr_vars[self.current_scope_id] = set()
                self.captured_ptr_vars[self.current_scope_id].add(param)

                # Extract parameter from args
                param_extract = CLiteral(f"(args.num > {i}) ? args.elems[{i}] : T_NULL", TVALUE)

                # uint16_t _cap_idx_param = _alloc_captured(extracted_param);
                stmts.append(CDeclare(
                    CVar(cap_idx_var, CType("uint16_t")),
                    CFunctionCall("_alloc_captured", [param_extract]),
                    direct_init=True
                ))

                # TValue_t* param = &_captured.captured[_cap_idx_param].value;
                ptr_expr = CLiteral(f"&_captured.captured[{cap_idx_var}].value", CType("TValue_t*"))
                stmts.append(CDeclare(
                    CVar(param, CType("TValue_t*")),
                    ptr_expr,
                    direct_init=True
                ))

        return stmts

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
                captured_vars = self._get_captured_vars_in_scope(self.current_scope_id)

                for i, name in enumerate(names):
                    value = values[i] if i < len(values) else Nil()

                    # Special handling for anonymous functions with captures
                    if isinstance(value, AnonymousFunction) and value.scope_id is not None:
                        scope = self.scopes[value.scope_id]
                        if scope.captures:
                            # This is a closure - handle it specially
                            anon_name = f"_anon_{self.next_temp}"
                            self.next_temp += 1

                            # Lower the function body
                            prev_scope = self.current_scope_id
                            self.current_scope_id = value.scope_id

                            # Handle captured parameters
                            captured_params = self._get_captured_params(value.scope_id, value.params)
                            param_setup = self._generate_captured_param_setup(value.params, captured_params)

                            func_body = param_setup + self._lower_block(value.body)
                            self.current_scope_id = prev_scope

                            # Create function def
                            func_def = CFunctionDef(
                                name=anon_name,
                                params=value.params,
                                body=func_body,
                                captures=[v.name for v in scope.captures],
                                captured_params=captured_params
                            )
                            self.c_functions.append(func_def)

                            # Generate closure creation (no GC - closures manage their own refcounting)
                            closure_expr = CLiteral(f"TCLOSURE({anon_name}, {len(scope.captures)})", TVALUE)
                            stmts.append(CDeclare(CVar(name, TVALUE), closure_expr, direct_init=True))

                            # Set up captures
                            for idx, captured_var_info in enumerate(scope.captures):
                                cap_var_name = captured_var_info.name
                                cap_scope_id = captured_var_info.scope_id
                                cap_idx_var = self.capture_indices.get((cap_scope_id, cap_var_name))
                                if cap_idx_var is None:
                                    continue

                                call = CFunctionCall("set_closure_arg", [
                                    CVarRef(CVar(name, TVALUE)),
                                    CLiteral(str(idx), CType("uint8_t")),
                                    CVarRef(CVar(cap_idx_var, CType("uint16_t")))
                                ])
                                stmts.append(CExprStmt(call, needs_cleanup=False))

                            continue

                    value_expr = self._lower_expr(value)

                    if name in captured_vars:
                        # This variable is captured by nested functions
                        # Allocate in captured arena
                        cap_idx_var = f"_cap_idx_{name}"
                        self.capture_indices[(self.current_scope_id, name)] = cap_idx_var

                        # Track that this variable is a pointer
                        if self.current_scope_id not in self.captured_ptr_vars:
                            self.captured_ptr_vars[self.current_scope_id] = set()
                        self.captured_ptr_vars[self.current_scope_id].add(name)

                        # uint16_t _cap_idx_varname = _alloc_captured(value);
                        stmts.append(CDeclare(
                            CVar(cap_idx_var, CType("uint16_t")),
                            CFunctionCall("_alloc_captured", [value_expr]),
                            direct_init=True
                        ))

                        # TValue_t* varname = &_captured.captured[_cap_idx_varname].value;
                        # Use CType("TValue_t*") so code generator knows it's a pointer
                        ptr_expr = CLiteral(f"&_captured.captured[{cap_idx_var}].value", CType("TValue_t*"))
                        stmts.append(CDeclare(
                            CVar(name, CType("TValue_t*")),
                            ptr_expr,
                            direct_init=True
                        ))
                    else:
                        # Regular local variable
                        stmts.append(CDeclare(CVar(name, TVALUE, CVarQualifier.GC), value_expr))
                return stmts

            case Assign(targets, values):
                stmts = []
                for i, target in enumerate(targets):
                    value = values[i] if i < len(values) else Nil()

                    # Special handling for anonymous functions with captures
                    if isinstance(value, AnonymousFunction) and value.scope_id is not None:
                        scope = self.scopes[value.scope_id]
                        if scope.captures:
                            # Generate the anonymous function
                            anon_name = f"_anon_{self.next_temp}"
                            self.next_temp += 1

                            # Lower function body
                            prev_scope = self.current_scope_id
                            self.current_scope_id = value.scope_id

                            # Handle captured parameters
                            captured_params = self._get_captured_params(value.scope_id, value.params)
                            param_setup = self._generate_captured_param_setup(value.params, captured_params)

                            func_body = param_setup + self._lower_block(value.body)
                            self.current_scope_id = prev_scope

                            # Create function def
                            func_def = CFunctionDef(
                                name=anon_name,
                                params=value.params,
                                body=func_body,
                                captures=[v.name for v in scope.captures],
                                captured_params=captured_params
                            )
                            self.c_functions.append(func_def)

                            # Get capture indices
                            cap_indices = []
                            for cap_var_info in scope.captures:
                                key = (cap_var_info.scope_id, cap_var_info.name)
                                if key in self.capture_indices:
                                    cap_indices.append(self.capture_indices[key])

                            num_captures = len(cap_indices)

                            # Generate assignment with TCLOSURE
                            closure_expr = CLiteral(f"TCLOSURE({anon_name}, {num_captures})", TVALUE)

                            # Determine target variable name for set_closure_arg calls
                            target_var_name = None
                            match target:
                                case NameRef(name, resolved):
                                    if resolved and resolved.kind == VarKind.GLOBAL:
                                        if name not in self.globals:
                                            self.globals.append(name)
                                    var_type = self._get_var_type(name)
                                    stmts.append(CAssign(CVar(name, var_type), closure_expr))
                                    target_var_name = name

                                case TableAccess(table, key, is_dot):
                                    # For table assignments, we need to create a temp variable
                                    temp_var = f"_tmp{self.next_temp}"
                                    self.next_temp += 1
                                    stmts.append(CDeclare(CVar(temp_var, TVALUE, CVarQualifier.GC), closure_expr))
                                    target_var_name = temp_var

                                    # Then assign to table
                                    table_expr = self._lower_expr(table)
                                    key_expr = self._lower_expr(key)
                                    call = CFunctionCall("set_tabvalue", [table_expr, key_expr, CVarRef(CVar(temp_var, TVALUE))])
                                    stmts.append(CExprStmt(call, needs_cleanup=False))

                            # Generate set_closure_arg calls
                            for idx, cap_idx_var in enumerate(cap_indices):
                                call = CFunctionCall("set_closure_arg", [
                                    CVarRef(CVar(target_var_name, TVALUE)),
                                    CLiteral(str(idx), CType("uint8_t")),
                                    CVarRef(CVar(cap_idx_var, CType("uint16_t")))
                                ])
                                stmts.append(CExprStmt(call, needs_cleanup=False))

                            continue  # Skip normal processing

                    match target:
                        case NameRef(name, resolved):
                            if resolved and resolved.kind == VarKind.GLOBAL:
                                if name not in self.globals:
                                    self.globals.append(name)

                            value_expr = self._lower_expr(value)
                            var_type = self._get_var_type(name)
                            stmts.append(CAssign(CVar(name, var_type), value_expr))

                        case TableAccess(table, key, is_dot):
                            value_expr = self._lower_expr(value)
                            table_expr = self._lower_expr(table)
                            key_expr = self._lower_expr(key)
                            call = CFunctionCall("set_tabvalue", [table_expr, key_expr, value_expr])
                            stmts.append(CExprStmt(call, needs_cleanup=False))
                return stmts

            case CompoundAssign(target, op, value):
                # a += b  ->  a = a + b
                # t.x += b  ->  t.x = t.x + b
                match target:
                    case NameRef(name, resolved):
                        var_type = self._get_var_type(name)
                        left = CVarRef(CVar(name, var_type))
                        right = self._lower_expr(value)
                        result = self._lower_binop(op, left, right)
                        return [CAssign(CVar(name, var_type), result)]
                    case TableAccess(table, key, is_dot):
                        table_expr = self._lower_expr(table)
                        key_expr = self._lower_expr(key)
                        left = CFunctionCall("get_tabvalue", [table_expr, key_expr])
                        right = self._lower_expr(value)
                        result = self._lower_binop(op, left, right)
                        set_call = CFunctionCall("set_tabvalue", [table_expr, key_expr, result])
                        return [CExprStmt(set_call, needs_cleanup=False)]
                    case other:
                        raise ValueError(f"not handled: {other}")

            case FunctionDef(name_parts, is_method, params, body, scope_id):
                base_func_name = "_".join(name_parts)

                # Get captures from scope
                captures = []
                if scope_id is not None:
                    scope = self.scopes[scope_id]
                    captures = [v.name for v in scope.captures]

                # For local functions with captures, use a different C function name to avoid shadowing
                is_local = (self.current_scope_id != self.global_scope.scope_id and len(name_parts) == 1)
                if is_local and captures:
                    func_name = f"{base_func_name}_fn"
                else:
                    func_name = base_func_name

                # Lower function body
                prev_scope = self.current_scope_id
                self.current_scope_id = scope_id

                # Handle captured parameters - move them to captured arena
                captured_params = self._get_captured_params(scope_id, params)
                param_setup = self._generate_captured_param_setup(params, captured_params)

                func_body = param_setup + self._lower_block(body)
                self.current_scope_id = prev_scope

                # Create function def
                func_def = CFunctionDef(
                    name=func_name,
                    params=params,
                    body=func_body,
                    captures=captures,
                    captured_params=captured_params
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

                    # Value is TFUN(func_name) or TCLOSURE if it has captures
                    if captures:
                        # For dotted names with captures, we need to create a temporary closure
                        stmts = []
                        temp = self._new_temp()

                        # Create the closure
                        closure_expr = CLiteral(f"TCLOSURE({func_name}, {len(captures)})", TVALUE)
                        stmts.append(CDeclare(CVar(temp, TVALUE), closure_expr, direct_init=True))

                        # Set up captures
                        for idx, captured_var_info in enumerate(scope.captures):
                            cap_var_name = captured_var_info.name
                            cap_scope_id = captured_var_info.scope_id
                            cap_idx_var = self.capture_indices.get((cap_scope_id, cap_var_name))
                            if cap_idx_var is None:
                                continue

                            call = CFunctionCall("set_closure_arg", [
                                CVarRef(CVar(temp, TVALUE)),
                                CLiteral(str(idx), CType("uint8_t")),
                                CVarRef(CVar(cap_idx_var, CType("uint16_t")))
                            ])
                            stmts.append(CExprStmt(call, needs_cleanup=False))

                        # Assign to table
                        table_expr = self._lower_expr(table)
                        key_expr = self._lower_expr(key)
                        call = CFunctionCall("set_tabvalue", [table_expr, key_expr, CVarRef(CVar(temp, TVALUE))])
                        stmts.append(CExprStmt(call, needs_cleanup=False))
                        return stmts
                    else:
                        func_value = CLiteral(f"TFUN({func_name})", TVALUE)

                    # Generate set_tabvalue call
                    table_expr = self._lower_expr(table)
                    key_expr = self._lower_expr(key)
                    call = CFunctionCall("set_tabvalue", [table_expr, key_expr, func_value])
                    return [CExprStmt(call, needs_cleanup=False)]

                # For simple named functions (not dotted), check if they need closure setup
                if len(name_parts) == 1 and captures:
                    # This is a local or global function with captures
                    # Determine if it's global or local
                    is_global = (self.current_scope_id == self.global_scope.scope_id)
                    var_name = base_func_name

                    stmts = []

                    if is_global:
                        # For global functions, we need to generate assignment to the global var
                        # The global variable declaration will be handled elsewhere
                        closure_expr = CLiteral(f"TCLOSURE({func_name}, {len(captures)})", TVALUE)
                        stmts.append(CAssign(CVar(var_name, TVALUE), closure_expr))
                    else:
                        # For local functions, declare the variable
                        closure_expr = CLiteral(f"TCLOSURE({func_name}, {len(captures)})", TVALUE)
                        stmts.append(CDeclare(CVar(var_name, TVALUE), closure_expr, direct_init=True))

                    # Set up captures
                    for idx, captured_var_info in enumerate(scope.captures):
                        cap_var_name = captured_var_info.name
                        cap_scope_id = captured_var_info.scope_id

                        # Get the capture index variable
                        cap_idx_var = self.capture_indices.get((cap_scope_id, cap_var_name))
                        if cap_idx_var is None:
                            # This shouldn't happen, but handle it gracefully
                            continue

                        call = CFunctionCall("set_closure_arg", [
                            CVarRef(CVar(var_name, TVALUE)),
                            CLiteral(str(idx), CType("uint8_t")),
                            CVarRef(CVar(cap_idx_var, CType("uint16_t")))
                        ])
                        stmts.append(CExprStmt(call, needs_cleanup=False))

                    return stmts

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

                # Check if loop variable is captured
                captured_vars = self._get_captured_vars_in_scope(scope_id) if scope_id else set()

                stmts = []

                # Declare loop variable (captured or regular)
                if var in captured_vars:
                    # Captured loop variable
                    cap_idx_var = f"_cap_idx_{var}"
                    self.capture_indices[(scope_id, var)] = cap_idx_var

                    # Track that this variable is a pointer
                    if scope_id not in self.captured_ptr_vars:
                        self.captured_ptr_vars[scope_id] = set()
                    self.captured_ptr_vars[scope_id].add(var)

                    # Save current scope and switch to loop scope
                    prev_scope = self.current_scope_id
                    self.current_scope_id = scope_id

                    stmts.append(CDeclare(
                        CVar(cap_idx_var, CType("uint16_t")),
                        CFunctionCall("_alloc_captured", [start_expr]),
                        direct_init=True
                    ))
                    ptr_expr = CLiteral(f"&_captured.captured[{cap_idx_var}].value", CType("TValue_t*"))
                    stmts.append(CDeclare(
                        CVar(var, CType("TValue_t*")),
                        ptr_expr,
                        direct_init=True
                    ))
                else:
                    # Regular loop variable
                    stmts.append(CDeclare(CVar(var, TVALUE, CVarQualifier.GC), start_expr))

                stop_var = self._new_temp()
                stmts.append(CDeclare(CVar(stop_var, TVALUE, CVarQualifier.GC), stop_expr))

                # Condition: var <= stop (or >= if step < 0)
                var_type = self._get_var_type(var) if scope_id and scope_id == self.current_scope_id else TVALUE
                cond = CFunctionCall("_leq", [CVarRef(CVar(var, var_type)), CVarRef(CVar(stop_var, TVALUE))])
                cond = CFunctionCall("__bool", [cond])

                body_stmts = self._lower_block(body)
                # Increment: var = var + step
                increment = CFunctionCall("_add", [CVarRef(CVar(var, var_type)), step_expr])
                body_stmts.append(CAssign(CVar(var, var_type), increment))

                # Restore scope if we changed it
                if var in captured_vars:
                    self.current_scope_id = prev_scope

                stmts.append(CWhile(cond, body_stmts))
                # Wrap in a block to avoid variable redefinition across multiple loops
                return [CBlock(stmts)]

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
                return CVarRef(CVar(name, self._get_var_type(name)))

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

                elif value.lower().startswith('0b') and '.' in value:
                    parts = value.split('.')
                    integer_part = parts[0]
                    frac_part = parts[1]

                    return CLiteral(f"TNUM(fix32_from_parts({integer_part}, 0b{frac_part:<016}))", TVALUE)

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

                    elif value.lower().startswith('0b') and '.' in value:
                        parts = value.split('.')
                        integer_part = parts[0]
                        frac_part = parts[1]

                        return CLiteral(f"TNUM(fix32_from_parts(-{integer_part}, 0b{frac_part:<016}))", TVALUE)

                    elif '.' in value or 'e' in value.lower():
                        return CLiteral(f"TNUM(fix32_from_float(-{value}f))", TVALUE)
                    else:
                        return CLiteral(f"TNUM(-{value})", TVALUE)

                operand_expr = self._lower_expr(operand)
                return self._lower_unop(op, operand_expr)

            case TableConstructor(fields, size_hint):
                # After normalization, all tables are hoisted with fields=[]
                assert not fields, "Non-empty tables should be normalized away"
                size = str(size_hint) if size_hint is not None else "0"
                return CFunctionCall("make_table", [CLiteral(size, INT)])

            case TableAccess(table, key, is_dot):
                table_expr = self._lower_expr(table)
                key_expr = self._lower_expr(key)
                return CFunctionCall("get_tabvalue", [table_expr, key_expr])

            case FunctionCall(func, args):
                arg_exprs = [self._lower_expr(arg) for arg in args]

                if isinstance(func, NameRef) and func.name in self.direct_call_builtins:
                    func_name = f"_{func.name}" if func.name in self.underscore_builtins else func.name
                    return CFunctionCall(func_name, arg_exprs)

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

                # Generate closure creation
                if not captures:
                    # No captures: TFUN(func_name)
                    return CLiteral(f"TFUN({anon_name})", TVALUE)
                else:
                    # Has captures - this should have been hoisted by normalizer
                    # But if we get here, error with a helpful message
                    raise AssertionError(f"Anonymous function with captures should have been hoisted by normalizer: {anon_name}")

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
