from pathlib import Path
import os
import tempfile
import subprocess

import pytest

from a import transform

here = Path(__file__).parent

def _compile_and_run(source: Path, dest_dir: Path):
    s = subprocess.check_output(['g++', '-std=c++17', f'-I{here.absolute()}', str(source.absolute())], cwd=dest_dir)
    s = subprocess.check_output(['./a.out'], cwd=dest_dir)
    return s.decode().strip().splitlines()

def find_case_pairs():
    return [(Path(f'test_cases/{d}/in.lua'), Path(f'test_cases/{d}/out.cpp'), Path(f'test_cases/{d}/expected_stdout'))
            for d in os.listdir('test_cases')]

@pytest.mark.parametrize("in_f,expected_f,stdout_f", find_case_pairs())
def test_cases(in_f: Path, expected_f: Path, stdout_f: Path):
    print(in_f, expected_f)
    with in_f.open() as fd:
        i = fd.read()
    with expected_f.open() as fd:
        expected = fd.read()

    actual = transform(i).strip().splitlines()
    assert actual == expected.strip().splitlines()

    if stdout_f is None:
        # not all test cases have expected outputs
        return
    with stdout_f.open() as fd:
        expected_stdout = fd.read()


    with tempfile.TemporaryDirectory() as td:
        exec_output = _compile_and_run(expected_f, Path(td))

    assert expected_stdout.strip().splitlines() == exec_output
