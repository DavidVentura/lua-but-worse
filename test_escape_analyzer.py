from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from capture_detector import CaptureDetector
from escape_analyzer import EscapeAnalyzer
from ast_nodes import *


def test_local_variable_not_escaping():
    code = """
local x = 5
local y = x + 10
"""
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

    x_var = global_scope.vars['x']
    y_var = global_scope.vars['y']

    assert x_var not in analyzer.escaping_vars
    assert y_var not in analyzer.escaping_vars


def test_returned_variable_escapes():
    code = """
function foo()
    local x = 5
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']

    assert x_var in analyzer.escaping_vars


def test_function_argument_escapes():
    code = """
function foo()
    local x = 5
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']

    assert x_var in analyzer.escaping_vars


def test_captured_variable_escapes():
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    x_var = global_scope.vars['x']

    assert x_var in analyzer.escaping_vars


def test_nested_capture_escapes():
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    x_var = global_scope.vars['x']
    outer_scope = scopes[1]
    y_var = outer_scope.vars['y']

    assert x_var in analyzer.escaping_vars
    assert y_var in analyzer.escaping_vars


def test_anonymous_function_capture_escapes():
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    x_var = global_scope.vars['x']

    assert x_var in analyzer.escaping_vars


def test_multiple_escape_reasons():
    code = """
function foo()
    local x = 5
    print(x)
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']

    assert x_var in analyzer.escaping_vars


def test_loop_variable_not_escaping():
    code = """
for i=1,10 do
    local x = i * 2
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    loop_scope = scopes[1]
    i_var = loop_scope.vars['i']
    x_var = loop_scope.vars['x']

    assert i_var not in analyzer.escaping_vars
    assert x_var not in analyzer.escaping_vars


def test_parameter_not_escaping():
    code = """
function foo(a, b)
    local x = a + b
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    a_var = func_scope.vars['a']
    b_var = func_scope.vars['b']

    assert a_var not in analyzer.escaping_vars
    assert b_var not in analyzer.escaping_vars


def test_parameter_escaping_when_returned():
    code = """
function foo(a, b)
    return a + b
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    a_var = func_scope.vars['a']
    b_var = func_scope.vars['b']

    assert a_var in analyzer.escaping_vars
    assert b_var in analyzer.escaping_vars


def test_global_not_in_escaping_set():
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    x_var = global_scope.vars['x']

    assert x_var not in analyzer.escaping_vars


def test_method_call_argument_escapes():
    code = """
function foo()
    local x = 5
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

    analyzer = EscapeAnalyzer(scopes, global_scope)
    analyzer.analyze(ast)

    func_scope = scopes[1]
    x_var = func_scope.vars['x']

    assert x_var in analyzer.escaping_vars
