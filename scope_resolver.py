from ast_nodes import *


class ScopeResolver:
    """Pass 1: Build scopes and resolve all NameRef nodes"""

    def __init__(self, ast: Block):
        self.scopes: dict[int, Scope] = {}
        self.scope_stack: list[Scope] = []
        self.next_scope_id = 0
        self.global_scope = self._push_scope(is_function=False)
        self.ast = ast

    def analyze(self) -> tuple[dict[int, Scope], Scope]:
        self._visit_block(self.ast)
        self._pop_scope()
        return self.scopes, self.global_scope

    def _push_scope(self, is_function=False, is_loop=False) -> Scope:
        parent = self.scope_stack[-1] if self.scope_stack else None
        scope = Scope(
            scope_id=self.next_scope_id,
            parent=parent,
            is_function=is_function,
            is_loop=is_loop
        )
        self.scopes[scope.scope_id] = scope
        self.scope_stack.append(scope)
        self.next_scope_id += 1
        return scope

    def _pop_scope(self):
        self.scope_stack.pop()

    def _current_scope(self) -> Scope:
        return self.scope_stack[-1]

    def _declare_var(self, name: str, kind: VarKind = VarKind.LOCAL) -> VarInfo:
        """Declare a local variable in current scope"""
        scope = self._current_scope()
        var = VarInfo(
            name=name,
            scope_id=scope.scope_id,
            kind=kind
        )
        scope.vars[name] = var
        return var

    def _resolve_var(self, name: str) -> VarInfo:
        """Look up variable in scope chain"""
        for scope in reversed(self.scope_stack):
            if name in scope.vars:
                return scope.vars[name]

        if name not in self.global_scope.vars:
            self.global_scope.vars[name] = VarInfo(
                name=name,
                scope_id=self.global_scope.scope_id,
                kind=VarKind.GLOBAL
            )
        return self.global_scope.vars[name]

    def _visit_block(self, block: Block):
        for stmt in block.stmts:
            self._visit_stmt(stmt)

    def _visit_stmt(self, stmt: Stmt):
        match stmt:
            case LocalDecl(names, values):
                for val in values:
                    self._visit_expr(val)
                for name in names:
                    self._declare_var(name)

            case Assign(targets, values):
                for val in values:
                    self._visit_expr(val)
                for target in targets:
                    self._visit_expr(target)

            case CompoundAssign(target, op, value):
                self._visit_expr(value)
                self._visit_expr(target)

            case FunctionDef(name_parts, is_method, params, body, scope_id):
                func_name = name_parts[0]
                current_scope = self._current_scope()
                if current_scope == self.global_scope:
                    self._declare_var(func_name, VarKind.GLOBAL)
                else:
                    self._declare_var(func_name)

                func_scope = self._push_scope(is_function=True)
                object.__setattr__(stmt, 'scope_id', func_scope.scope_id)

                for param in params:
                    self._declare_var(param, VarKind.PARAM)

                self._visit_block(body)

                self._pop_scope()

            case If(condition, then_block, elseif_parts, else_block):
                self._visit_expr(condition)
                self._visit_block(then_block)
                for elif_cond, elif_block in elseif_parts:
                    self._visit_expr(elif_cond)
                    self._visit_block(elif_block)
                if else_block:
                    self._visit_block(else_block)

            case ForNum(var, start, stop, step, body, scope_id):
                loop_scope = self._push_scope(is_loop=True)
                object.__setattr__(stmt, 'scope_id', loop_scope.scope_id)

                self._declare_var(var)

                self._visit_expr(start)
                self._visit_expr(stop)
                if step:
                    self._visit_expr(step)

                self._visit_block(body)

                self._pop_scope()

            case ForIn(vars, iterator, body, scope_id):
                loop_scope = self._push_scope(is_loop=True)
                object.__setattr__(stmt, 'scope_id', loop_scope.scope_id)

                for var in vars:
                    self._declare_var(var)

                self._visit_expr(iterator)
                self._visit_block(body)

                self._pop_scope()

            case While(condition, body):
                self._visit_expr(condition)
                self._visit_block(body)

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
            case NameRef(name) as ref:
                var_info = self._resolve_var(name)
                object.__setattr__(ref, 'resolved', var_info)

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
                anon_scope = self._push_scope(is_function=True)
                object.__setattr__(expr, 'scope_id', anon_scope.scope_id)

                for param in params:
                    self._declare_var(param, VarKind.PARAM)

                self._visit_block(body)

                self._pop_scope()

            case Number(_) | String(_) | Bool(_) | Nil():
                pass
