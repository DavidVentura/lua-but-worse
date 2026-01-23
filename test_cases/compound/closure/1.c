#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_double_e_0;

TValue_t whatever_func_fn(TVSlice_t args);
TValue_t test_double_enclosing_fornum_iterator(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t whatever_func_fn(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* y = &_captured.captured[_func->captured_indices[0]].value;
    TValue_t* x = &_captured.captured[_func->captured_indices[1]].value;

    printh(_mult(*x, *y));
    return T_NULL;
}

TValue_t test_double_enclosing_fornum_iterator(TVSlice_t args) {
    printh(__str_ct_double_e_0);
    uint16_t _cap_idx_x = _alloc_captured(TNUM(1));
    TValue_t* x = &_captured.captured[_cap_idx_x].value;
    TValue_t gc tmp_0 = T_NULL;
    _set(&tmp_0, TNUM(2));
    while (__bool(_leq(*x, tmp_0))) {
            uint16_t _cap_idx_y = _alloc_captured(TNUM(1));
            TValue_t* y = &_captured.captured[_cap_idx_y].value;
            TValue_t gc tmp_1 = T_NULL;
            _set(&tmp_1, TNUM(2));
            while (__bool(_leq(*y, tmp_1))) {
                        TValue_t whatever_func = TCLOSURE(whatever_func_fn, 2);
                        set_closure_arg(whatever_func, 0, _cap_idx_y);
                        set_closure_arg(whatever_func, 1, _cap_idx_x);
                        {
                            TValue_t gc _tmp;
                            _set(&_tmp, CALL(whatever_func, ((TVSlice_t){NULL, 0})));
                        }
                        _move(y, _add(*y, TNUM8(1)));
                    }
            _move(x, _add(*x, TNUM8(1)));
        }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(test_double_enclosing_fornum_iterator, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_double_e_0, TSTRi(_store_str_at_or_die(CONSTSTR("double_enclosing fornum iterator"), 0)));

    return T_NULL;
}