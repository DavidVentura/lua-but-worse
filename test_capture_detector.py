from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from ast_nodes import *


def test_simple_capture():
    code = """
local x = 1
function outer()
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    outer_scope = scopes[1]
    assert x_var in outer_scope.captures


def test_no_capture_same_scope():
    code = """
function foo()
    local x = 1
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL
    assert len(func_scope.captures) == 0


def test_nested_capture():
    code = """
local x = 1
function outer()
    local y = 2
    function inner()
        return x + y
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    outer_scope = scopes[1]
    y_var = outer_scope.vars['y']
    assert y_var.kind == VarKind.LOCAL

    inner_scope = scopes[2]
    assert x_var in inner_scope.captures
    assert y_var in inner_scope.captures


def test_parameter_capture():
    code = """
function outer(a, b)
    function inner()
        return a + b
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    outer_scope = scopes[1]
    a_var = outer_scope.vars['a']
    b_var = outer_scope.vars['b']

    assert a_var.kind == VarKind.PARAM
    assert b_var.kind == VarKind.PARAM

    inner_scope = scopes[2]
    assert a_var in inner_scope.captures
    assert b_var in inner_scope.captures


def test_anonymous_function_capture():
    code = """
local x = 1
local f = function()
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    anon_scope = scopes[1]
    assert x_var in anon_scope.captures


def test_capture_in_loop():
    code = """
local x = 1
for i=1,10 do
    function foo()
        return x + i
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    loop_scope = scopes[1]
    i_var = loop_scope.vars['i']
    assert i_var.kind == VarKind.LOCAL

    func_scope = scopes[2]
    assert x_var in func_scope.captures
    assert i_var in func_scope.captures


def test_multiple_functions_same_capture():
    code = """
local x = 1
function f1()
    return x
end
function f2()
    return x * 2
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    f1_scope = scopes[1]
    f2_scope = scopes[2]
    assert x_var in f1_scope.captures
    assert x_var in f2_scope.captures


def test_deep_nesting():
    code = """
local a = 1
function f1()
    local b = 2
    function f2()
        local c = 3
        function f3()
            return a + b + c
        end
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    a_var = global_scope.vars['a']
    f1_scope = scopes[1]
    b_var = f1_scope.vars['b']
    f2_scope = scopes[2]
    c_var = f2_scope.vars['c']
    f3_scope = scopes[3]

    assert a_var.kind == VarKind.LOCAL
    assert b_var.kind == VarKind.LOCAL
    assert c_var.kind == VarKind.LOCAL

    assert a_var in f3_scope.captures
    assert b_var in f3_scope.captures
    assert c_var in f3_scope.captures


def test_no_capture_loop_variable_in_loop():
    code = """
for i=1,10 do
    print(i)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    loop_scope = scopes[1]
    i_var = loop_scope.vars['i']
    assert i_var.kind == VarKind.LOCAL


def test_global_not_captured():
    code = """
x = 1
function foo()
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.GLOBAL

    func_scope = scopes[1]
    assert len(func_scope.captures) == 0


def test_function_call_as_statement():
    code = """
local x = 1
function foo()
    print(x)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    func_scope = scopes[1]
    assert x_var in func_scope.captures


def test_method_call_as_statement():
    code = """
local x = 1
function foo()
    obj:method(x)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    x_var = global_scope.vars['x']
    assert x_var.kind == VarKind.LOCAL

    func_scope = scopes[1]
    assert x_var in func_scope.captures


def test_global_function_not_captured():
    """Test that global functions are not marked as captured when called from other functions"""
    code = """
function something(arg)
    return arg
end

function main()
    something(5)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    detector = CaptureDetector(scopes, global_scope)
    detector.analyze(ast)

    # something is defined in global scope
    something_var = global_scope.vars['something']
    
    # main is also in global scope
    main_scope = scopes[2]  # scope 0=global, 1=something, 2=main
    
    # something should NOT be captured by main
    assert something_var not in main_scope.captures, f"Global function 'something' should not be captured, but main.captures = {main_scope.captures}"
