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


def test_compound_assignment():
    code = "x += 2"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], CompoundAssign)
    compound = ast.stmts[0]
    assert compound.op == '+'
    assert isinstance(compound.target, NameRef)
    assert compound.target.name == 'x'
    assert isinstance(compound.value, Number)
    assert compound.value.value == '2'


def test_if_not_equal_comparison():
    code = "if a!=b then end"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], If)
    if_stmt = ast.stmts[0]
    assert isinstance(if_stmt.condition, BinOp)
    assert if_stmt.condition.op == '!='
    assert isinstance(if_stmt.condition.left, NameRef)
    assert if_stmt.condition.left.name == 'a'
    assert isinstance(if_stmt.condition.right, NameRef)
    assert if_stmt.condition.right.name == 'b'


def test_local_with_trailing_semicolon():
    code = "local a = 5;"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], LocalDecl)
    local_decl = ast.stmts[0]
    assert local_decl.names == ['a']
    assert len(local_decl.values) == 1
    assert isinstance(local_decl.values[0], Number)
    assert local_decl.values[0].value == '5'


def test_function_call_attribute_assignment():
    code = "func(args).attr=-1"
    parser = create_parser()
    tree = parser.parse(code)
    builder = ASTBuilder()
    ast = builder.transform(tree)
    assert isinstance(ast, Block)
    assert len(ast.stmts) == 1
    assert isinstance(ast.stmts[0], Assign)
    assign = ast.stmts[0]
    assert len(assign.targets) == 1
    target = assign.targets[0]
    assert isinstance(target, TableAccess)
    assert isinstance(target.key, String)
    assert target.key.value == 'attr'
    assert target.is_dot == True
    assert isinstance(target.table, FunctionCall)
    func_call = target.table
    assert isinstance(func_call.func, NameRef)
    assert func_call.func.name == 'func'
    assert len(func_call.args) == 1
    assert isinstance(func_call.args[0], NameRef)
    assert func_call.args[0].name == 'args'
    assert len(assign.values) == 1
    assert isinstance(assign.values[0], UnOp)
    assert assign.values[0].op == '-'
    assert isinstance(assign.values[0].operand, Number)
    assert assign.values[0].operand.value == '1'
