from compiler import create_parser
from ast_builder import ASTBuilder
from ast_nodes import *


def test_nested_functions():
    code = """
function outer()
    function inner()
        return 1
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert len(ast.stmts) == 1
    outer = ast.stmts[0]
    assert isinstance(outer, FunctionDef)
    assert outer.name == ['outer']
    assert len(outer.body.stmts) == 1
    inner = outer.body.stmts[0]
    assert isinstance(inner, FunctionDef)
    assert inner.name == ['inner']


def test_local_before_function():
    code = """
local x = 1
function foo()
    return x
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert len(ast.stmts) == 2
    assert isinstance(ast.stmts[0], LocalDecl)
    assert isinstance(ast.stmts[1], FunctionDef)


def test_function_inside_function_with_locals():
    code = """
function outer()
    local y = 2
    function inner()
        local z = 3
        return y + z
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    outer = ast.stmts[0]
    assert isinstance(outer, FunctionDef)
    assert len(outer.body.stmts) == 2
    assert isinstance(outer.body.stmts[0], LocalDecl)
    assert isinstance(outer.body.stmts[1], FunctionDef)


def test_deeply_nested_functions():
    code = """
function level1()
    function level2()
        function level3()
            return 42
        end
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    level1 = ast.stmts[0]
    assert isinstance(level1, FunctionDef)

    level2 = level1.body.stmts[0]
    assert isinstance(level2, FunctionDef)

    level3 = level2.body.stmts[0]
    assert isinstance(level3, FunctionDef)


def test_for_loop_with_local():
    code = """
for i=1,10 do
    local x = i
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert isinstance(ast.stmts[0], ForNum)
    loop = ast.stmts[0]
    assert len(loop.body.stmts) == 1
    assert isinstance(loop.body.stmts[0], LocalDecl)


def test_while_loop_with_local():
    code = """
while true do
    local x = 1
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert isinstance(ast.stmts[0], While)
    loop = ast.stmts[0]
    assert len(loop.body.stmts) == 1
    assert isinstance(loop.body.stmts[0], LocalDecl)


def test_if_with_function():
    code = """
if x then
    function foo()
        return 1
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert isinstance(ast.stmts[0], If)
    if_stmt = ast.stmts[0]
    assert len(if_stmt.then_block.stmts) == 1
    assert isinstance(if_stmt.then_block.stmts[0], FunctionDef)


def test_function_with_for_loop():
    code = """
function foo()
    for i=1,10 do
        print(i)
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    func = ast.stmts[0]
    assert isinstance(func, FunctionDef)
    assert len(func.body.stmts) == 1
    assert isinstance(func.body.stmts[0], ForNum)


def test_function_with_while_loop():
    code = """
function foo()
    while x do
        local y = 1
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    func = ast.stmts[0]
    assert isinstance(func, FunctionDef)
    assert len(func.body.stmts) == 1
    assert isinstance(func.body.stmts[0], While)


def test_function_with_return():
    code = """
function foo()
    return 42
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    func = ast.stmts[0]
    assert isinstance(func, FunctionDef)
    assert len(func.body.stmts) == 1
    assert isinstance(func.body.stmts[0], Return)


def test_anonymous_function_in_assignment():
    code = """
local f = function()
    return 1
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert isinstance(ast.stmts[0], LocalDecl)
    local_decl = ast.stmts[0]
    assert len(local_decl.values) == 1
    assert isinstance(local_decl.values[0], AnonymousFunction)


def test_nested_anonymous_functions():
    code = """
local f = function()
    local g = function()
        return 42
    end
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    local_decl = ast.stmts[0]
    outer_func = local_decl.values[0]
    assert isinstance(outer_func, AnonymousFunction)

    inner_local = outer_func.body.stmts[0]
    assert isinstance(inner_local, LocalDecl)

    inner_func = inner_local.values[0]
    assert isinstance(inner_func, AnonymousFunction)


def test_for_in_loop_with_local():
    code = """
for k, v in pairs(t) do
    local x = k + v
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert isinstance(ast.stmts[0], ForIn)
    loop = ast.stmts[0]
    assert len(loop.body.stmts) == 1
    assert isinstance(loop.body.stmts[0], LocalDecl)


def test_elseif_with_local():
    code = """
if x then
    local a = 1
elseif y then
    local b = 2
else
    local c = 3
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    if_stmt = ast.stmts[0]
    assert isinstance(if_stmt, If)
    assert len(if_stmt.then_block.stmts) == 1
    assert isinstance(if_stmt.then_block.stmts[0], LocalDecl)
    assert len(if_stmt.elseif_parts) == 1
    assert isinstance(if_stmt.elseif_parts[0][1].stmts[0], LocalDecl)
    assert len(if_stmt.else_block.stmts) == 1
    assert isinstance(if_stmt.else_block.stmts[0], LocalDecl)


def test_function_with_global_assignment():
    code = """
function f()
    x = 5
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    assert len(ast.stmts) == 1
    func = ast.stmts[0]
    assert isinstance(func, FunctionDef)
    assert func.name == ['f']
    assert len(func.body.stmts) == 1
    assert isinstance(func.body.stmts[0], Assign)
