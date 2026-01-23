#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_y_0;
TValue_t __str_ct_x_1;
TValue_t __str_ct_move_y_2;

TValue_t _anon_0(TVSlice_t args);
TValue_t test_celeste_init_object_captures_obj(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _anon_0(TVSlice_t args) {
    TValue_t amount = (args.num > 0) ? args.elems[0] : T_NULL;

    // Extract closure context
    TValue_t _closure_func = (args.num > 1) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* obj = &_captured.captured[_func->captured_indices[0]].value;

    set_tabvalue(*obj, __str_ct_y_0, TNUM(0));
    TValue_t gc i = T_NULL;
    _set(&i, TNUM(0));
    TValue_t gc tmp_1 = T_NULL;
    _set(&tmp_1, TNUM(2));
    while (__bool(_leq(i, tmp_1))) {
            set_tabvalue(*obj, __str_ct_x_1, TNUM(0));
            _move(&i, _add(i, TNUM8(1)));
        }
    return T_NULL;
}

TValue_t test_celeste_init_object_captures_obj(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    uint16_t _cap_idx_obj = _alloc_captured(_tmp0);
    TValue_t* obj = &_captured.captured[_cap_idx_obj].value;
    TValue_t _tmp1 = TCLOSURE(_anon_0, 1);
    set_closure_arg(_tmp1, 0, _cap_idx_obj);
    set_tabvalue(*obj, __str_ct_move_y_2, _tmp1);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(test_celeste_init_object_captures_obj, ((TVSlice_t){NULL, 0})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_y_0, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
    _set(&__str_ct_x_1, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
    _set(&__str_ct_move_y_2, TSTRi(_store_str_at_or_die(CONSTSTR("move_y"), 2)));

    return T_NULL;
}