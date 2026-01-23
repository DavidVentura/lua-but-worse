#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct___index_1;
TValue_t __str_ct_y_2;

TValue_t a;
TValue_t b;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(1)));
    set_tabvalue(_tmp0, __str_ct_x_0, TNUM(5));
    _set(&a, _tmp0);
    set_tabvalue(a, __str_ct___index_1, a);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(0)));
    _set(&b, _tmp1);
    setmetatable(b, a);
    printh(get_tabvalue(b, __str_ct_x_0));
    set_tabvalue(b, __str_ct_x_0, _add(get_tabvalue(b, __str_ct_x_0), TNUM(5)));
    printh(get_tabvalue(b, __str_ct_x_0));
    printh(get_tabvalue(b, __str_ct_y_2));
    set_tabvalue(a, __str_ct_y_2, TNUM(7));
    printh(get_tabvalue(b, __str_ct_y_2));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct___index_1, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 1)));
    _set(&__str_ct_y_2, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 2)));

    return T_NULL;
}