from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from escape_analyzer import EscapeAnalyzer
from ir_lowering import IRLowering
from code_generator import CCodeGenerator


def test_fibonacci():
    """Test compiling a simple fibonacci function"""
    lua_code = """
function fib(n)
    if n <= 1 then
        return n
    end
    return fib(n - 1) + fib(n - 2)
end
"""

    # Parse
    parser = create_parser()
    tree = parser.parse(lua_code)

    # Build AST
    builder = ASTBuilder()
    ast = builder.transform(tree)

    # Scope resolution
    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    # Capture detection
    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    # Escape analysis
    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    # Lower to IR
    lowering = IRLowering(scopes, global_scope, analyzer.escaping_vars)
    globals, functions, escaping_names, string_constants = lowering.lower(ast)

    # Generate C code
    codegen = CCodeGenerator()
    c_code = codegen.generate(globals, functions, escaping_names, string_constants)

    # Verify generated code
    assert '#include "lua.h"' in c_code
    assert 'TValue_t fib(TVSlice_t args)' in c_code
    assert 'TValue_t n = (args.num > 0) ? args.elems[0] : T_NULL;' in c_code
    assert 'if (__bool(_leq(' in c_code
    assert '_sub(' in c_code
    assert '_add(' in c_code

    # Print for inspection
    print("\nGenerated C code for fibonacci:")
    print("=" * 60)
    print(c_code)
    print("=" * 60)


def test_closure():
    """Test compiling a closure"""
    lua_code = """
function make_counter()
    local count = 0
    function increment()
        count = count + 1
        return count
    end
    return increment
end
"""

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
    globals, functions, escaping_names, string_constants = lowering.lower(ast)

    codegen = CCodeGenerator()
    c_code = codegen.generate(globals, functions, escaping_names, string_constants)

    # Verify closure handling
    assert 'increment' in c_code
    # TODO: Once closure implementation is done, add more assertions

    print("\nGenerated C code for closure:")
    print("=" * 60)
    print(c_code)
    print("=" * 60)


def test_table_operations():
    """Test compiling table operations"""
    lua_code = """
local t = {}
t.x = 5
t.y = 10
local sum = t.x + t.y
"""

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
    globals, functions, escaping_names, string_constants = lowering.lower(ast)

    codegen = CCodeGenerator()
    c_code = codegen.generate(globals, functions, escaping_names, string_constants)

    # Verify table operations
    assert 'make_table' in c_code
    assert 'set_tabvalue' in c_code
    assert 'get_tabvalue' in c_code

    print("\nGenerated C code for table operations:")
    print("=" * 60)
    print(c_code)
    print("=" * 60)
