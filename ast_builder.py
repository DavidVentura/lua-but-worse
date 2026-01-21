from lark import Transformer, Token
from ast_nodes import *


class ASTBuilder(Transformer):
    """Transform Lark parse tree to our AST"""

    def primary(self, items):
        token = items[0]
        if isinstance(token, Token):
            if token.type == 'NUMBER':
                return Number(token.value)
            elif token.type == 'STRING':
                return String(token.value[1:-1])
            elif token.type == 'BOOLEAN':
                return Bool(token.value == 'true')
            elif token.type == 'NIL':
                return Nil()
            elif token.type == 'NAME':
                return NameRef(token.value)
        else:
            return items[0]

    def or_expr(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            result = BinOp('or', result, items[i + 1])
        return result

    def and_expr(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            result = BinOp('and', result, items[i + 1])
        return result

    def not_expr(self, items):
        return UnOp('not', items[1])

    def comparison(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            op = items[i].value
            right = items[i + 1]
            result = BinOp(op, result, right)
        return result

    def concat(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            result = BinOp('..', result, items[i + 1])
        return result

    def add_expr(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            op = items[i].value
            right = items[i + 1]
            result = BinOp(op, result, right)
        return result

    def mult_expr(self, items):
        result = items[0]
        for i in range(1, len(items), 2):
            op = items[i].value
            right = items[i + 1]
            result = BinOp(op, result, right)
        return result

    def unary(self, items):
        op = items[0].value
        return UnOp(op, items[1])

    def atom(self, items):
        """atom: primary suffix*"""
        result = items[0]
        for suffix in items[1:]:
            if isinstance(suffix, tuple):
                suffix_type, suffix_data = suffix
                if suffix_type == 'table_suffix':
                    key, is_dot = suffix_data
                    result = TableAccess(result, key, is_dot)
                elif suffix_type == 'function_call':
                    args, method_info = suffix_data
                    if method_info:
                        method_name, call_args = method_info
                        result = MethodCall(result, method_name, call_args)
                    else:
                        result = FunctionCall(result, args)
        return result

    def table_suffix(self, items):
        """table_suffix: '.' NAME | '[' expr ']'"""
        if isinstance(items[0], Token):
            return ('table_suffix', (String(items[0].value), True))
        else:
            return ('table_suffix', (items[0], False))

    def function_call(self, items):
        """function_call: '(' [expr_list] ')' | ':' NAME '(' [expr_list] ')'"""
        if len(items) >= 2 and isinstance(items[0], Token) and items[0].value == ':':
            method_name = items[1].value
            args = items[2] if len(items) > 2 else []
            return ('function_call', (args, (method_name, args)))
        else:
            args = items[0] if items else []
            return ('function_call', (args, None))

    def expr_list(self, items):
        """expr_list: expr (',' expr)*"""
        return items

    def table_constructor(self, items):
        """table_constructor: '{' [table_fields] '}'"""
        fields = items[0] if items else []
        return TableConstructor(fields)

    def table_fields(self, items):
        """table_fields: table_field (',' table_field)* [',']"""
        return items

    def table_field(self, items):
        """
        table_field: '[' expr ']' '=' expr
                   | NAME '=' expr
                   | expr
        """
        if len(items) == 1:
            return TableField(None, items[0])
        elif isinstance(items[0], Token):
            return TableField(String(items[0].value), items[1])
        else:
            return TableField(items[0], items[1])

    def assignment(self, items):
        """
        assignment: assignable (',' assignable)* '=' expr_list
                  | assignable '+=' expr
                  | assignable '-=' expr
                  | ...
        """
        if len(items) >= 2 and isinstance(items[1], Token) and items[1].value in ['+=', '-=', '*=', '/=', '%=']:
            target = items[0]
            op = items[1].value[:-1]
            value = items[2]
            return CompoundAssign(target, op, value)
        else:
            equals_idx = None
            for i, item in enumerate(items):
                if isinstance(item, list):
                    equals_idx = i
                    break

            if equals_idx is None:
                return ExprStmt(items[0])

            targets = items[:equals_idx]
            values = items[equals_idx]
            return Assign(targets, values)

    def assignable(self, items):
        """assignable: NAME table_suffix*"""
        result = NameRef(items[0].value)
        for suffix in items[1:]:
            if isinstance(suffix, tuple):
                suffix_type, (key, is_dot) = suffix
                result = TableAccess(result, key, is_dot)
        return result

    def local_decl(self, items):
        """local_decl: 'local' NAME (',' NAME)* ['=' expr_list]"""
        names = []
        values = []
        for item in items:
            if isinstance(item, Token):
                names.append(item.value)
            elif isinstance(item, list):
                values = item
        return LocalDecl(names, values)

    def function_def(self, items):
        """
        function_def: 'function' function_name '(' [param_list] ')' block 'end'
                    | 'function' function_name ':' NAME '(' [param_list] ')' block 'end'
        """
        name_parts = items[0]
        idx = 1
        is_method = False
        params = []

        if idx < len(items) and isinstance(items[idx], Token) and items[idx].type == 'NAME':
            is_method = True
            name_parts.append(items[idx].value)
            params = ['self']
            idx += 1

        if idx < len(items) and isinstance(items[idx], list) and all(isinstance(x, str) for x in items[idx]):
            params.extend(items[idx])
            idx += 1

        body = items[idx]

        return FunctionDef(name_parts, is_method, params, body)

    def function_name(self, items):
        """function_name: NAME ('.' NAME)*"""
        return [item.value for item in items if isinstance(item, Token)]

    def param_list(self, items):
        """param_list: NAME (',' NAME)*"""
        return [item.value for item in items]

    def block(self, items):
        """block: statement*"""
        return Block(items)

    def if_stmt(self, items):
        """
        if_stmt: 'if' expr 'then' block elif_parts else_part 'end'
               | 'if' '(' expr ')' statement ['else' statement+]
        """
        condition = items[0]

        if isinstance(items[1], Block):
            then_block = items[1]
            elif_parts = items[2] if len(items) > 2 and isinstance(items[2], list) else []
            else_block = items[3] if len(items) > 3 else None
            return If(condition, then_block, elif_parts, else_block)
        else:
            then_stmt = items[1]
            else_stmts = items[2:] if len(items) > 2 else []

            then_block = Block([then_stmt])
            else_block = Block(else_stmts) if else_stmts else None
            return If(condition, then_block, [], else_block)

    def elif_parts(self, items):
        """elif_parts: ('elseif' expr 'then' block)*"""
        result = []
        for i in range(0, len(items), 2):
            if i + 1 < len(items):
                condition = items[i]
                block = items[i + 1]
                result.append((condition, block))
        return result

    def else_part(self, items):
        """else_part: ['else' block]"""
        return items[0] if items else None

    def for_stmt(self, items):
        """
        for_stmt: 'for' NAME '=' expr ',' expr [',' expr] 'do' block 'end'
                | 'for' NAME (',' NAME)* 'in' expr 'do' block 'end'

        Numeric for receives: [NAME, start_expr, stop_expr, step_expr_or_None, block]
        Iterator for receives: [NAME, NAME*, iterator_expr, block]
        """
        var = items[0].value

        if isinstance(items[1], Token):
            vars = [var]
            idx = 1
            while isinstance(items[idx], Token):
                vars.append(items[idx].value)
                idx += 1
            iterator = items[idx]
            body = items[idx + 1]
            return ForIn(vars, iterator, body)
        else:
            start = items[1]
            stop = items[2]
            step = items[3]
            body = items[4]
            return ForNum(var, start, stop, step, body)

    def while_stmt(self, items):
        """
        while_stmt: 'while' expr 'do' block 'end'
                  | 'while' '(' expr ')' statement
        """
        condition = items[0]
        if isinstance(items[1], Block):
            body = items[1]
        else:
            body = Block([items[1]])
        return While(condition, body)

    def return_stmt(self, items):
        """return_stmt: 'return' [expr_list]"""
        values = items[0] if items else []
        return Return(values)

    def anonymous_function(self, items):
        """anonymous_function: 'function' '(' [param_list] ')' block 'end'"""
        if len(items) == 1:
            params = []
            body = items[0]
        else:
            params = items[0]
            body = items[1]
        return AnonymousFunction(params, body)

    def start(self, items):
        """start: statement*"""
        return Block(items)
