import os
import pytest

from a import transform

def find_case_pairs():
    return [(f'test_cases/{d}/in.lua', f'test_cases/{d}/out.cpp') for d in os.listdir('test_cases')]

@pytest.mark.parametrize("in_f,expected_f", find_case_pairs())
def test_cases(in_f, expected_f):
    with open(in_f) as fd:
        i = fd.read()
    with open(expected_f) as fd:
        expected = fd.read()
    assert transform(i).strip() == expected.strip()
