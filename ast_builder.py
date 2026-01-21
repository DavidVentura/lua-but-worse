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
        for i in range(1, len(items)):
            result = BinOp('..', result, items[i])
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
        """function_call: '(' [expr_list] ')' | ':' NAME '(' [expr_list] ')'

        For method calls, the ':' is consumed by grammar, so items are:
        [NAME_token (method name), expr_list or None]

        For regular calls, items are:
        [expr_list or None]
        """
        if len(items) >= 1 and isinstance(items[0], Token) and items[0].type == 'NAME':
            method_name = items[0].value
            args = items[1] if len(items) > 1 and isinstance(items[1], list) else []
            return ('function_call', (args, (method_name, args)))
        else:
            args = items[0] if items and isinstance(items[0], list) else []
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
                  | assignable COMPOUND_OP expr
        """
        if len(items) >= 2 and isinstance(items[1], Token) and items[1].type == 'COMPOUND_OP':
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
        """local_decl: LOCAL NAME (',' NAME)* ['=' expr_list]

        items = [LOCAL_token, NAME_token, ..., expr_list_or_None]
        """
        names = []
        values = []
        for item in items:
            if isinstance(item, Token):
                if item.type == 'NAME':
                    names.append(item.value)
            elif isinstance(item, list):
                values = item
        return LocalDecl(names, values)

    def function_def(self, items):
        """
        function_def: FUNCTION function_name '(' [param_list] ')' block END
                    | FUNCTION function_name ':' NAME '(' [param_list] ')' block END

        items now include keyword tokens: [FUNCTION_token, function_name_list, param_list_or_None, block, END_token]
        or for methods: [FUNCTION_token, function_name_list, NAME, param_list_or_None, block, END_token]
        """
        name_parts = items[1]
        idx = 2
        is_method = False
        params = []

        if idx < len(items) and isinstance(items[idx], Token) and items[idx].type == 'NAME':
            is_method = True
            name_parts.append(items[idx].value)
            params = ['self']
            idx += 1

        if idx < len(items) and items[idx] is not None and isinstance(items[idx], list):
            params.extend(items[idx])
            idx += 1
        elif idx < len(items) and items[idx] is None:
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
        if_stmt: IF expr THEN block elif_parts else_part END
               | IF '(' expr ')' statement [ELSE statement+]

        items = [IF_token, expr, THEN_token, block, elif_parts, else_part, END_token]
        or [IF_token, expr, statement, ELSE_token?, statement*]
        """
        condition = items[1]

        if len(items) > 5:
            then_block = items[3]
            elif_parts = items[4] if isinstance(items[4], list) else []
            else_block = items[5] if isinstance(items[5], Block) else None
            return If(condition, then_block, elif_parts, else_block)
        else:
            then_stmt = items[2]
            else_stmts = items[3:] if len(items) > 3 else []

            then_block = Block([then_stmt])
            else_block = Block(else_stmts) if else_stmts else None
            return If(condition, then_block, [], else_block)

    def elif_parts(self, items):
        """elif_parts: (ELSEIF expr THEN block)*

        items = [ELSEIF_token, expr, THEN_token, block, ...]
        """
        result = []
        for i in range(0, len(items), 4):
            if i + 3 < len(items):
                condition = items[i + 1]
                block = items[i + 3]
                result.append((condition, block))
        return result

    def else_part(self, items):
        """else_part: [ELSE block]

        items = [ELSE_token, block] or []
        """
        return items[1] if items else None

    def for_stmt(self, items):
        """
        for_stmt: FOR NAME '=' expr ',' expr [',' expr] DO block END
                | FOR NAME (',' NAME)* IN expr DO block END

        For-in items: [FOR_token, NAME, NAME*, IN_token, expr, DO_token, block, END_token]
        For-num items: [FOR_token, NAME, '=', expr, ',', expr, [',', expr], DO_token, block, END_token]
        """
        var = items[1].value

        if isinstance(items[2], Token) and items[2].type == 'IN':
            vars = [var]
            iterator = items[3]
            body = items[5]
            return ForIn(vars, iterator, body)
        elif isinstance(items[2], Token) and items[2].type == 'NAME':
            vars = [var]
            idx = 2
            while idx < len(items) and isinstance(items[idx], Token) and items[idx].type == 'NAME':
                vars.append(items[idx].value)
                idx += 1
            iterator = items[idx + 1]
            body = items[idx + 3]
            return ForIn(vars, iterator, body)
        else:
            start = items[2]
            stop = items[3]
            step = items[4]
            body = items[6]
            return ForNum(var, start, stop, step, body)

    def while_stmt(self, items):
        """
        while_stmt: WHILE expr DO block END
                  | WHILE '(' expr ')' statement

        items = [WHILE_token, expr, DO_token, block, END_token] or [WHILE_token, expr, statement]
        """
        condition = items[1]
        if len(items) > 3:
            body = items[3]
        else:
            body = Block([items[2]])
        return While(condition, body)

    def return_stmt(self, items):
        """return_stmt: RETURN [expr_list]

        items = [RETURN_token] or [RETURN_token, expr_list]
        """
        values = items[1] if len(items) > 1 else []
        return Return(values)

    def anonymous_function(self, items):
        """anonymous_function: FUNCTION '(' [param_list] ')' block END

        items = [FUNCTION_token, param_list_or_None, block, END_token]
        """
        if items[1] is None:
            params = []
            body = items[2]
        else:
            params = items[1]
            body = items[2]
        return AnonymousFunction(params, body)

    def start(self, items):
        """start: statement*"""
        return Block(items)
