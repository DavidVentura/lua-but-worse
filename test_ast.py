from compiler import create_parser
from ast_builder import ASTBuilder
from ast_nodes import *


def test_simple_assignment():
    code = "a = 5"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], Assign)


def test_local_declaration():
    code = "local x = 10"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], LocalDecl)


def test_function_definition():
    code = """
function add(a, b)
    return a + b
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], FunctionDef)
    func = ast.stmts[0]
    assert func.name == ['add']
    assert func.params == ['a', 'b']


def test_if_statement():
    code = """
if x > 5 then
    print(x)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], If)


def test_for_loop():
    code = """
for i=1,10 do
    print(i)
end
"""
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], ForNum)


def test_table_constructor():
    code = "t = {1, 2, x=3}"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], Assign)
    assign = ast.stmts[0]
    assert isinstance(assign.values[0], TableConstructor)


def test_binary_operations():
    code = "result = a + b * c"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], Assign)
    assign = ast.stmts[0]
    assert isinstance(assign.values[0], BinOp)
