from ast_nodes import *


class CaptureDetector:
    """Pass 2: Detect captured variables"""

    def __init__(self, scopes: dict[int, Scope], global_scope: Scope):
        self.scopes = scopes
        self.global_scope = global_scope
        self.current_scope_stack: list[int] = []

    def analyze(self, ast: Block):
        self.current_scope_stack = [self.global_scope.scope_id]
        self._visit_block(ast)

    def _current_scope(self) -> Scope:
        return self.scopes[self.current_scope_stack[-1]]

    def _push_scope(self, scope_id: int):
        self.current_scope_stack.append(scope_id)

    def _pop_scope(self):
        self.current_scope_stack.pop()

    def _check_var_reference(self, var_info: VarInfo):
        """Check if a variable reference needs to be captured.

        Variables are only captured when accessed across FUNCTION boundaries,
        not just block scope boundaries (if/else/while).
        """
        if var_info.kind == VarKind.GLOBAL:
            return

        current_scope = self._current_scope()

        if var_info.scope_id != current_scope.scope_id:
            # Walk up the scope stack to find where the variable is defined
            found_function_boundary = False
            for scope_id in reversed(self.current_scope_stack):
                scope = self.scopes[scope_id]

                # If we found where the variable is defined, stop
                if scope.scope_id == var_info.scope_id:
                    break

                # Only mark as captured if we cross a function boundary
                if scope.is_function:
                    found_function_boundary = True
                    scope.captures.add(var_info)
                    break

    def _visit_block(self, block: Block):
        for stmt in block.stmts:
            self._visit_stmt(stmt)

    def _visit_stmt(self, stmt: Stmt):
        match stmt:
            case LocalDecl(names, values):
                for val in values:
                    self._visit_expr(val)

            case Assign(targets, values):
                for val in values:
                    self._visit_expr(val)
                for target in targets:
                    self._visit_expr(target)

            case CompoundAssign(target, op, value):
                self._visit_expr(value)
                self._visit_expr(target)

            case FunctionDef(name_parts, is_method, params, body, scope_id):
                if scope_id is not None:
                    self._push_scope(scope_id)
                    self._visit_block(body)
                    self._pop_scope()

            case If(condition, then_block, elseif_parts, else_block, then_scope_id, elseif_scope_ids, else_scope_id):
                self._visit_expr(condition)

                # Visit then block with its scope
                if then_scope_id is not None:
                    self._push_scope(then_scope_id)
                self._visit_block(then_block)
                if then_scope_id is not None:
                    self._pop_scope()

                # Visit each elseif block with its scope
                for i, (elif_cond, elif_block) in enumerate(elseif_parts):
                    self._visit_expr(elif_cond)
                    if elseif_scope_ids and i < len(elseif_scope_ids):
                        self._push_scope(elseif_scope_ids[i])
                    self._visit_block(elif_block)
                    if elseif_scope_ids and i < len(elseif_scope_ids):
                        self._pop_scope()

                # Visit else block with its scope
                if else_block:
                    if else_scope_id is not None:
                        self._push_scope(else_scope_id)
                    self._visit_block(else_block)
                    if else_scope_id is not None:
                        self._pop_scope()

            case ForNum(var, start, stop, step, body, scope_id):
                self._visit_expr(start)
                self._visit_expr(stop)
                if step:
                    self._visit_expr(step)

                if scope_id is not None:
                    self._push_scope(scope_id)
                    self._visit_block(body)
                    self._pop_scope()

            case ForIn(vars, iterator, body, scope_id):
                self._visit_expr(iterator)

                if scope_id is not None:
                    self._push_scope(scope_id)
                    self._visit_block(body)
                    self._pop_scope()

            case While(condition, body, body_scope_id):
                self._visit_expr(condition)

                # Visit while body with its scope
                if body_scope_id is not None:
                    self._push_scope(body_scope_id)
                self._visit_block(body)
                if body_scope_id is not None:
                    self._pop_scope()

            case Return(values):
                if values:
                    for val in values:
                        self._visit_expr(val)

            case ExprStmt(expr):
                self._visit_expr(expr)

            case FunctionCall(func, args):
                self._visit_expr(stmt)

            case MethodCall(obj, method, args):
                self._visit_expr(stmt)

    def _visit_expr(self, expr: Expr):
        match expr:
            case NameRef(name, resolved):
                if resolved:
                    self._check_var_reference(resolved)

            case BinOp(op, left, right):
                self._visit_expr(left)
                self._visit_expr(right)

            case UnOp(op, operand):
                self._visit_expr(operand)

            case TableConstructor(fields):
                if fields:
                    for field in fields:
                        if field.key:
                            self._visit_expr(field.key)
                        self._visit_expr(field.value)

            case TableAccess(table, key, is_dot):
                self._visit_expr(table)
                self._visit_expr(key)

            case FunctionCall(func, args):
                self._visit_expr(func)
                for arg in args:
                    self._visit_expr(arg)

            case MethodCall(obj, method, args):
                self._visit_expr(obj)
                for arg in args:
                    self._visit_expr(arg)

            case AnonymousFunction(params, body, scope_id):
                if scope_id is not None:
                    self._push_scope(scope_id)
                    self._visit_block(body)
                    self._pop_scope()

            case _:
                pass
