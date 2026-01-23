#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;
TValue_t __str_ct_a_2;
TValue_t __str_ct_b_3;
TValue_t __str_ct_c_4;
TValue_t __str_ct_d_5;

TValue_t squares;
TValue_t member;
TValue_t empty;
TValue_t adder;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, TNUM(1), TNUM(1));
    set_tabvalue(_tmp0, TNUM(2), TNUM(4));
    set_tabvalue(_tmp0, TNUM(3), TNUM(9));
    set_tabvalue(_tmp0, TNUM(4), TNUM(16));
    set_tabvalue(_tmp0, TNUM(5), TNUM(25));
    _set(&squares, _tmp0);
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_x_0, TNUM(fix32_from_float(-1.5f)));
    set_tabvalue(_tmp1, __str_ct_y_1, TNUM(66));
    _set(&member, _tmp1);
    TValue_t gc _tmp2;
    _set(&_tmp2, TTAB(make_table(0)));
    _set(&empty, _tmp2);
    printh(get_tabvalue(squares, TNUM(2)));
    printh(get_tabvalue(member, __str_ct_x_0));
    printh(get_tabvalue(member, __str_ct_y_1));
    TValue_t gc _tmp3;
    _set(&_tmp3, TTAB(make_table(0)));
    _set(&adder, _tmp3);
    set_tabvalue(adder, __str_ct_a_2, TNUM(1));
    printh(get_tabvalue(adder, __str_ct_a_2));
    set_tabvalue(adder, __str_ct_b_3, TNUM(2));
    printh(get_tabvalue(adder, __str_ct_a_2));
    printh(get_tabvalue(adder, __str_ct_b_3));
    set_tabvalue(adder, __str_ct_c_4, TNUM(3));
    printh(get_tabvalue(adder, __str_ct_a_2));
    printh(get_tabvalue(adder, __str_ct_b_3));
    printh(get_tabvalue(adder, __str_ct_c_4));
    set_tabvalue(adder, __str_ct_d_5, TNUM(4));
    printh(get_tabvalue(adder, __str_ct_a_2));
    printh(get_tabvalue(adder, __str_ct_b_3));
    printh(get_tabvalue(adder, __str_ct_c_4));
    printh(get_tabvalue(adder, __str_ct_d_5));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(6);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));
    _set(&__str_ct_a_2, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 2)));
    _set(&__str_ct_b_3, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 3)));
    _set(&__str_ct_c_4, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 4)));
    _set(&__str_ct_d_5, TSTRi(_store_str_at_or_die(CONSTSTR("d"), 5)));

    return T_NULL;
}