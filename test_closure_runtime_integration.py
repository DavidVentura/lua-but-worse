from pathlib import Path
import tempfile
import subprocess

here = Path(__file__).parent


def _compile_and_run(c_code: str, dest_dir: Path):
    """Compile and run C code with the Lua runtime"""
    target_temp = Path(dest_dir / 'test.c')

    with target_temp.open('w') as fd:
        fd.write(c_code)

    flags = [
        'gcc', '-O0', '-std=c11', '-fsanitize=address',
        f'-I{here.absolute()}',
        '-lm',
        '-g',
        str(target_temp),
        f"{here.absolute()}/lua.c",
        f"{here.absolute()}/fix32.c",
        f"{here.absolute()}/lua_table.c",
        f"{here.absolute()}/lua_math.c",
    ]

    subprocess.check_output(flags, cwd=dest_dir)
    output = subprocess.check_output(['./a.out'], cwd=dest_dir)
    return output.decode().strip().splitlines()


def test_closure_runtime_basic():
    """Test that the closure runtime (captured variables, refcounting) works correctly"""
    c_code = '''
#include "lua.h"
#include <stdio.h>

// Test function with one capture
TValue_t closure_func(TVSlice_t args) {
    // Extract the function TValue_t (last argument)
    TValue_t func_tv = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* func = GETTFUN(func_tv);

    // Access captured variable
    TValue_t* captured = &_captured.captured[func->captured_indices[0]].value;

    return *captured;
}

// Test function without captures
TValue_t simple_func(TVSlice_t args) {
    return TNUM(42);
}

TValue_t main(TVSlice_t args) {
    // Test 1: Simple function without captures
    TValue_t gc f1 = TFUN(simple_func);
    TValue_t result1 = CALL(f1, ((TVSlice_t){NULL, 0}));
    printh(result1);

    // Test 2: Function with one capture
    uint16_t cap_idx = _alloc_captured(TNUM(123));
    uint16_t* cap_array = malloc(sizeof(uint16_t) * 1);
    cap_array[0] = cap_idx;

    TValue_t gc f2 = TCLOSURE(closure_func, cap_array, 1);
    TValue_t result2 = CALL(f2, ((TVSlice_t){NULL, 0}));
    printh(result2);

    // Test 3: Modify captured variable
    _captured.captured[cap_idx].value = TNUM(456);
    TValue_t result3 = CALL(f2, ((TVSlice_t){NULL, 0}));
    printh(result3);

    // Test 4: Share captured variable between two closures
    uint16_t* cap_array2 = malloc(sizeof(uint16_t) * 1);
    cap_array2[0] = cap_idx;

    TValue_t gc f3 = TCLOSURE(closure_func, cap_array2, 1);
    TValue_t result4 = CALL(f3, ((TVSlice_t){NULL, 0}));
    printh(result4);

    return T_NULL;
}
'''

    with tempfile.TemporaryDirectory() as td:
        exec_output = _compile_and_run(c_code, Path(td))

    expected_output = ['42', '123', '456', '456']
    assert exec_output == expected_output, f"Expected {expected_output}, got {exec_output}"


def test_closure_runtime_multiple_captures():
    """Test closure with multiple captured variables"""
    c_code = '''
#include "lua.h"
#include <stdio.h>

TValue_t closure_with_two_captures(TVSlice_t args) {
    // Extract the function TValue_t (last argument)
    TValue_t func_tv = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* func = GETTFUN(func_tv);

    // Access both captured variables
    TValue_t* cap1 = &_captured.captured[func->captured_indices[0]].value;
    TValue_t* cap2 = &_captured.captured[func->captured_indices[1]].value;

    // Return sum of captured variables
    return _add(*cap1, *cap2);
}

TValue_t main(TVSlice_t args) {
    // Allocate two captured variables
    uint16_t cap_idx1 = _alloc_captured(TNUM(10));
    uint16_t cap_idx2 = _alloc_captured(TNUM(20));

    uint16_t* cap_array = malloc(sizeof(uint16_t) * 2);
    cap_array[0] = cap_idx1;
    cap_array[1] = cap_idx2;

    TValue_t gc f = TCLOSURE(closure_with_two_captures, cap_array, 2);
    TValue_t result = CALL(f, ((TVSlice_t){NULL, 0}));
    printh(result);

    // Modify first captured variable
    _captured.captured[cap_idx1].value = TNUM(100);
    TValue_t result2 = CALL(f, ((TVSlice_t){NULL, 0}));
    printh(result2);

    return T_NULL;
}
'''

    with tempfile.TemporaryDirectory() as td:
        exec_output = _compile_and_run(c_code, Path(td))

    expected_output = ['30', '120']
    assert exec_output == expected_output, f"Expected {expected_output}, got {exec_output}"


def test_closure_runtime_refcounting():
    """Test that closure refcounting properly manages captured variable lifetime"""
    c_code = '''
#include "lua.h"
#include <stdio.h>

TValue_t get_captured(TVSlice_t args) {
    TValue_t func_tv = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* func = GETTFUN(func_tv);
    TValue_t* captured = &_captured.captured[func->captured_indices[0]].value;
    return *captured;
}

TValue_t main(TVSlice_t args) {
    uint16_t cap_idx = _alloc_captured(TNUM(99));

    // Create closure in a scope
    {
        uint16_t* cap_array = malloc(sizeof(uint16_t) * 1);
        cap_array[0] = cap_idx;
        TValue_t gc f = TCLOSURE(get_captured, cap_array, 1);

        TValue_t result = CALL(f, ((TVSlice_t){NULL, 0}));
        printh(result);

        // f goes out of scope here, refcount should be managed
    }

    // The captured variable should still exist because we hold a reference via cap_idx
    printh(_captured.captured[cap_idx].value);
    _decref_captured(cap_idx);

    return T_NULL;
}
'''

    with tempfile.TemporaryDirectory() as td:
        exec_output = _compile_and_run(c_code, Path(td))

    expected_output = ['99', '99']
    assert exec_output == expected_output, f"Expected {expected_output}, got {exec_output}"
