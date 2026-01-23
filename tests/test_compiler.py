#!/usr/bin/env python3

import pytest
from compiler import parse


def test_simple_assignment():
    code = "a = 5"
    tree = parse(code)

    assert tree.data == "start"
    assert len(tree.children) == 1

    statement = tree.children[0]
    assert statement.data == "assignment"


def test_variable_assignment_to_variable():
    code = "x = y"
    tree = parse(code)

    assignment = tree.children[0]
    assert assignment.data == "assignment"


def test_float_assignment():
    code = "pi = 3.14"
    tree = parse(code)

    assignment = tree.children[0]
    assert assignment.data == "assignment"


# Phase 2: Operators and Expressions

def test_arithmetic_operators():
    expressions = [
        "a = 1 + 2",
        "b = 3 - 1",
        "c = 4 * 5",
        "d = 10 / 2",
        "e = 7 % 3",
        "f = 7 \\ 3",  # integer division
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_comparison_operators():
    expressions = [
        "a = 1 == 2",
        "b = 3 ~= 4",
        "c = 5 < 6",
        "d = 7 > 8",
        "e = 9 <= 10",
        "f = 11 >= 12",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_logical_operators():
    expressions = [
        "a = true and false",
        "b = true or false",
        "c = not true",
        "d = not false",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_string_concatenation():
    code = 'result = "hello" .. "world"'
    tree = parse(code)
    assert tree.data == "start"


def test_unary_operators():
    expressions = [
        "a = -5",
        "b = #table",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_operator_precedence():
    code = "a = 1 + 2 * 3"
    tree = parse(code)
    assert tree.data == "start"

    code = "b = (1 + 2) * 3"
    tree = parse(code)
    assert tree.data == "start"


def test_complex_expression():
    code = "result = 1 + 2 * 3 - 4 / 2"
    tree = parse(code)
    assert tree.data == "start"


# Phase 3: Literals and Data Types

def test_string_literals():
    expressions = [
        'a = "double quoted"',
        "b = 'single quoted'",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_boolean_literals():
    expressions = [
        "a = true",
        "b = false",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_nil_literal():
    code = "a = nil"
    tree = parse(code)
    assert tree.data == "start"


def test_decimal_starting_with_dot():
    code = "a = .5"
    tree = parse(code)
    assert tree.data == "start"


def test_hex_numbers():
    expressions = [
        "a = 0x1234",
        "b = 0xABCD",
        "c = 0xff",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_hex_decimal_numbers():
    expressions = [
        "a = 0x0.8000",
        "b = 0x0.1",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_binary_numbers():
    expressions = [
        "a = 0b1010",
        "b = 0b1111",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_binary_decimal_numbers():
    code = "a = 0b1010.1"
    tree = parse(code)
    assert tree.data == "start"


# Phase 4: Tables

def test_empty_table():
    code = "a = {}"
    tree = parse(code)
    assert tree.data == "start"


def test_array_table():
    code = "a = {1, 2, 3, 4, 5}"
    tree = parse(code)
    assert tree.data == "start"


def test_hash_table():
    code = "a = {x=1, y=2}"
    tree = parse(code)
    assert tree.data == "start"


def test_mixed_table():
    code = "a = {1, 2, x=3, y=4}"
    tree = parse(code)
    assert tree.data == "start"


def test_table_field_access():
    expressions = [
        "a = table.field",
        "b = object.x",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_table_indexing():
    expressions = [
        "a = table[1]",
        'b = object["key"]',
        "c = arr[index]",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_table_field_assignment():
    expressions = [
        "table.field = 5",
        "object.x = 10",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_table_index_assignment():
    expressions = [
        "table[1] = 5",
        'object["key"] = 10',
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_chained_table_access():
    code = "a = table.field.subfield"
    tree = parse(code)
    assert tree.data == "start"


def test_length_operator():
    code = "len = #table"
    tree = parse(code)
    assert tree.data == "start"


def test_compound_assignment():
    expressions = [
        "x += 2",
        "y -= 1",
        "z *= 3",
        "w /= 2",
        "v %= 5",
    ]
    for expr in expressions:
        tree = parse(expr)
        assert tree.data == "start"


def test_compound_assignment_table_field():
    code = "obj.field += 5"
    tree = parse(code)
    assert tree.data == "start"


# Phase 5: Functions

def test_function_definition():
    code = """
function test()
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_with_params():
    code = """
function add(a, b)
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_with_body():
    code = """
function greet(name)
    x = 5
    y = 10
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_call():
    code = "result = func()"
    tree = parse(code)
    assert tree.data == "start"


def test_function_call_with_args():
    code = "result = add(1, 2, 3)"
    tree = parse(code)
    assert tree.data == "start"


def test_nested_function_calls():
    code = "result = outer(inner(5))"
    tree = parse(code)
    assert tree.data == "start"


def test_anonymous_function():
    code = """
f = function(x)
    y = x
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_in_table():
    code = """
t = {
    func = function()
    end
}
"""
    tree = parse(code)
    assert tree.data == "start"


def test_return_statement():
    code = """
function test()
    return
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_return_with_value():
    code = """
function test()
    return 5
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_return_multiple_values():
    code = """
function test()
    return 1, 2, 3
end
"""
    tree = parse(code)
    assert tree.data == "start"


# Phase 6: Control Flow

def test_if_statement():
    code = """
if x then
    y = 1
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_if_else_statement():
    code = """
if x then
    y = 1
else
    y = 2
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_if_elseif_else():
    code = """
if x then
    y = 1
elseif z then
    y = 2
else
    y = 3
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_pico8_short_if():
    code = "if (x) y = 1"
    tree = parse(code)
    assert tree.data == "start"


def test_pico8_short_if_else():
    code = "if (x) y = 1 else y = 2"
    tree = parse(code)
    assert tree.data == "start"


def test_pico8_short_if_multiple_else():
    code = "if (x) y = 1 else y = 2 z = 3"
    tree = parse(code)
    assert tree.data == "start"


def test_for_loop_numeric():
    code = """
for i=1,10 do
    x = i
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_for_loop_with_step():
    code = """
for i=1,10,2 do
    x = i
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_for_loop_iterator():
    code = """
for item in all(t) do
    x = item
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_while_loop():
    code = """
while x do
    y = 1
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_pico8_short_while():
    code = "while (x) y = 1"
    tree = parse(code)
    assert tree.data == "start"


# Phase 7: Local Variables

def test_local_declaration():
    code = "local x"
    tree = parse(code)
    assert tree.data == "start"


def test_local_with_assignment():
    code = "local x = 5"
    tree = parse(code)
    assert tree.data == "start"


def test_local_multiple():
    code = "local a, b, c"
    tree = parse(code)
    assert tree.data == "start"


def test_local_multiple_with_values():
    code = "local a, b = 1, 2"
    tree = parse(code)
    assert tree.data == "start"


# Phase 8: Multi-assignment

def test_multi_assignment():
    code = "a, b = 1, 2"
    tree = parse(code)
    assert tree.data == "start"


def test_multi_assignment_functions():
    code = "a, b = func1(), func2()"
    tree = parse(code)
    assert tree.data == "start"


def test_multi_assignment_table_fields():
    code = "t.x, t.y = 1, 2"
    tree = parse(code)
    assert tree.data == "start"


# Phase 9: Methods

def test_method_definition():
    code = """
function obj:method()
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_method_with_params():
    code = """
function obj:method(a, b)
    x = a
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_method_call():
    code = "obj:method()"
    tree = parse(code)
    assert tree.data == "start"


def test_method_call_with_args():
    code = "obj:method(1, 2)"
    tree = parse(code)
    assert tree.data == "start"


def test_chained_method_calls():
    code = "result = obj:method1():method2()"
    tree = parse(code)
    assert tree.data == "start"


# Phase 10: Comments

def test_lua_comment():
    code = """
-- this is a comment
x = 5
"""
    tree = parse(code)
    assert tree.data == "start"


def test_pico8_comment():
    code = """
// this is a pico8 comment
x = 5
"""
    tree = parse(code)
    assert tree.data == "start"


def test_inline_comment():
    code = "x = 5 -- inline comment"
    tree = parse(code)
    assert tree.data == "start"


# Integration Tests

def test_complete_function():
    code = """
function main()
    local x = 5
    if x > 3 then
        return x * 2
    else
        return 0
    end
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_closure():
    code = """
function main()
    local captured = 7
    a = function(x)
        return x * captured
    end
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_nested_structures():
    code = """
function test()
    for i=1,10 do
        if i > 5 then
            x = i
        end
    end
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_method_with_table():
    code = """
function a:method(value)
    self.x = value
    return nil
end
"""
    tree = parse(code)
    assert tree.data == "start"


# Additional Features

def test_function_table_notation():
    """Test function definition with table.field notation"""
    code = """
function vector.new(x, y)
    return {x=x, y=y}
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_metamethod():
    """Test function definition with __metamethod names"""
    code = """
function vector.__add(v0, v1)
    return vector.new(v0.x + v1.x, v0.y + v1.y)
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_function_nested_table_notation():
    """Test function with deeply nested table notation"""
    code = """
function a.b.c.method()
    x = 5
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_for_in_multiple_vars():
    """Test for-in loop with multiple variables"""
    code = """
for k, v in pairs(tab) do
    printh(k)
    printh(v)
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_for_in_three_vars():
    """Test for-in loop with three variables"""
    code = """
for i, k, v in iterator(tab) do
    printh(i)
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_expression_as_callable():
    """Test calling result of an expression"""
    code = """
(obj or nil)(arg)
"""
    tree = parse(code)
    assert tree.data == "start"


def test_complex_table_with_functions():
    """Test table with functions and bracket notation"""
    code = """
func_in_tab = {
    func = function(this)
        this.attr = {key="value"}
    end,
}
"""
    tree = parse(code)
    assert tree.data == "start"


def test_table_literal_with_string_keys():
    """Test table with string keys in brackets"""
    code = """
literals = {
    ["top"] = {x=0, y=-68},
    ["bottom"] = {x=0, y=68}
}
"""
    tree = parse(code)
    assert tree.data == "start"


def test_compound_assignment_to_method_result():
    """Test compound assignment with method call on right side"""
    code = """
function test()
    self.x /= self:len()
end
"""
    tree = parse(code)
    assert tree.data == "start"


def test_return_with_function_calls_after_if():
    """Test that return statement after if correctly parses function calls

    This is a regression test for a grammar ambiguity where:
        return fib(n - 1) + fib(n - 2)
    was incorrectly parsed as:
        return fib
        (n - 1) + fib(n - 2)  // treated as separate statement
    """
    code = """
function fib(n)
    if n <= 1 then
        return n
    end
    return fib(n - 1) + fib(n - 2)
end
"""
    tree = parse(code)
    assert tree.data == "start"

    # Verify the structure: should have one function with one if and one return
    func_def = tree.children[0]

    # Find the block (it's the child before the "end" token)
    block = None
    for child in func_def.children:
        if hasattr(child, 'data') and child.data == 'block':
            block = child
            break

    assert block is not None, "Could not find block in function_def"

    # Block should have exactly 2 statements: if and return
    # Not 3 (which would be: if, return, expr)
    assert len(block.children) == 2, f"Expected 2 statements in block, got {len(block.children)}"

    # Second statement should be a return_stmt
    return_stmt = block.children[1]
    assert return_stmt.data == "return_stmt"

    # The return should have an add_expr in its expr_list
    # return_stmt children: [Token(RETURN), expr_list]
    expr_list = return_stmt.children[1]
    assert expr_list.data == "expr_list"

    # The expr should be an add_expr (fib(...) + fib(...))
    add_expr = expr_list.children[0]
    assert add_expr.data == "add_expr"
