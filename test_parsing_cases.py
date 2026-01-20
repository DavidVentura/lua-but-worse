#!/usr/bin/env python3

import pytest
from pathlib import Path
from compiler import parse


TEST_CASES_DIR = Path(__file__).parent / "test_cases"


def get_test_files():
    if not TEST_CASES_DIR.exists():
        return []

    test_files = []
    for lua_file in TEST_CASES_DIR.rglob("in.lua"):
        test_files.append((lua_file.parent.name, lua_file))
    return sorted(test_files)


@pytest.mark.parametrize("name,lua_file", get_test_files())
def test_parse_real_case(name, lua_file):
    code = lua_file.read_text()
    tree = parse(code)
    assert tree.data == "start"
    assert tree is not None
