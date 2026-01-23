#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_value_0;
TValue_t __str_ct_key_1;
TValue_t __str_ct_a_2;

TValue_t u;
TValue_t t;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    TValue_t gc o = T_NULL;
    _set(&o, _tmp0);
    set_tabvalue(o, __str_ct_key_1, __str_ct_value_0);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_a_2, TNUM(1));
    _set(&u, _tmp1);
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(0)));
    set_tabvalue(_tmp2, TNUM(7), TNUM(8));
    set_tabvalue(_tmp2, TNUM(8), TNUM(7));
    _move(&t, get_tabvalue(_tmp2, TNUM(7)));
    printh(get_tabvalue(o, __str_ct_key_1));
    printh(get_tabvalue(u, __str_ct_a_2));
    printh(t);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_value_0, TSTRi(_store_str_at_or_die(CONSTSTR("value"), 0)));
    _set(&__str_ct_key_1, TSTRi(_store_str_at_or_die(CONSTSTR("key"), 1)));
    _set(&__str_ct_a_2, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 2)));

    return T_NULL;
}