from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from escape_analyzer import EscapeAnalyzer
from ir_lowering import IRLowering
from code_generator import CCodeGenerator


def generate_code(lua_code: str) -> str:
    """Helper to run full pipeline and return generated C code"""
    parser = create_parser()
    tree = parser.parse(lua_code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    lowering = IRLowering(scopes, global_scope, analyzer.escaping_vars)
    globals, functions, escaping_names = lowering.lower(ast)

    codegen = CCodeGenerator()
    return codegen.generate(globals, functions, escaping_names)


def test_simple_local():
    code = """
local x = 5
"""
    result = generate_code(code)

    # Should have headers
    assert '#include "lua.h"' in result
    assert '#include "lua_table.h"' in result
    assert '#include "lua_math.h"' in result

    # Should have main function
    assert 'TValue_t _lua_main(TVSlice_t args)' in result

    # Should have variable declaration
    assert 'TValue_t gc x;' in result
    assert '_set(&x, TNUM(5));' in result


def test_function_with_params():
    code = """
function add(a, b)
    return a + b
end
"""
    result = generate_code(code)

    # Should have forward declaration
    assert 'TValue_t add(TVSlice_t args);' in result

    # Should have function definition
    assert 'TValue_t add(TVSlice_t args)' in result

    # Should extract parameters
    assert 'TValue_t a = (args.num > 0) ? args.elems[0] : T_NULL;' in result
    assert 'TValue_t b = (args.num > 1) ? args.elems[1] : T_NULL;' in result

    # Should have addition
    assert '_add(' in result

    # Should have return
    assert 'return' in result


def test_binary_operations():
    code = """
local x = 5 + 10
local y = x * 2
"""
    result = generate_code(code)

    # Should have _add call
    assert '_add(' in result

    # Should have _mult call
    assert '_mult(' in result


def test_if_statement():
    code = """
if x then
    y = 1
else
    y = 2
end
"""
    result = generate_code(code)

    # Should have if statement with __bool
    assert 'if (__bool(' in result
    assert '} else {' in result


def test_while_loop():
    code = """
while x do
    y = y + 1
end
"""
    result = generate_code(code)

    # Should have while loop
    assert 'while (__bool(' in result


def test_function_call():
    code = """
foo(1, 2, 3)
"""
    result = generate_code(code)

    # Should have __call with TVSlice_t packing
    assert '__call(' in result
    assert 'TVSlice_t' in result


def test_comparison():
    code = """
local x = a < b
"""
    result = generate_code(code)

    # Should use _lt
    assert '_lt(' in result


def test_table_access():
    code = """
local x = t[5]
"""
    result = generate_code(code)

    # Should use get_tabvalue
    assert 'get_tabvalue(' in result


def test_string_literal():
    code = """
local s = "hello"
"""
    result = generate_code(code)

    # Should use TSTR macro
    assert 'TSTR("hello")' in result


def test_boolean_literal():
    code = """
local t = true
local f = false
"""
    result = generate_code(code)

    # Should use T_TRUE and T_FALSE
    assert 'T_TRUE' in result
    assert 'T_FALSE' in result


def test_nil_literal():
    code = """
local n = nil
"""
    result = generate_code(code)

    # Should use T_NULL
    assert 'T_NULL' in result


def test_multiple_functions():
    code = """
function foo()
    return 1
end

function bar()
    return 2
end
"""
    result = generate_code(code)

    # Should have both forward declarations
    assert 'TValue_t foo(TVSlice_t args);' in result
    assert 'TValue_t bar(TVSlice_t args);' in result

    # Should have both function definitions
    assert 'TValue_t foo(TVSlice_t args) {' in result
    assert 'TValue_t bar(TVSlice_t args) {' in result
