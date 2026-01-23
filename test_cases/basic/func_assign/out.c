#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;

TValue_t g_obj;

TValue_t make_obj(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t make_obj(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, __str_ct_x_0, TNUM(10));
    set_tabvalue(_tmp0, __str_ct_y_1, TNUM(20));
    _set(&g_obj, _tmp0);
    return g_obj;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp1;
    _set(&_tmp1, CALL(make_obj, ((TVSlice_t){NULL, 0})));
    set_tabvalue(_tmp1, __str_ct_x_0, TNUM(-1));
    printh(get_tabvalue(g_obj, __str_ct_x_0));
    TValue_t gc _tmp2;
    _set(&_tmp2, CALL(make_obj, ((TVSlice_t){NULL, 0})));
    set_tabvalue(_tmp2, __str_ct_y_1, TNUM(99));
    printh(get_tabvalue(g_obj, __str_ct_y_1));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(2);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));

    _set(&g_obj, T_NULL);
    return T_NULL;
}

