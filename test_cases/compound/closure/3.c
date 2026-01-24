#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_enclosin_0;
TValue_t __str_ct_not_wrap_1;
TValue_t __str_ct_x_2;
TValue_t __str_ct_wrapped_3;
TValue_t __str_ct_a_4;

TValue_t wrapped_fn(TVSlice_t args);
TValue_t wrapper(TVSlice_t args);
TValue_t test_enclosing_table_index(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t wrapped_fn(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* obj = &_captured.captured[_func->captured_indices[0]].value;
    TValue_t* y = &_captured.captured[_func->captured_indices[1]].value;

    printh(get_tabvalue(*obj, __str_ct_x_2));
    printh(get_tabvalue(*obj, *y));
    return T_NULL;
}

TValue_t wrapper(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;

    uint16_t _cap_idx_y = _alloc_captured((args.num > 1) ? args.elems[1] : T_NULL);
    TValue_t* y = &_captured.captured[_cap_idx_y].value;
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(2)));
    uint16_t _cap_idx_obj = _alloc_captured(_tmp0);
    TValue_t* obj = &_captured.captured[_cap_idx_obj].value;
    set_tabvalue(*obj, __str_ct_x_2, __str_ct_not_wrap_1);
    set_tabvalue(*obj, __str_ct_a_4, __str_ct_wrapped_3);
    TValue_t wrapped = TCLOSURE(wrapped_fn, 2);
    set_closure_arg(wrapped, 0, _cap_idx_obj);
    set_closure_arg(wrapped, 1, _cap_idx_y);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(wrapped, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t test_enclosing_table_index(TVSlice_t args) {
    printh(__str_ct_enclosin_0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(wrapper, ((TVSlice_t){(TValue_t[]){TNUM(5), __str_ct_a_4}, 2})));
    }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_enclosing_table_index, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(5);
    _set(&__str_ct_enclosin_0, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing arg overlap"), 0)));
    _set(&__str_ct_not_wrap_1, TSTRi(_store_str_at_or_die(CONSTSTR("not wrapped"), 1)));
    _set(&__str_ct_x_2, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 2)));
    _set(&__str_ct_wrapped_3, TSTRi(_store_str_at_or_die(CONSTSTR("wrapped"), 3)));
    _set(&__str_ct_a_4, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 4)));

    return T_NULL;
}