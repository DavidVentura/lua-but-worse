from pathlib import Path
import glob
import os
import tempfile
import subprocess

import pytest

from a import transform

here = Path(__file__).parent
SHOULD_REGENERATE_OUTPUT = os.environ.get("SHOULD_REGENERATE_OUTPUT", None)
DO_NOT_GENERATE_CODE = os.environ.get("DO_NOT_GENERATE_CODE", None)

def _compile_and_run(transformed_src: str, dest_dir: Path, testing_params: dict):
    _target_temp = Path(dest_dir / 'out.c')

    with _target_temp.open('w') as fd:
        fd.write(transformed_src)


    flags = ['gcc', '-O0', '-std=c11', '-fsanitize=address',
            f'-I{here.absolute()}',
             '-I/home/david/git/PicoPico/src',
             '-lm',  # link, -l, not I
             '-g',
             str(_target_temp),
             f"{here.absolute()}/lua.c",
             f"{here.absolute()}/fix32.c",
             f"{here.absolute()}/lua_table.c",
             f"{here.absolute()}/lua_math.c",
             ]
    #flags = ['tcc', '-O0', '-std=c11', '-fsanitize=address', f'-I{here.absolute()}', str(_target_temp)]
    if testing_params.get('disable_grow_table'):
        flags += ["-DNO_GROW_TABLE"]
    s = subprocess.check_output(flags, cwd=dest_dir)
    s = subprocess.check_output(['./a.out'], cwd=dest_dir)
    return s.decode().strip().splitlines()

def find_case_pairs():
    marks = ['basic', 'compound', 'internals', 'pico8', 'syntax', 'regression']
    ret = []
    for m in marks:
        per_mark = []
        for d in glob.glob(f'test_cases/{m}/*/*.lua'):
            per_mark.append(pytest.param(f'test_cases/{m}', os.path.basename(os.path.dirname(d)), os.path.splitext(os.path.basename(d))[0], marks=getattr(pytest.mark, m)))
        ret += per_mark
    return ret

def _extract_pragmas(src: str) -> dict:
    ret = {}
    for line in src.splitlines():
        line = line.strip()
        if not line.startswith("-- pragma: "):
            continue
        line = line.replace("-- pragma: ", "")
        ret[line] = 1
    return ret

@pytest.mark.parametrize("test_dir, test_case, test_name", find_case_pairs())
def test_cases(test_dir: str, test_case: str, test_name: str):
    in_f = Path(f'{test_dir}/{test_case}/{test_name}.lua')
    expected_f = Path(f'{test_dir}/{test_case}/{test_name}.c')
    stdout_f = Path(f'{test_dir}/{test_case}/{test_name}.out')

    with in_f.open() as fd:
        i = fd.read()

    testing_params = _extract_pragmas(i)

    if not DO_NOT_GENERATE_CODE:
        code = transform(i).strip()
        actual = code.splitlines()
        if SHOULD_REGENERATE_OUTPUT:
            with expected_f.open('w') as fd:
                print(code, file=fd)
                expected = code

        with expected_f.open() as fd:
            expected = fd.read()

        assert actual == expected.strip().splitlines()

    with stdout_f.open() as fd:
        expected_stdout = fd.read()

    _patched_src = transform(i, testing_params=testing_params).strip()
    with open('patched_out.c', 'w') as fd:
        print(_patched_src, file=fd)

    with tempfile.TemporaryDirectory() as td:
        exec_output = _compile_and_run(_patched_src, Path(td), testing_params)

    assert expected_stdout.strip().splitlines() == exec_output
