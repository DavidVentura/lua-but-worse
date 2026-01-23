#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;
TValue_t __str_ct_tables_2;
TValue_t __str_ct_a_3;
TValue_t __str_ct_b_4;

TValue_t a;
TValue_t b;
TValue_t c;
TValue_t state;
TValue_t next_state;
TValue_t change_state;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&a, _tmp0);
    set_tabvalue(a, __str_ct_x_0, TNUM(5));
    set_tabvalue(a, __str_ct_y_1, TNUM(6));
    printh(get_tabvalue(a, __str_ct_x_0));
    printh(get_tabvalue(a, __str_ct_y_1));
    _set(&b, TNUM(1));
    _set(&c, TNUM(2));
    printh(b);
    printh(c);
    printh(__str_ct_tables_2);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(1)));
    set_tabvalue(_tmp1, __str_ct_a_3, TNUM(1));
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(1)));
    set_tabvalue(_tmp2, __str_ct_b_4, TNUM(2));
    _set(&state, _tmp1);
    _set(&next_state, _tmp2);
    _set(&change_state, T_FALSE);
    printh(get_tabvalue(state, __str_ct_a_3));
    printh(get_tabvalue(next_state, __str_ct_b_4));
    printh(change_state);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(5);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));
    _set(&__str_ct_tables_2, TSTRi(_store_str_at_or_die(CONSTSTR("tables"), 2)));
    _set(&__str_ct_a_3, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 3)));
    _set(&__str_ct_b_4, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 4)));

    return T_NULL;
}