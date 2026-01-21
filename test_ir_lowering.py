from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from escape_analyzer import EscapeAnalyzer
from ir_lowering import IRLowering
from ir_nodes import *


def lower_code(code: str) -> list:
    """Helper to run full pipeline and return IR functions"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    lowering = IRLowering(scopes, global_scope, analyzer.escaping_vars)
    globals, functions = lowering.lower(ast)
    return functions


def test_simple_local():
    code = """
local x = 5
"""
    functions = lower_code(code)

    assert len(functions) == 1
    assert functions[0].name == "_lua_main"
    assert len(functions[0].body) == 1
    assert isinstance(functions[0].body[0], CDeclare)


def test_simple_function():
    code = """
function add(a, b)
    return a + b
end
"""
    functions = lower_code(code)

    assert len(functions) == 2

    func_names = {f.name for f in functions}
    assert "_lua_main" in func_names
    assert "add" in func_names

    add_func = next(f for f in functions if f.name == "add")
    assert add_func.params == ["a", "b"]
    assert len(add_func.captures) == 0


def test_closure():
    code = """
local x = 1
function f()
    return x
end
"""
    functions = lower_code(code)

    assert len(functions) == 2

    func_names = {f.name for f in functions}
    assert "_lua_main" in func_names
    assert "f" in func_names

    f_func = next(f for f in functions if f.name == "f")
    assert f_func.is_closure()
    assert "x" in f_func.captures


def test_binary_operation():
    code = """
local x = 5 + 10
"""
    functions = lower_code(code)

    assert len(functions) == 1
    main = functions[0]
    assert len(main.body) == 1

    decl = main.body[0]
    assert isinstance(decl, CDeclare)
    assert decl.var.name == "x"
    assert isinstance(decl.init, CFunctionCall)
    assert decl.init.func_name == "_add"


def test_if_statement():
    code = """
if x then
    y = 1
else
    y = 2
end
"""
    functions = lower_code(code)

    assert len(functions) == 1
    main = functions[0]
    assert len(main.body) == 1
    assert isinstance(main.body[0], CIf)


def test_table_access():
    code = """
local x = t[5]
"""
    functions = lower_code(code)

    assert len(functions) == 1
    main = functions[0]
    decl = main.body[0]
    assert isinstance(decl, CDeclare)
    assert isinstance(decl.init, CFunctionCall)
    assert decl.init.func_name == "get_tabvalue"
