#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_newCount_0;

TValue_t c1;
TValue_t c2;

TValue_t test_returning_lambda(TVSlice_t args);
TValue_t _anon_0(TVSlice_t args);
TValue_t newCounter(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t test_returning_lambda(TVSlice_t args) {
    printh(__str_ct_newCount_0);
    _set(&c1, CALL(newCounter, ((TVSlice_t){NULL, 0})));
    printh(CALL(c1, ((TVSlice_t){NULL, 0})));
    printh(CALL(c1, ((TVSlice_t){NULL, 0})));
    _set(&c2, CALL(newCounter, ((TVSlice_t){NULL, 0})));
    printh(CALL(c2, ((TVSlice_t){NULL, 0})));
    printh(CALL(c1, ((TVSlice_t){NULL, 0})));
    printh(CALL(c2, ((TVSlice_t){NULL, 0})));
    return T_NULL;
}

TValue_t _anon_0(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* i = &_captured.captured[_func->captured_indices[0]].value;

    _set(i, _add(*i, TNUM(1)));
    return *i;
}

TValue_t newCounter(TVSlice_t args) {
    uint16_t _cap_idx_i = _alloc_captured(TNUM(0));
    TValue_t* i = &_captured.captured[_cap_idx_i].value;
    TValue_t _tmp0 = TCLOSURE(_anon_0, 1);
    set_closure_arg(_tmp0, 0, _cap_idx_i);
    return _tmp0;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(test_returning_lambda, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_newCount_0, TSTRi(_store_str_at_or_die(CONSTSTR("newCounter"), 0)));

    return T_NULL;
}