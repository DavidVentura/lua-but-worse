from ir_nodes import *


class CCodeGenerator:
    """Generate C code from IR

    Converts CFunctionDef IR nodes to C code strings that use the runtime API
    defined in lua.h, lua_table.h, lua_math.h.

    Key design decisions:
    - All Lua functions have signature: TValue_t func(TVSlice_t args)
    - Logical parameters are extracted from args array in function body
    - Function calls pack arguments into TVSlice_t
    - Closures need context structs (TODO)
    """

    def __init__(self):
        self.indent_level = 0
        self.escaping_vars = set()

    def generate(self, globals: list[str], functions: list[CFunctionDef], escaping_vars: set[str] = None) -> str:
        """Generate complete C program from IR"""
        self.escaping_vars = escaping_vars or set()
        code = []

        code.append('#include "lua.h"')
        code.append('#include "lua_math.h"')
        code.append('#include "lua_table.h"')
        code.append('#include "pico8.h"')
        code.append('#include "stdlib.h"')

        code.append('')

        for var_name in globals:
            code.append(f"TValue_t {var_name};")
        if globals:
            code.append('')

        for func in functions:
            code.append(f"TValue_t {func.name}(TVSlice_t args);")
        code.append('')

        for func in functions:
            code.append(self._generate_function(func))
            code.append('')

        return '\n'.join(code)

    def _generate_function(self, func: CFunctionDef) -> str:
        """Generate a single C function"""
        lines = []

        # TODO: Handle closures with captures
        if func.is_closure():
            lines.append(f"// WARNING: Closure {func.name} has captures: {func.captures}")
            lines.append("// TODO: Implement closure context struct")
            lines.append("")

        # Function signature: TValue_t name(TVSlice_t args)
        lines.append(f"TValue_t {func.name}(TVSlice_t args) {{")

        self.indent_level += 1

        # Extract parameters from args array
        for i, param_name in enumerate(func.params):
            lines.append(self._indent(
                f"TValue_t {param_name} = (args.num > {i}) ? args.elems[{i}] : T_NULL;"
            ))

        if func.params:
            lines.append("")  # Blank line after param extraction

        # Generate function body
        for stmt in func.body:
            stmt_code = self._generate_stmt(stmt)
            lines.append(self._indent(stmt_code))

        self.indent_level -= 1
        lines.append("}")

        return '\n'.join(lines)

    def _generate_stmt(self, stmt: CStmt) -> str:
        """Generate C code for a statement"""
        match stmt:
            case CDeclare(var, init):
                if init:
                    value_expr = self._generate_expr(init)
                    return f"{self._type_to_c(var.type)} gc {var.name};\n_set(&{var.name}, {value_expr});"
                else:
                    return f"{self._type_to_c(var.type)} gc {var.name};"

            case CAssign(target, value):
                value_expr = self._generate_expr(value)
                return f"_set(&{target.name}, {value_expr});"

            case CIf(condition, then_stmts, else_stmts):
                lines = []
                lines.append(f"if ({self._generate_expr(condition)}) {{")
                self.indent_level += 1
                for s in then_stmts:
                    lines.append(self._indent(self._generate_stmt(s)))
                self.indent_level -= 1

                if else_stmts:
                    lines.append(self._indent("} else {"))
                    self.indent_level += 1
                    for s in else_stmts:
                        lines.append(self._indent(self._generate_stmt(s)))
                    self.indent_level -= 1

                lines.append(self._indent("}"))
                return '\n'.join(lines)

            case CWhile(condition, body):
                lines = []
                lines.append(f"while ({self._generate_expr(condition)}) {{")
                self.indent_level += 1
                for s in body:
                    lines.append(self._indent(self._generate_stmt(s)))
                self.indent_level -= 1
                lines.append(self._indent("}"))
                return '\n'.join(lines)

            case CReturn(value):
                if value:
                    return f"return {self._generate_expr(value)};"
                else:
                    return "return T_NULL;"

            case CExprStmt(expr, needs_cleanup):
                expr_str = self._generate_expr(expr)
                if needs_cleanup:
                    return f"{{\n    TValue_t gc _tmp;\n    _set(&_tmp, {expr_str});\n}}"
                else:
                    return f"{expr_str};"

            case _:
                return f"/* Unknown statement: {type(stmt).__name__} */"

    def _generate_expr(self, expr: CExpr) -> str:
        """Generate C code for an expression"""
        match expr:
            case CVarRef(var):
                return var.name

            case CLiteral(value, _):
                return value

            case CFunctionCall(func_name, args):
                # Special handling for function calls:
                # Most runtime functions take TValue_t arguments directly
                # But __call needs TVSlice_t packing

                if func_name == "__call":
                    # Dynamic call: __call(func, arg1, arg2, ...)
                    # Need to pack args into TVSlice_t
                    if len(args) < 1:
                        return "/* ERROR: __call needs at least func */"

                    func_expr = self._generate_expr(args[0])
                    arg_exprs = [self._generate_expr(a) for a in args[1:]]

                    if arg_exprs:
                        # Create temp array for args
                        arr_elements = ", ".join(arg_exprs)
                        return f"__call({func_expr}, (TVSlice_t){{(TValue_t[]){{{arr_elements}}}, {len(arg_exprs)}}})"
                    else:
                        # No args
                        return f"__call({func_expr}, (TVSlice_t){{NULL, 0}})"
                else:
                    # Normal runtime function call
                    args_str = ", ".join(self._generate_expr(a) for a in args)
                    return f"{func_name}({args_str})"

            case CBinOp(op, left, right):
                return f"({self._generate_expr(left)} {op} {self._generate_expr(right)})"

            case CUnOp(op, operand):
                return f"({op}{self._generate_expr(operand)})"

            case _:
                return f"/* Unknown expr: {type(expr).__name__} */"

    def _type_to_c(self, ctype: CType) -> str:
        """Convert CType to C string"""
        base = ctype.name
        return f"{base}*" if ctype.is_pointer else base

    def _indent(self, text: str) -> str:
        """Add current indentation to text"""
        if not text:
            return text

        indent = "    " * self.indent_level
        # Handle multi-line strings
        lines = text.split('\n')
        return '\n'.join(indent + line if line else line for line in lines)
