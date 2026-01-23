#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_captured_0;

TValue_t captures_5;
TValue_t captures_6;

TValue_t f2_fn(TVSlice_t args);
TValue_t f(TVSlice_t args);
TValue_t test_function_args_captured(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t f2_fn(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* arg = &_captured.captured[_func->captured_indices[0]].value;

    printh(*arg);
    return T_NULL;
}

TValue_t f(TVSlice_t args) {
    uint16_t _cap_idx_arg = _alloc_captured((args.num > 0) ? args.elems[0] : T_NULL);
    TValue_t* arg = &_captured.captured[_cap_idx_arg].value;
    TValue_t f2 = TCLOSURE(f2_fn, 1);
    set_closure_arg(f2, 0, _cap_idx_arg);
    return f2;
}

TValue_t test_function_args_captured(TVSlice_t args) {
    printh(__str_ct_captured_0);
    _set(&captures_5, CALL(f, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    _set(&captures_6, CALL(f, ((TVSlice_t){(TValue_t[]){TNUM(6)}, 1})));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(captures_5, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(captures_6, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(captures_5, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(test_function_args_captured, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_captured_0, TSTRi(_store_str_at_or_die(CONSTSTR("captured args"), 0)));

    return T_NULL;
}