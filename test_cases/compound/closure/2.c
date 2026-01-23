#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_enclosin_0;

TValue_t func_fn(TVSlice_t args);
TValue_t test_enclosing_fornum_iterator(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t func_fn(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* i = &_captured.captured[_func->captured_indices[0]].value;

    printh(*i);
    return T_NULL;
}

TValue_t test_enclosing_fornum_iterator(TVSlice_t args) {
    printh(__str_ct_enclosin_0);
    uint16_t _cap_idx_i = _alloc_captured(TNUM(1));
    TValue_t* i = &_captured.captured[_cap_idx_i].value;
    TValue_t gc tmp_0;
    _set(&tmp_0, TNUM(2));
    while (__bool(_leq(*i, tmp_0))) {
            TValue_t func = TCLOSURE(func_fn, 1);
            set_closure_arg(func, 0, _cap_idx_i);
            {
                TValue_t gc _tmp;
                _set(&_tmp, CALL(func, ((TVSlice_t){NULL, 0})));
            }
            _set(i, _add(*i, TNUM8(1)));
        }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(test_enclosing_fornum_iterator, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_enclosin_0, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing fornum iterator"), 0)));

    return T_NULL;
}