#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_____0;

TValue_t test_simple_redecl(TVSlice_t args);
TValue_t _anon_0(TVSlice_t args);
TValue_t test_closure_capture(TVSlice_t args);
TValue_t _anon_1(TVSlice_t args);
TValue_t _anon_2(TVSlice_t args);
TValue_t test_multiple_closures(TVSlice_t args);
TValue_t test_sequential_redecl(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t test_simple_redecl(TVSlice_t args) {
    TValue_t gc x = T_NULL;
    _set(&x, TNUM(1));
    printh(x);
    TValue_t gc x_1 = T_NULL;
    _set(&x_1, TNUM(2));
    printh(x_1);
    TValue_t gc x_2 = T_NULL;
    _set(&x_2, TNUM(3));
    printh(x_2);
    return T_NULL;
}

TValue_t _anon_0(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* c = &_captured.captured[_func->captured_indices[0]].value;

    printh(*c);
    return T_NULL;
}

TValue_t test_closure_capture(TVSlice_t args) {
    uint16_t _cap_idx_c = _alloc_captured(TNUM(5));
    TValue_t* c = &_captured.captured[_cap_idx_c].value;
    TValue_t _tmp0 = TCLOSURE(_anon_0, 1);
    set_closure_arg(_tmp0, 0, _cap_idx_c);
    TValue_t gc f = T_NULL;
    _set(&f, _tmp0);
    uint16_t _cap_idx_c_1 = _alloc_captured(TNUM(6));
    TValue_t* c_1 = &_captured.captured[_cap_idx_c_1].value;
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(f, ((TVSlice_t){NULL, 0})));
    }
    printh(*c_1);
    return T_NULL;
}

TValue_t _anon_1(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* x = &_captured.captured[_func->captured_indices[0]].value;

    printh(*x);
    return T_NULL;
}

TValue_t _anon_2(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* x_1 = &_captured.captured[_func->captured_indices[0]].value;

    printh(*x_1);
    return T_NULL;
}

TValue_t test_multiple_closures(TVSlice_t args) {
    uint16_t _cap_idx_x = _alloc_captured(TNUM(10));
    TValue_t* x = &_captured.captured[_cap_idx_x].value;
    TValue_t _tmp1 = TCLOSURE(_anon_1, 1);
    set_closure_arg(_tmp1, 0, _cap_idx_x);
    TValue_t gc f1 = T_NULL;
    _set(&f1, _tmp1);
    uint16_t _cap_idx_x_1 = _alloc_captured(TNUM(20));
    TValue_t* x_1 = &_captured.captured[_cap_idx_x_1].value;
    TValue_t _tmp2 = TCLOSURE(_anon_2, 1);
    set_closure_arg(_tmp2, 0, _cap_idx_x_1);
    TValue_t gc f2 = T_NULL;
    _set(&f2, _tmp2);
    uint16_t _cap_idx_x_2 = _alloc_captured(TNUM(30));
    TValue_t* x_2 = &_captured.captured[_cap_idx_x_2].value;
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(f1, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(f2, ((TVSlice_t){NULL, 0})));
    }
    printh(*x_2);
    return T_NULL;
}

TValue_t test_sequential_redecl(TVSlice_t args) {
    TValue_t gc a = T_NULL;
    _set(&a, TNUM(100));
    printh(a);
    TValue_t gc a_1 = T_NULL;
    _set(&a_1, TNUM(200));
    printh(a_1);
    TValue_t gc a_2 = T_NULL;
    _set(&a_2, TNUM(300));
    printh(a_2);
    TValue_t gc a_3 = T_NULL;
    _set(&a_3, TNUM(400));
    printh(a_3);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_simple_redecl, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_closure_capture, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_multiple_closures, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_sequential_redecl, ((TVSlice_t){NULL, 0})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_____0, TSTRi(_store_str_at_or_die(CONSTSTR("---"), 0)));

    return T_NULL;
}

