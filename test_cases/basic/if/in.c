#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_a_0;
TValue_t __str_ct_c_1;
TValue_t __str_ct_b_2;

TValue_t cond(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t cond(TVSlice_t args) {
    TValue_t a = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t b = (args.num > 1) ? args.elems[1] : T_NULL;
    TValue_t c = (args.num > 2) ? args.elems[2] : T_NULL;

    if (__bool(a)) {
            printh(__str_ct_a_0);
        } else {
            if (__bool(b)) {
                        printh(__str_ct_b_2);
                    } else {
                        printh(__str_ct_c_1);
                    }
        }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(cond, ((TVSlice_t){(TValue_t[]){T_TRUE, T_FALSE, T_FALSE}, 3})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(cond, ((TVSlice_t){(TValue_t[]){T_FALSE, T_TRUE, T_FALSE}, 3})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(cond, ((TVSlice_t){(TValue_t[]){T_FALSE, T_FALSE, T_TRUE}, 3})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_a_0, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));
    _set(&__str_ct_c_1, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 1)));
    _set(&__str_ct_b_2, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 2)));

    return T_NULL;
}