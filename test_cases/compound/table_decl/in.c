#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_value_to_0;
TValue_t __str_ct_key_1;
TValue_t __str_ct_attr_2;
TValue_t __str_ct_value_at_3;
TValue_t __str_ct_func_in__4;
TValue_t __str_ct_func_5;
TValue_t __str_ct_x_6;
TValue_t __str_ct_y_7;
TValue_t __str_ct_top_8;
TValue_t __str_ct_bottom_9;
TValue_t __str_ct_left_10;
TValue_t __str_ct_right_11;

TValue_t this;
TValue_t func_in_tab;
TValue_t literals;

TValue_t _anon_0(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _anon_0(TVSlice_t args) {
    TValue_t this = (args.num > 0) ? args.elems[0] : T_NULL;

    TValue_t gc _tmp3 = T_NULL;
    _move(&_tmp3, TTAB(make_table(0)));
    set_tabvalue(_tmp3, __str_ct_key_1, __str_ct_value_at_3);
    set_tabvalue(this, __str_ct_attr_2, _tmp3);
    printh(__str_ct_func_in__4);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp4 = T_NULL;
    _move(&_tmp4, TTAB(make_table(0)));
    TValue_t gc _tmp5 = T_NULL;
    _move(&_tmp5, TTAB(make_table(0)));
    set_tabvalue(_tmp5, __str_ct_x_6, TNUM(0));
    set_tabvalue(_tmp5, __str_ct_y_7, TNUM(-68));
    set_tabvalue(_tmp4, __str_ct_top_8, _tmp5);
    TValue_t gc _tmp6 = T_NULL;
    _move(&_tmp6, TTAB(make_table(0)));
    set_tabvalue(_tmp6, __str_ct_x_6, TNUM(0));
    set_tabvalue(_tmp6, __str_ct_y_7, TNUM(68));
    set_tabvalue(_tmp4, __str_ct_bottom_9, _tmp6);
    TValue_t gc _tmp7 = T_NULL;
    _move(&_tmp7, TTAB(make_table(0)));
    set_tabvalue(_tmp7, __str_ct_x_6, TNUM(-68));
    set_tabvalue(_tmp7, __str_ct_y_7, TNUM(0));
    set_tabvalue(_tmp4, __str_ct_left_10, _tmp7);
    TValue_t gc _tmp8 = T_NULL;
    _move(&_tmp8, TTAB(make_table(0)));
    set_tabvalue(_tmp8, __str_ct_x_6, TNUM(68));
    set_tabvalue(_tmp8, __str_ct_y_7, TNUM(0));
    set_tabvalue(_tmp4, __str_ct_right_11, _tmp8);
    _set(&literals, _tmp4);
    printh(get_tabvalue(get_tabvalue(literals, __str_ct_top_8), __str_ct_y_7));
    printh(get_tabvalue(get_tabvalue(literals, __str_ct_right_11), __str_ct_x_6));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(get_tabvalue(func_in_tab, __str_ct_func_5), ((TVSlice_t){(TValue_t[]){func_in_tab}, 1})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(12);
    _set(&__str_ct_value_to_0, TSTRi(_store_str_at_or_die(CONSTSTR("value_top"), 0)));
    _set(&__str_ct_key_1, TSTRi(_store_str_at_or_die(CONSTSTR("key"), 1)));
    _set(&__str_ct_attr_2, TSTRi(_store_str_at_or_die(CONSTSTR("attr"), 2)));
    _set(&__str_ct_value_at_3, TSTRi(_store_str_at_or_die(CONSTSTR("value_attr"), 3)));
    _set(&__str_ct_func_in__4, TSTRi(_store_str_at_or_die(CONSTSTR("func in tab"), 4)));
    _set(&__str_ct_func_5, TSTRi(_store_str_at_or_die(CONSTSTR("func"), 5)));
    _set(&__str_ct_x_6, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 6)));
    _set(&__str_ct_y_7, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 7)));
    _set(&__str_ct_top_8, TSTRi(_store_str_at_or_die(CONSTSTR("top"), 8)));
    _set(&__str_ct_bottom_9, TSTRi(_store_str_at_or_die(CONSTSTR("bottom"), 9)));
    _set(&__str_ct_left_10, TSTRi(_store_str_at_or_die(CONSTSTR("left"), 10)));
    _set(&__str_ct_right_11, TSTRi(_store_str_at_or_die(CONSTSTR("right"), 11)));

    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&this, _tmp0);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_key_1, __str_ct_value_to_0);
    set_tabvalue(this, __str_ct_attr_2, _tmp1);
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(0)));
    set_tabvalue(_tmp2, __str_ct_func_5, TFUN(_anon_0));
    _set(&func_in_tab, _tmp2);
    return T_NULL;
}