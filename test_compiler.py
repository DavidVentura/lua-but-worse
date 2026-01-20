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
    assert len(statement.children) == 1

    assignment = statement.children[0]
    assert assignment.data == "assignment"
    assert len(assignment.children) == 2

    name = assignment.children[0]
    assert isinstance(name, Token)
    assert name.type == "NAME"
    assert name.value == "a"

    value = assignment.children[1]
    assert value.data == "value"
    assert len(value.children) == 1

    number = value.children[0]
    assert isinstance(number, Token)
    assert number.type == "NUMBER"
    assert number.value == "5"


def test_variable_assignment_to_variable():
    code = "x = y"
    tree = parse(code)

    assignment = tree.children[0].children[0]
    assert assignment.data == "assignment"

    name = assignment.children[0]
    assert name.value == "x"

    value = assignment.children[1]
    value_token = value.children[0]
    assert value_token.type == "NAME"
    assert value_token.value == "y"


def test_float_assignment():
    code = "pi = 3.14"
    tree = parse(code)

    assignment = tree.children[0].children[0]
    number = assignment.children[1].children[0]
    assert number.type == "NUMBER"
    assert number.value == "3.14"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
