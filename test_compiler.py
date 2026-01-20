#!/usr/bin/env python3

import pytest
from lark import Tree, Token
from compiler import parse


def test_simple_assignment():
    code = "a = 5"
    tree = parse(code)

    assert tree.data == "start"
    assert len(tree.children) == 1

    statement = tree.children[0]
    assert statement.data == "statement"


def test_variable_assignment_to_variable():
    code = "x = y"
    tree = parse(code)

    assignment = tree.children[0].children[0]
    assert assignment.data == "assignment"


def test_float_assignment():
    code = "pi = 3.14"
    tree = parse(code)

    assignment = tree.children[0].children[0]
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


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
