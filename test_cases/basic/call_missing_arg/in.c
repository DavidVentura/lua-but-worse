#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_all_0;
TValue_t __str_ct_str_1;
TValue_t __str_ct_all_miss_2;

TValue_t f(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t f(TVSlice_t args) {
    TValue_t a = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t b = (args.num > 1) ? args.elems[1] : T_NULL;
    TValue_t c = (args.num > 2) ? args.elems[2] : T_NULL;

    printh(a);
    printh(b);
    printh(c);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    printh(__str_ct_all_0);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(f, ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(2), TNUM(3)}, 3})));
    }
    printh(__str_ct_str_1);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(f, ((TVSlice_t){(TValue_t[]){TNUM(1)}, 1})));
    }
    printh(__str_ct_all_miss_2);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(f, ((TVSlice_t){NULL, 0})));
    }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_all_0, TSTRi(_store_str_at_or_die(CONSTSTR("all"), 0)));
    _set(&__str_ct_str_1, TSTRi(_store_str_at_or_die(CONSTSTR("2 missing"), 1)));
    _set(&__str_ct_all_miss_2, TSTRi(_store_str_at_or_die(CONSTSTR("all missing"), 2)));

    return T_NULL;
}