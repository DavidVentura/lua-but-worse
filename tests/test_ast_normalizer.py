from compiler import create_parser
from ast_builder import ASTBuilder
from scope_resolver import ScopeResolver
from ast_normalizer import ASTNormalizer
from ast_nodes import *


def normalize_code(lua_code: str) -> Block:
    """Helper to run normalization pipeline and return normalized AST"""
    parser = create_parser()
    tree = parser.parse(lua_code)
    builder = ASTBuilder()
    ast = builder.transform(tree)

    resolver = ScopeResolver(ast)
    scopes, global_scope = resolver.analyze()

    normalizer = ASTNormalizer(scopes, global_scope)
    return normalizer.normalize(ast)


def test_hoist_table_in_function_arg():
    """foo({x=1}) → local _tmp0={}; _tmp0.x=1; foo(_tmp0)"""
    code = "foo({x=1})"
    result = normalize_code(code)

    # Should have 3 statements: local _tmp0 = {}, _tmp0.x = 1, foo(_tmp0)
    assert len(result.stmts) == 3

    # First statement: local _tmp0 = {}
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')
    assert len(stmt0.values) == 1
    assert isinstance(stmt0.values[0], TableConstructor)
    assert len(stmt0.values[0].fields) == 0  # Empty after normalization

    # Second statement: _tmp0.x = 1
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, Assign)
    assert len(stmt1.targets) == 1
    assert isinstance(stmt1.targets[0], TableAccess)
    assert isinstance(stmt1.targets[0].table, NameRef)
    assert stmt1.targets[0].table.name.startswith('_tmp')
    assert isinstance(stmt1.targets[0].key, String)
    assert stmt1.targets[0].key.value == 'x'
    assert isinstance(stmt1.values[0], Number)
    assert stmt1.values[0].value == '1'

    # Third statement: foo(_tmp0) (raw FunctionCall, not wrapped in ExprStmt)
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, FunctionCall)
    assert isinstance(stmt2.func, NameRef)
    assert stmt2.func.name == 'foo'
    assert len(stmt2.args) == 1
    assert isinstance(stmt2.args[0], NameRef)
    assert stmt2.args[0].name.startswith('_tmp')


def test_hoist_table_in_return():
    """return {x=1} → local _tmp0={}; _tmp0.x=1; return _tmp0"""
    code = "return {x=1}"
    result = normalize_code(code)

    assert len(result.stmts) == 3

    # First two: temp creation and initialization
    assert isinstance(result.stmts[0], LocalDecl)
    assert isinstance(result.stmts[1], Assign)

    # Third: return _tmp0
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, Return)
    assert len(stmt2.values) == 1
    assert isinstance(stmt2.values[0], NameRef)
    assert stmt2.values[0].name.startswith('_tmp')


def test_hoist_table_in_local_decl():
    """local t = {x=1} → local _tmp0={}; _tmp0.x=1; local t=_tmp0"""
    code = "local t = {x=1}"
    result = normalize_code(code)

    assert len(result.stmts) == 3

    # First: local _tmp0 = {}
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')

    # Second: _tmp0.x = 1
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, Assign)
    assert isinstance(stmt1.targets[0], TableAccess)

    # Third: local t = _tmp0
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, LocalDecl)
    assert stmt2.names[0] == 't'
    assert isinstance(stmt2.values[0], NameRef)
    assert stmt2.values[0].name.startswith('_tmp')


def test_hoist_table_in_assign():
    """g = {x=1} → local _tmp0={}; _tmp0.x=1; g=_tmp0"""
    code = "g = {x=1}"
    result = normalize_code(code)

    assert len(result.stmts) == 3

    # First: local _tmp0 = {}
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')

    # Second: _tmp0.x = 1
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, Assign)
    assert isinstance(stmt1.targets[0], TableAccess)

    # Third: g = _tmp0
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, Assign)
    assert isinstance(stmt2.targets[0], NameRef)
    assert stmt2.targets[0].name == 'g'
    assert isinstance(stmt2.values[0], NameRef)
    assert stmt2.values[0].name.startswith('_tmp')


def test_nested_tables():
    """{{1}, {2}} → properly hoists inner then outer"""
    code = "local outer = {{1}, {2}}"
    result = normalize_code(code)

    # Should have multiple statements:
    # local _tmp0 = {} (inner table 1)
    # _tmp0[1] = 1
    # local _tmp1 = {} (inner table 2)
    # _tmp1[1] = 2
    # local _tmp2 = {} (outer table)
    # _tmp2[1] = _tmp0
    # _tmp2[2] = _tmp1
    # local outer = _tmp2

    # At least 8 statements
    assert len(result.stmts) >= 8

    # Last statement should be local outer = _tmpN
    last_stmt = result.stmts[-1]
    assert isinstance(last_stmt, LocalDecl)
    assert last_stmt.names[0] == 'outer'
    assert isinstance(last_stmt.values[0], NameRef)
    assert last_stmt.values[0].name.startswith('_tmp')


def test_table_as_key():
    """t[{k=1}] = v → hoists table used as key"""
    code = "t[{k=1}] = v"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0.k = 1
    # t[_tmp0] = v
    assert len(result.stmts) == 3

    # First two: temp table creation
    assert isinstance(result.stmts[0], LocalDecl)
    assert isinstance(result.stmts[1], Assign)

    # Third: t[_tmp0] = v
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, Assign)
    assert isinstance(stmt2.targets[0], TableAccess)
    assert isinstance(stmt2.targets[0].table, NameRef)
    assert stmt2.targets[0].table.name == 't'
    assert isinstance(stmt2.targets[0].key, NameRef)
    assert stmt2.targets[0].key.name.startswith('_tmp')


def test_table_as_value_complex_target():
    """t[k] = {x=1} → hoists table"""
    code = "t[k] = {x=1}"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0.x = 1
    # t[k] = _tmp0
    assert len(result.stmts) == 3

    # Last statement: t[k] = _tmp0
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, Assign)
    assert isinstance(stmt2.targets[0], TableAccess)
    assert isinstance(stmt2.values[0], NameRef)
    assert stmt2.values[0].name.startswith('_tmp')


def test_scope_tracking():
    """Verify temps created in correct scope (function vs global)"""
    code = """
function f()
    return {x=1}
end
local g = {y=2}
"""
    result = normalize_code(code)

    # The function should have normalized statements in its body
    # The global should have normalized statements for local g
    func_stmt = result.stmts[0]
    assert isinstance(func_stmt, FunctionDef)
    assert func_stmt.name == ['f']

    # Function body should have normalized return
    assert len(func_stmt.body.stmts) == 3  # local _tmp, assignment, return

    # Global scope should have normalized local g
    # After the function def, we should have: local _tmp, _tmp.y=2, local g=_tmp
    global_stmts_after_func = result.stmts[1:]
    assert len(global_stmts_after_func) == 3


def test_mixed_expression():
    """foo({a=1}) + bar({b=2}) → tables and function calls hoisted"""
    code = "local x = foo({a=1}) + bar({b=2})"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0.a = 1
    # local _tmp1 = {}
    # _tmp1.b = 2
    # local _tmp2 = foo(_tmp0)
    # local _tmp3 = bar(_tmp1)
    # local x = _tmp2 + _tmp3
    assert len(result.stmts) == 7

    # Last statement: local x = _tmp2 + _tmp3
    last_stmt = result.stmts[-1]
    assert isinstance(last_stmt, LocalDecl)
    assert last_stmt.names[0] == 'x'
    assert isinstance(last_stmt.values[0], BinOp)
    assert last_stmt.values[0].op == '+'
    assert isinstance(last_stmt.values[0].left, NameRef)
    assert isinstance(last_stmt.values[0].right, NameRef)


def test_empty_table_hoisted():
    """local t = {} → local _tmp0={}; local t=_tmp0 (for consistency)"""
    code = "local t = {}"
    result = normalize_code(code)

    # Should normalize even empty tables for consistency
    assert len(result.stmts) == 2

    # First: local _tmp0 = {}
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')

    # Second: local t = _tmp0
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, LocalDecl)
    assert stmt1.names[0] == 't'
    assert isinstance(stmt1.values[0], NameRef)


def test_table_with_array_and_named_fields():
    """local t = {1, 2, x=3} → _tmp[1]=1, _tmp[2]=2, _tmp.x=3"""
    code = "local t = {1, 2, x=3}"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0[1] = 1
    # _tmp0[2] = 2
    # _tmp0.x = 3
    # local t = _tmp0
    assert len(result.stmts) == 5

    # Check array assignments use numeric keys
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, Assign)
    assert isinstance(stmt1.targets[0], TableAccess)
    assert isinstance(stmt1.targets[0].key, Number)
    assert stmt1.targets[0].key.value == '1'

    stmt2 = result.stmts[2]
    assert isinstance(stmt2, Assign)
    assert isinstance(stmt2.targets[0], TableAccess)
    assert isinstance(stmt2.targets[0].key, Number)
    assert stmt2.targets[0].key.value == '2'

    # Check named field assignment
    stmt3 = result.stmts[3]
    assert isinstance(stmt3, Assign)
    assert isinstance(stmt3.targets[0], TableAccess)
    assert isinstance(stmt3.targets[0].key, String)
    assert stmt3.targets[0].key.value == 'x'


def test_table_in_if_condition():
    """if {x=1} then end → hoists table before if"""
    code = """
if {x=1} then
    printh("yes")
end
"""
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0.x = 1
    # if _tmp0 then printh("yes") end
    assert len(result.stmts) == 3

    # Last statement: if _tmp0
    last_stmt = result.stmts[-1]
    assert isinstance(last_stmt, If)
    assert isinstance(last_stmt.condition, NameRef)
    assert last_stmt.condition.name.startswith('_tmp')


def test_table_in_while_condition():
    """while {x=1} do end → hoists table before while"""
    code = """
while {x=1} do
    break
end
"""
    result = normalize_code(code)

    # Should have hoisted statements before while
    assert len(result.stmts) >= 3

    # Find the while statement
    while_stmt = None
    for stmt in result.stmts:
        if isinstance(stmt, While):
            while_stmt = stmt
            break

    assert while_stmt is not None
    assert isinstance(while_stmt.condition, NameRef)
    assert while_stmt.condition.name.startswith('_tmp')


def test_table_in_for_range():
    """for i=1,{x=10}.x do end → hoists table"""
    code = """
for i=1,{x=10}.x do
    printh(i)
end
"""
    result = normalize_code(code)

    # Should have hoisted statements before for
    assert len(result.stmts) >= 3

    # Find the ForNum statement
    for_stmt = None
    for stmt in result.stmts:
        if isinstance(stmt, ForNum):
            for_stmt = stmt
            break

    assert for_stmt is not None
    # The stop expression should reference the temp table
    assert isinstance(for_stmt.stop, TableAccess)
    assert isinstance(for_stmt.stop.table, NameRef)
    assert for_stmt.stop.table.name.startswith('_tmp')


def test_multiple_returns_with_tables():
    """return {x=1}, {y=2} → hoists both tables"""
    code = "return {x=1}, {y=2}"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = {}
    # _tmp0.x = 1
    # local _tmp1 = {}
    # _tmp1.y = 2
    # return _tmp0, _tmp1
    assert len(result.stmts) == 5

    # Last statement: return _tmp0, _tmp1
    last_stmt = result.stmts[-1]
    assert isinstance(last_stmt, Return)
    assert len(last_stmt.values) == 2
    assert all(isinstance(v, NameRef) and v.name.startswith('_tmp') for v in last_stmt.values)


def test_method_call_with_table():
    """obj:method({x=1}) → hoists table"""
    code = "obj:method({x=1})"
    result = normalize_code(code)

    # Should have hoisted statements
    assert len(result.stmts) == 3

    # Last statement: obj:method(_tmp0) (raw MethodCall)
    last_stmt = result.stmts[-1]
    assert isinstance(last_stmt, MethodCall)
    assert last_stmt.method == 'method'
    assert len(last_stmt.args) == 1
    assert isinstance(last_stmt.args[0], NameRef)
    assert last_stmt.args[0].name.startswith('_tmp')


def test_and_operator_transformation():
    """a and b → if statement with temps for refcount safety"""
    code = "local result = a and b"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = a
    # local _tmp1
    # if _tmp0 then _tmp1 = b else _tmp1 = _tmp0 end
    # local result = _tmp1
    assert len(result.stmts) == 4

    # First: local _tmp0 = a
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')
    assert len(stmt0.values) == 1
    assert isinstance(stmt0.values[0], NameRef)
    assert stmt0.values[0].name == 'a'

    # Second: local _tmp1 (no value, declared but not assigned)
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, LocalDecl)
    assert stmt1.names[0].startswith('_tmp')
    assert len(stmt1.values) == 0

    # Third: if _tmp0 then _tmp1 = b else _tmp1 = _tmp0 end
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, If)
    assert isinstance(stmt2.condition, NameRef)
    assert stmt2.condition.name.startswith('_tmp')

    # Check then block: _tmp1 = b
    assert len(stmt2.then_block.stmts) == 1
    then_assign = stmt2.then_block.stmts[0]
    assert isinstance(then_assign, Assign)
    assert isinstance(then_assign.targets[0], NameRef)
    assert then_assign.targets[0].name.startswith('_tmp')
    assert isinstance(then_assign.values[0], NameRef)
    assert then_assign.values[0].name == 'b'

    # Check else block: _tmp1 = _tmp0
    assert stmt2.else_block is not None
    assert len(stmt2.else_block.stmts) == 1
    else_assign = stmt2.else_block.stmts[0]
    assert isinstance(else_assign, Assign)
    assert isinstance(else_assign.targets[0], NameRef)
    assert else_assign.targets[0].name.startswith('_tmp')
    assert isinstance(else_assign.values[0], NameRef)
    assert else_assign.values[0].name.startswith('_tmp')

    # Fourth: local result = _tmp1
    stmt3 = result.stmts[3]
    assert isinstance(stmt3, LocalDecl)
    assert stmt3.names[0] == 'result'
    assert isinstance(stmt3.values[0], NameRef)
    assert stmt3.values[0].name.startswith('_tmp')


def test_or_operator_transformation():
    """a or b → if statement with temps for refcount safety"""
    code = "local result = a or b"
    result = normalize_code(code)

    # Should have:
    # local _tmp0 = a
    # local _tmp1
    # if _tmp0 then _tmp1 = _tmp0 else _tmp1 = b end
    # local result = _tmp1
    assert len(result.stmts) == 4

    # First: local _tmp0 = a
    stmt0 = result.stmts[0]
    assert isinstance(stmt0, LocalDecl)
    assert stmt0.names[0].startswith('_tmp')
    assert len(stmt0.values) == 1
    assert isinstance(stmt0.values[0], NameRef)
    assert stmt0.values[0].name == 'a'

    # Second: local _tmp1 (no value)
    stmt1 = result.stmts[1]
    assert isinstance(stmt1, LocalDecl)
    assert stmt1.names[0].startswith('_tmp')
    assert len(stmt1.values) == 0

    # Third: if _tmp0 then _tmp1 = _tmp0 else _tmp1 = b end
    stmt2 = result.stmts[2]
    assert isinstance(stmt2, If)
    assert isinstance(stmt2.condition, NameRef)
    assert stmt2.condition.name.startswith('_tmp')

    # Check then block: _tmp1 = _tmp0
    assert len(stmt2.then_block.stmts) == 1
    then_assign = stmt2.then_block.stmts[0]
    assert isinstance(then_assign, Assign)
    assert isinstance(then_assign.targets[0], NameRef)
    assert then_assign.targets[0].name.startswith('_tmp')
    assert isinstance(then_assign.values[0], NameRef)
    assert then_assign.values[0].name.startswith('_tmp')

    # Check else block: _tmp1 = b
    assert stmt2.else_block is not None
    assert len(stmt2.else_block.stmts) == 1
    else_assign = stmt2.else_block.stmts[0]
    assert isinstance(else_assign, Assign)
    assert isinstance(else_assign.targets[0], NameRef)
    assert else_assign.targets[0].name.startswith('_tmp')
    assert isinstance(else_assign.values[0], NameRef)
    assert else_assign.values[0].name == 'b'

    # Fourth: local result = _tmp1
    stmt3 = result.stmts[3]
    assert isinstance(stmt3, LocalDecl)
    assert stmt3.names[0] == 'result'
    assert isinstance(stmt3.values[0], NameRef)
    assert stmt3.values[0].name.startswith('_tmp')


def test_and_with_function_call():
    """true and tostring(5) → ensures tostring result is in gc-tracked temp"""
    code = "local x = true and tostring(5)"
    result = normalize_code(code)

    # Should transform and into if statement
    # The function call should appear in the then block assignment
    assert len(result.stmts) >= 4

    # Find the If statement
    if_stmt = None
    for stmt in result.stmts:
        if isinstance(stmt, If):
            if_stmt = stmt
            break

    assert if_stmt is not None
    # The then block should assign the function call result to temp
    assert len(if_stmt.then_block.stmts) == 1
    then_assign = if_stmt.then_block.stmts[0]
    assert isinstance(then_assign, Assign)
    # The value could be either a direct function call or a NameRef to a hoisted call
    assert isinstance(then_assign.values[0], (FunctionCall, NameRef))


def test_or_with_function_call():
    """nil or tostring(5) → ensures tostring result is in gc-tracked temp"""
    code = "local x = nil or tostring(5)"
    result = normalize_code(code)

    # Should transform or into if statement
    # The function call should appear in the else block assignment
    assert len(result.stmts) >= 4

    # Find the If statement
    if_stmt = None
    for stmt in result.stmts:
        if isinstance(stmt, If):
            if_stmt = stmt
            break

    assert if_stmt is not None
    # The else block should assign the function call result to temp
    assert if_stmt.else_block is not None
    assert len(if_stmt.else_block.stmts) == 1
    else_assign = if_stmt.else_block.stmts[0]
    assert isinstance(else_assign, Assign)
    # The value could be either a direct function call or a NameRef to a hoisted call
    assert isinstance(else_assign.values[0], (FunctionCall, NameRef))
