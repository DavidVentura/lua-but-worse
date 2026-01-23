#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_nope_0;
TValue_t __str_ct_nope2_1;
TValue_t __str_ct_str_2;

TValue_t elseif_else_arm(TVSlice_t args);
TValue_t else_arm(TVSlice_t args);
TValue_t local_var(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t elseif_else_arm(TVSlice_t args) {
    TValue_t gc a;
    _set(&a, T_NULL);
    if (__bool(T_FALSE)) {
            printh(__str_ct_nope_0);
        } else {
            if (__bool(T_FALSE)) {
                        printh(__str_ct_nope2_1);
                    } else {
                        TValue_t gc a;
                        _set(&a, TNUM(200));
                        printh(a);
                    }
        }
    printh(a);
    return T_NULL;
}

TValue_t else_arm(TVSlice_t args) {
    TValue_t gc a;
    _set(&a, T_NULL);
    if (__bool(T_FALSE)) {
            printh(__str_ct_nope_0);
        } else {
            TValue_t gc a;
            _set(&a, TNUM(100));
            printh(a);
        }
    printh(a);
    return T_NULL;
}

TValue_t local_var(TVSlice_t args) {
    TValue_t gc a;
    _set(&a, T_NULL);
    if (__bool(TNUM(5))) {
            TValue_t gc a;
            _set(&a, TNUM(100));
        }
    printh(a);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(local_var, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_str_2);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(else_arm, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_str_2);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(elseif_else_arm, ((TVSlice_t){NULL, 0})));
    }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_nope_0, TSTRi(_store_str_at_or_die(CONSTSTR("nope"), 0)));
    _set(&__str_ct_nope2_1, TSTRi(_store_str_at_or_die(CONSTSTR("nope2"), 1)));
    _set(&__str_ct_str_2, TSTRi(_store_str_at_or_die(CONSTSTR("---"), 2)));

    return T_NULL;
}