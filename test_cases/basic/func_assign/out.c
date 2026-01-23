#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;
TValue_t __str_ct_value_2;

TValue_t make_obj(TVSlice_t args);
TValue_t get_table(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t make_obj(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, __str_ct_x_0, TNUM(10));
    set_tabvalue(_tmp0, __str_ct_y_1, TNUM(20));
    return _tmp0;
}

TValue_t get_table(TVSlice_t args) {
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    TValue_t gc t;
    _set(&t, _tmp1);
    set_tabvalue(t, __str_ct_value_2, TNUM(5));
    return t;
}

TValue_t main(TVSlice_t args) {
    set_tabvalue(CALL(make_obj, ((TVSlice_t){NULL, 0})), __str_ct_x_0, TNUM(-1));
    printh(get_tabvalue(CALL(make_obj, ((TVSlice_t){NULL, 0})), __str_ct_x_0));
    set_tabvalue(CALL(get_table, ((TVSlice_t){NULL, 0})), __str_ct_value_2, TNUM(100));
    printh(get_tabvalue(CALL(get_table, ((TVSlice_t){NULL, 0})), __str_ct_value_2));
    TValue_t gc result;
    _set(&result, CALL(make_obj, ((TVSlice_t){NULL, 0})));
    set_tabvalue(result, __str_ct_y_1, TNUM(99));
    printh(get_tabvalue(result, __str_ct_y_1));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));
    _set(&__str_ct_value_2, TSTRi(_store_str_at_or_die(CONSTSTR("value"), 2)));

    return T_NULL;
}

