from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from ast_nodes import *


def test_simple_local_variable():
    code = "local x = 5"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    assert len(scopes) == 1
    assert 'x' in global_scope.vars
    assert global_scope.vars['x'].kind == VarKind.LOCAL


def test_global_variable():
    code = "x = 5"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    assert 'x' in global_scope.vars
    assert global_scope.vars['x'].kind == VarKind.CAPTURED


def test_function_scope():
    code = """
function foo(a, b)
    local x = a + b
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    assert len(scopes) == 2
    assert 'foo' in global_scope.vars

    func_scope = scopes[1]
    assert func_scope.is_function
    assert 'a' in func_scope.vars
    assert 'b' in func_scope.vars
    assert 'x' in func_scope.vars
    assert func_scope.vars['a'].kind == VarKind.PARAM
    assert func_scope.vars['b'].kind == VarKind.PARAM
    assert func_scope.vars['x'].kind == VarKind.LOCAL


def test_nested_scopes():
    code = """
local x = 1
function outer()
    local y = 2
    function inner()
        local z = 3
        return x + y + z
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    assert len(scopes) == 3
    assert 'x' in global_scope.vars

    outer_scope = scopes[1]
    assert 'y' in outer_scope.vars
    assert 'inner' in outer_scope.vars

    inner_scope = scopes[2]
    assert 'z' in inner_scope.vars


def test_for_loop_scope():
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

    assert len(scopes) == 2
    loop_scope = scopes[1]
    assert loop_scope.is_loop
    assert 'i' in loop_scope.vars
    assert 'x' in loop_scope.vars


def test_name_resolution():
    code = """
local x = 5
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

    func_def = ast.stmts[1]
    return_stmt = func_def.body.stmts[0]
    name_ref = return_stmt.values[0]

    assert isinstance(name_ref, NameRef)
    assert name_ref.resolved is not None
    assert name_ref.resolved.name == 'x'
    assert name_ref.resolved.scope_id == global_scope.scope_id


def test_shadowing():
    code = """
local x = 1
function foo()
    local x = 2
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    func_def = ast.stmts[1]
    return_stmt = func_def.body.stmts[1]
    name_ref = return_stmt.values[0]

    assert name_ref.resolved.scope_id == scopes[1].scope_id


def test_global_assignment_in_function():
    code = """
function f()
    x = 5
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    assert len(scopes) == 2
    assert 'f' in global_scope.vars
    assert 'x' in global_scope.vars
    assert global_scope.vars['x'].kind == VarKind.CAPTURED

    func_scope = scopes[1]
    assert 'x' not in func_scope.vars
