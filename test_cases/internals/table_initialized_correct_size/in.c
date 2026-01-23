#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_a_0;
TValue_t __str_ct_x_1;
TValue_t __str_ct_y_2;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(1)));
    TValue_t gc a = T_NULL;
    _set(&a, _tmp0);
    set_tabvalue(a, __str_ct_a_0, TNUM(1));
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(2)));
    set_tabvalue(_tmp1, __str_ct_x_1, TNUM(1));
    set_tabvalue(_tmp1, __str_ct_y_2, TNUM(2));
    TValue_t gc b = T_NULL;
    _set(&b, _tmp1);
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(3)));
    set_tabvalue(_tmp2, __str_ct_x_1, TNUM(1));
    set_tabvalue(_tmp2, __str_ct_y_2, TNUM(2));
    TValue_t gc c = T_NULL;
    _set(&c, _tmp2);
    set_tabvalue(c, __str_ct_a_0, TNUM(1));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_a_0, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));
    _set(&__str_ct_x_1, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
    _set(&__str_ct_y_2, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 2)));

    return T_NULL;
}