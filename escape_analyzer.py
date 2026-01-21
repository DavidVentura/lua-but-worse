from ast_nodes import *


class EscapeAnalyzer:
    """
    Pass 3: Determine which variables escape

    Conservative analysis: A variable escapes if it:
    1. Is returned from a function
    2. Is passed as an argument to a function
    3. Is assigned to or captured by any closure

    Future refinements could track:
    - Which specific closures escape (closures that don't escape don't cause captures to escape)
    - Safe builtins that don't store their arguments
    - Escape propagation through function calls (instead of any func call = escape)
    - Escape propagation through assignment chains
    """

    def __init__(self, scopes: dict[int, Scope], global_scope: Scope):
        self.scopes = scopes
        self.global_scope = global_scope
        self.escaping_vars: set[VarInfo] = set()

    def analyze(self, ast: Block):
        self._visit_block(ast)

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
                    self._mark_closure_captures_as_escaping(scope_id)
                self._visit_block(body)

            case If(condition, then_block, elseif_parts, else_block):
                self._visit_expr(condition)
                self._visit_block(then_block)
                for elif_cond, elif_block in elseif_parts:
                    self._visit_expr(elif_cond)
                    self._visit_block(elif_block)
                if else_block:
                    self._visit_block(else_block)

            case ForNum(var, start, stop, step, body, scope_id):
                self._visit_expr(start)
                self._visit_expr(stop)
                if step:
                    self._visit_expr(step)
                self._visit_block(body)

            case ForIn(vars, iterator, body, scope_id):
                self._visit_expr(iterator)
                self._visit_block(body)

            case While(condition, body):
                self._visit_expr(condition)
                self._visit_block(body)

            case Return(values):
                if values:
                    for val in values:
                        self._mark_expr_as_escaping(val)
                        self._visit_expr(val)

            case ExprStmt(expr):
                self._visit_expr(expr)

            case FunctionCall(func, args):
                self._visit_expr(func)
                for arg in args:
                    self._mark_expr_as_escaping(arg)
                    self._visit_expr(arg)

            case MethodCall(obj, method, args):
                self._visit_expr(obj)
                for arg in args:
                    self._mark_expr_as_escaping(arg)
                    self._visit_expr(arg)

    def _visit_expr(self, expr: Expr):
        match expr:
            case NameRef(name, resolved):
                pass

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
                    self._mark_expr_as_escaping(arg)
                    self._visit_expr(arg)

            case MethodCall(obj, method, args):
                self._visit_expr(obj)
                for arg in args:
                    self._mark_expr_as_escaping(arg)
                    self._visit_expr(arg)

            case AnonymousFunction(params, body, scope_id):
                if scope_id is not None:
                    self._mark_closure_captures_as_escaping(scope_id)
                self._visit_block(body)

            case _:
                pass

    def _mark_closure_captures_as_escaping(self, scope_id: int):
        """Mark all variables captured by this closure as escaping

        Future refinement: Only mark as escaping if the closure itself escapes
        (e.g., is returned, passed to a function, or stored in a table)
        """
        scope = self.scopes[scope_id]
        for captured_var in scope.captures:
            self.escaping_vars.add(captured_var)

    def _mark_expr_as_escaping(self, expr: Expr):
        """Mark variables referenced in this expression as escaping"""
        match expr:
            case NameRef(name, resolved):
                if resolved and resolved.kind != VarKind.GLOBAL:
                    self.escaping_vars.add(resolved)

            case BinOp(op, left, right):
                self._mark_expr_as_escaping(left)
                self._mark_expr_as_escaping(right)

            case UnOp(op, operand):
                self._mark_expr_as_escaping(operand)

            case TableAccess(table, key, is_dot):
                self._mark_expr_as_escaping(table)
                self._mark_expr_as_escaping(key)

            case _:
                pass
