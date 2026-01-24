from ast_nodes import *
from typing import Optional


class ASTNormalizer:
    """Statement-Expression Normalization

    Transforms multi-statement constructs:
     - table constructors, future:
    into statement sequences.

    TODO:
     - anonymous functions
     - multiple returns

    This pass enables a simpler IR lowering with zero special cases.
    """

    def __init__(self, scopes: dict[int, Scope], global_scope: Scope):
        self.scopes = scopes
        self.global_scope = global_scope
        self.current_scope_id: Optional[int] = None
        self.next_temp = 0
        self.next_var_id = 0  # For temp variables
        self.hoisted_stmts: list[Stmt] = []

    def normalize(self, ast: Block) -> Block:
        """Normalize the entire AST"""
        self.current_scope_id = self.global_scope.scope_id
        normalized_stmts = self._normalize_block(ast)
        return Block(stmts=normalized_stmts)

    def _current_scope(self) -> Scope:
        """Get the current scope"""
        if self.current_scope_id is None:
            return self.global_scope
        return self.scopes[self.current_scope_id]

    def _new_temp(self) -> str:
        """Generate a new temporary variable name"""
        name = f"_tmp{self.next_temp}"
        self.next_temp += 1
        return name

    def _declare_temp(self, name: str) -> VarInfo:
        """Declare a temp variable in current scope"""
        scope = self._current_scope()
        var = VarInfo(
            name=name,
            scope_id=scope.scope_id,
            kind=VarKind.LOCAL,
            c_name=name,  # Temp vars already have unique names like _tmp0, _tmp1
            var_id=self.next_var_id
        )
        self.next_var_id += 1
        scope.used_c_names.add(name)
        scope.vars[name] = var
        return var

    def _hoist_table_constructor(self, table: TableConstructor) -> NameRef:
        """Hoist a table constructor to a temp variable with initialization statements

        Returns a NameRef to the temp variable.
        Adds initialization statements to self.hoisted_stmts.
        """
        temp_name = self._new_temp()
        var_info = self._declare_temp(temp_name)

        # Empty table constructor (optimization: no hoisting needed, but we do it for consistency)
        if not table.fields:
            empty_table = TableConstructor(fields=[], size_hint=0)
            self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[empty_table], resolved=[var_info]))
            return NameRef(name=temp_name, resolved=var_info)

        # Create local declaration: local _tmpN = {} with size hint
        num_fields = len(table.fields)
        empty_table = TableConstructor(fields=[], size_hint=num_fields)
        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[empty_table], resolved=[var_info]))

        # For each field, emit assignment statement
        array_index = 1
        for field in table.fields:
            # Normalize field value (may be nested table)
            normalized_value = self._normalize_expr(field.value)

            if field.key is None:
                # Array-style: {1, 2, 3} -> _tmp[1] = 1, _tmp[2] = 2, etc.
                key_expr = Number(str(array_index))
                array_index += 1
            else:
                # Named or computed key: {x=1} or {[expr]=1}
                normalized_key = self._normalize_expr(field.key)
                key_expr = normalized_key

            # Create assignment: _tmpN[key] = value or _tmpN.key = value
            table_ref = NameRef(name=temp_name, resolved=var_info)
            table_access = TableAccess(table=table_ref, key=key_expr, is_dot=False)
            assignment = Assign(targets=[table_access], values=[normalized_value])
            self.hoisted_stmts.append(assignment)

        return NameRef(name=temp_name, resolved=var_info)

    def _normalize_assignment_target(self, target: Expr) -> Expr:
        """Normalize an assignment target, hoisting function calls to temps

        When a target is func().attr, we need to hoist func() to a temp:
          local _tmp = func()
          _tmp.attr = value
        """
        match target:
            case TableAccess(table, key, is_dot):
                # Check if table is a function call - if so, hoist it
                if isinstance(table, FunctionCall | MethodCall):
                    # Hoist the function call to a temp
                    normalized_call = self._normalize_expr(table)

                    # If normalization already hoisted it to a NameRef, use that
                    if isinstance(normalized_call, NameRef):
                        hoisted_ref = normalized_call
                    else:
                        # Create temp for the function call result
                        temp_name = self._new_temp()
                        var_info = self._declare_temp(temp_name)
                        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_call], resolved=[var_info]))
                        hoisted_ref = NameRef(name=temp_name, resolved=var_info)

                    # Recursively normalize the key
                    normalized_key = self._normalize_expr(key)

                    # Return TableAccess with temp variable instead of function call
                    return TableAccess(table=hoisted_ref, key=normalized_key, is_dot=is_dot)
                else:
                    # Regular case: recursively normalize both table and key
                    normalized_table = self._normalize_assignment_target(table)
                    normalized_key = self._normalize_expr(key)
                    return TableAccess(table=normalized_table, key=normalized_key, is_dot=is_dot)

            case _:
                # For other expressions (NameRef, etc.), normalize normally
                return self._normalize_expr(target)

    def _normalize_expr(self, expr: Expr) -> Expr:
        """Normalize an expression, hoisting table constructors to temps"""
        match expr:
            case TableConstructor(fields):
                # Always hoist tables, even empty ones (for consistency)
                return self._hoist_table_constructor(expr)

            case BinOp(op, left, right):
                normalized_left = self._normalize_expr(left)
                normalized_right = self._normalize_expr(right)

                if op in ('and', 'or'):
                    # Transform and/or to If statements with gc-tracked temps
                    # This ensures refcount safety while preserving short-circuit semantics

                    # 1. Create temp for left side and assign it
                    left_temp_name = self._new_temp()
                    left_var_info = self._declare_temp(left_temp_name)
                    self.hoisted_stmts.append(LocalDecl(names=[left_temp_name], values=[normalized_left], resolved=[left_var_info]))
                    left_temp_ref = NameRef(name=left_temp_name, resolved=left_var_info)

                    # 2. Create temp for result (declared but not assigned yet)
                    result_temp_name = self._new_temp()
                    result_var_info = self._declare_temp(result_temp_name)
                    self.hoisted_stmts.append(LocalDecl(names=[result_temp_name], values=[], resolved=[result_var_info]))
                    result_temp_ref = NameRef(name=result_temp_name, resolved=result_var_info)

                    # 3. Create If statement with appropriate branches
                    if op == 'and':
                        # if left_temp then result = right else result = left_temp
                        then_block = Block(stmts=[Assign(targets=[result_temp_ref], values=[normalized_right])])
                        else_block = Block(stmts=[Assign(targets=[result_temp_ref], values=[left_temp_ref])])
                    else:  # op == 'or'
                        # if left_temp then result = left_temp else result = right
                        then_block = Block(stmts=[Assign(targets=[result_temp_ref], values=[left_temp_ref])])
                        else_block = Block(stmts=[Assign(targets=[result_temp_ref], values=[normalized_right])])

                    if_stmt = If(
                        condition=left_temp_ref,
                        then_block=then_block,
                        elseif_parts=[],
                        else_block=else_block
                    )
                    self.hoisted_stmts.append(if_stmt)

                    # 4. Return reference to result temp
                    return result_temp_ref
                else:
                    # For other binary ops, hoist function calls to temps
                    if isinstance(normalized_left, FunctionCall | MethodCall):
                        temp_name = self._new_temp()
                        var_info = self._declare_temp(temp_name)
                        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_left], resolved=[var_info]))
                        normalized_left = NameRef(name=temp_name, resolved=var_info)

                    if isinstance(normalized_right, FunctionCall | MethodCall):
                        temp_name = self._new_temp()
                        var_info = self._declare_temp(temp_name)
                        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_right], resolved=[var_info]))
                        normalized_right = NameRef(name=temp_name, resolved=var_info)

                    return BinOp(op=op, left=normalized_left, right=normalized_right)

            case UnOp(op, operand):
                normalized_operand = self._normalize_expr(operand)
                return UnOp(op=op, operand=normalized_operand)

            case FunctionCall(func, args):
                normalized_func = self._normalize_expr(func)
                normalized_args = []

                for arg in args:
                    normalized_arg = self._normalize_expr(arg)
                    if isinstance(normalized_arg, FunctionCall | MethodCall):
                        temp_name = self._new_temp()
                        var_info = self._declare_temp(temp_name)
                        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_arg], resolved=[var_info]))
                        normalized_args.append(NameRef(name=temp_name, resolved=var_info))
                    else:
                        normalized_args.append(normalized_arg)

                return FunctionCall(func=normalized_func, args=normalized_args)

            case MethodCall(obj, method, args):
                normalized_obj = self._normalize_expr(obj)
                normalized_args = []

                for arg in args:
                    normalized_arg = self._normalize_expr(arg)
                    if isinstance(normalized_arg, FunctionCall | MethodCall):
                        temp_name = self._new_temp()
                        var_info = self._declare_temp(temp_name)
                        self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_arg], resolved=[var_info]))
                        normalized_args.append(NameRef(name=temp_name, resolved=var_info))
                    else:
                        normalized_args.append(normalized_arg)

                return MethodCall(obj=normalized_obj, method=method, args=normalized_args)

            case TableAccess(table, key, is_dot):
                normalized_table = self._normalize_expr(table)
                normalized_key = self._normalize_expr(key)
                return TableAccess(table=normalized_table, key=normalized_key, is_dot=is_dot)

            case AnonymousFunction(params, body, scope_id):
                # Save current scope and hoisted statements
                prev_scope = self.current_scope_id
                prev_hoisted = self.hoisted_stmts
                self.current_scope_id = scope_id
                self.hoisted_stmts = []

                # Normalize function body
                normalized_body_stmts = self._normalize_block(body)
                normalized_body = Block(stmts=normalized_body_stmts)

                # Restore scope and hoisted statements
                self.current_scope_id = prev_scope
                self.hoisted_stmts = prev_hoisted

                # Check if this function has captures
                has_captures = False
                if scope_id is not None and scope_id in self.scopes:
                    scope = self.scopes[scope_id]
                    has_captures = len(scope.captures) > 0

                # If it has captures, hoist it to a temp variable
                # (Closures need statement-level setup with set_closure_arg calls)
                if has_captures:
                    temp_name = self._new_temp()
                    var_info = self._declare_temp(temp_name)

                    normalized_func = AnonymousFunction(params=params, body=normalized_body, scope_id=scope_id)
                    self.hoisted_stmts.append(LocalDecl(names=[temp_name], values=[normalized_func], resolved=[var_info]))

                    return NameRef(name=temp_name, resolved=var_info)
                else:
                    # No captures: can stay inline (TFUN is simple)
                    return AnonymousFunction(params=params, body=normalized_body, scope_id=scope_id)

            case NameRef(_) | Number(_) | String(_) | Bool(_) | Nil():
                # Literals and variable references pass through unchanged
                return expr

            case _:
                # Unknown expression type, return as-is
                return expr

    def _normalize_stmt(self, stmt: Stmt) -> list[Stmt]:
        """Normalize a statement, returning a list of statements (hoisted + normalized)"""
        # Clear hoisted statements accumulator
        self.hoisted_stmts = []

        match stmt:
            case LocalDecl(names, values, resolved):
                # Normalize all values (may hoist table constructors)
                normalized_values = [self._normalize_expr(val) for val in values]
                normalized_stmt = LocalDecl(names=names, values=normalized_values, resolved=resolved)
                return self.hoisted_stmts + [normalized_stmt]

            case Assign(targets, values):
                # Normalize all values and targets
                # Use _normalize_assignment_target for targets to handle func().attr = value
                normalized_values = [self._normalize_expr(val) for val in values]
                normalized_targets = [self._normalize_assignment_target(target) for target in targets]
                normalized_stmt = Assign(targets=normalized_targets, values=normalized_values)
                return self.hoisted_stmts + [normalized_stmt]

            case CompoundAssign(target, op, value):
                normalized_value = self._normalize_expr(value)
                normalized_target = self._normalize_assignment_target(target)
                normalized_stmt = CompoundAssign(target=normalized_target, op=op, value=normalized_value)
                return self.hoisted_stmts + [normalized_stmt]

            case FunctionDef(name_parts, is_method, params, body, scope_id):
                # Save current scope and hoisted statements
                prev_scope = self.current_scope_id
                prev_hoisted = self.hoisted_stmts
                self.current_scope_id = scope_id
                self.hoisted_stmts = []

                # Normalize function body
                normalized_body_stmts = self._normalize_block(body)
                normalized_body = Block(stmts=normalized_body_stmts)

                # Restore scope and hoisted statements
                self.current_scope_id = prev_scope
                self.hoisted_stmts = prev_hoisted

                normalized_stmt = FunctionDef(
                    name=name_parts,
                    is_method=is_method,
                    params=params,
                    body=normalized_body,
                    scope_id=scope_id
                )
                return [normalized_stmt]

            case If(condition, then_block, elseif_parts, else_block):
                normalized_condition = self._normalize_expr(condition)
                # Save hoisted statements from condition before normalizing blocks
                hoisted = self.hoisted_stmts[:]
                self.hoisted_stmts = []

                normalized_then = Block(stmts=self._normalize_block(then_block))

                # Transform elseif parts into nested If statements inside else block
                # This ensures hoisted statements from elseif conditions are placed correctly
                normalized_else = None
                if elseif_parts:
                    # Build nested If from elseif parts (right to left)
                    current_else = else_block
                    for elif_cond, elif_block in reversed(elseif_parts):
                        # Normalize the elseif condition
                        normalized_elif_cond = self._normalize_expr(elif_cond)
                        # Save hoisted statements from this elseif condition
                        elif_hoisted = self.hoisted_stmts[:]
                        self.hoisted_stmts = []

                        # Normalize the elseif block
                        normalized_elif_block = Block(stmts=self._normalize_block(elif_block))

                        # Normalize current else block if it exists
                        normalized_current_else = None
                        if current_else:
                            normalized_current_else = Block(stmts=self._normalize_block(current_else))

                        # Create nested If statement with hoisted statements prepended
                        nested_if = If(
                            condition=normalized_elif_cond,
                            then_block=normalized_elif_block,
                            elseif_parts=[],
                            else_block=normalized_current_else
                        )
                        # Wrap in a Block with hoisted statements
                        current_else = Block(stmts=elif_hoisted + [nested_if])

                    normalized_else = current_else
                elif else_block:
                    normalized_else = Block(stmts=self._normalize_block(else_block))

                normalized_stmt = If(
                    condition=normalized_condition,
                    then_block=normalized_then,
                    elseif_parts=[],
                    else_block=normalized_else
                )
                return hoisted + [normalized_stmt]

            case While(condition, body):
                normalized_condition = self._normalize_expr(condition)
                # Save hoisted statements from condition before normalizing body
                hoisted = self.hoisted_stmts[:]

                normalized_body = Block(stmts=self._normalize_block(body))
                normalized_stmt = While(condition=normalized_condition, body=normalized_body)
                return hoisted + [normalized_stmt]

            case ForNum(var, start, stop, step, body, scope_id):
                # Save current scope
                prev_scope = self.current_scope_id
                self.current_scope_id = scope_id

                normalized_start = self._normalize_expr(start)
                normalized_stop = self._normalize_expr(stop)
                normalized_step = self._normalize_expr(step) if step else None
                # Save hoisted statements from expressions before normalizing body
                hoisted = self.hoisted_stmts[:]

                normalized_body = Block(stmts=self._normalize_block(body))

                # Restore scope
                self.current_scope_id = prev_scope

                normalized_stmt = ForNum(
                    var=var,
                    start=normalized_start,
                    stop=normalized_stop,
                    step=normalized_step,
                    body=normalized_body,
                    scope_id=scope_id
                )
                return hoisted + [normalized_stmt]

            case ForIn(vars, iterator, body, scope_id):
                # Save current scope
                prev_scope = self.current_scope_id
                self.current_scope_id = scope_id

                normalized_iterator = self._normalize_expr(iterator)
                # Save hoisted statements from iterator before normalizing body
                hoisted = self.hoisted_stmts[:]

                normalized_body = Block(stmts=self._normalize_block(body))

                # Restore scope
                self.current_scope_id = prev_scope

                normalized_stmt = ForIn(
                    vars=vars,
                    iterator=normalized_iterator,
                    body=normalized_body,
                    scope_id=scope_id
                )
                return hoisted + [normalized_stmt]

            case Return(values):
                if values is None:
                    normalized_values = None
                else:
                    normalized_values = [self._normalize_expr(val) for val in values]
                normalized_stmt = Return(values=normalized_values)
                return self.hoisted_stmts + [normalized_stmt]

            case ExprStmt(expr):
                normalized_expr = self._normalize_expr(expr)
                normalized_stmt = ExprStmt(expr=normalized_expr)
                return self.hoisted_stmts + [normalized_stmt]

            case FunctionCall(func, args):
                # Raw function call as statement (not wrapped in ExprStmt)
                normalized_expr = self._normalize_expr(stmt)
                return self.hoisted_stmts + [normalized_expr]

            case MethodCall(obj, method, args):
                # Raw method call as statement (not wrapped in ExprStmt)
                normalized_expr = self._normalize_expr(stmt)
                return self.hoisted_stmts + [normalized_expr]

            case _:
                # Unknown statement type, return as-is
                return [stmt]

    def _normalize_block(self, block: Block) -> list[Stmt]:
        """Normalize a block of statements"""
        normalized = []
        for stmt in block.stmts:
            normalized.extend(self._normalize_stmt(stmt))

        # Post-process to count additional field assignments and update size hints
        normalized = self._update_table_size_hints(normalized)
        return normalized

    def _update_table_size_hints(self, stmts: list[Stmt]) -> list[Stmt]:
        """Count static field assignments after table creation and update size hints"""
        # Track which variables are table temps and their initial size
        table_temps = {}  # temp_name -> (stmt_index, initial_size)
        temp_to_final = {}  # temp_name -> final_var_name

        # First pass: identify table temps and their mappings
        for i, stmt in enumerate(stmts):
            match stmt:
                case LocalDecl(names=[name], values=[TableConstructor(fields=[], size_hint=size)], resolved=resolved):
                    # This is a hoisted table temp
                    table_temps[name] = [i, size if size else 0, 0, resolved]  # [index, initial_size, additional_count, resolved]
                case LocalDecl(names=[final_name], values=[NameRef(name=temp_name)], resolved=_) if temp_name in table_temps:
                    # This maps temp to final variable
                    temp_to_final[temp_name] = final_name

        # Second pass: count field assignments to final variables
        for stmt in stmts:
            match stmt:
                case Assign(targets=[TableAccess(table=NameRef(name=var_name), key=_, is_dot=is_dot)], values=_):
                    # Check if this is a static field assignment (dot access or constant key)
                    if is_dot:
                        # Find which temp this variable came from
                        for temp_name, final_name in temp_to_final.items():
                            if var_name == final_name and temp_name in table_temps:
                                table_temps[temp_name][2] += 1  # Increment additional count

        # Third pass: rebuild statements with updated size hints
        result = []
        for i, stmt in enumerate(stmts):
            match stmt:
                case LocalDecl(names=[name], values=[TableConstructor(fields=[], size_hint=old_size)], resolved=_) if name in table_temps:
                    _, initial_size, additional, resolved = table_temps[name]
                    new_size = initial_size + additional
                    new_constructor = TableConstructor(fields=[], size_hint=new_size)
                    result.append(LocalDecl(names=[name], values=[new_constructor], resolved=resolved))
                case _:
                    result.append(stmt)

        return result
