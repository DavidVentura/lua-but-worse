#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_str_0;
TValue_t __str_ct_somethin_1;
TValue_t __str_ct_test_sub_2;
TValue_t __str_ct_a_3;
TValue_t __str_ct_b_4;
TValue_t __str_ct_str_5;
TValue_t __str_ct_str_6;

TValue_t test_empty_thing(TVSlice_t args);
TValue_t test_sub(TVSlice_t args);
TValue_t test_concat(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t test_empty_thing(TVSlice_t args) {
    TValue_t gc an_empty_one = T_NULL;
    _set(&an_empty_one, __str_ct_str_0);
    TValue_t gc another_empty_one = T_NULL;
    _set(&another_empty_one, __str_ct_str_0);
    TValue_t gc n1 = T_NULL;
    _set(&n1, TNUM(5));
    TValue_t gc n2 = T_NULL;
    _set(&n2, TNUM(6));
    TValue_t gc a = T_NULL;
    _move(&a, _concat(an_empty_one, n1));
    TValue_t gc b = T_NULL;
    _move(&b, _concat(an_empty_one, n2));
    return T_NULL;
}

TValue_t test_sub(TVSlice_t args) {
    TValue_t gc s = T_NULL;
    _set(&s, __str_ct_somethin_1);
    printh(__str_ct_test_sub_2);
    printh(CALL(sub, ((TVSlice_t){(TValue_t[]){s, TNUM(5)}, 2})));
    printh(CALL(sub, ((TVSlice_t){(TValue_t[]){s, TNUM(-5)}, 2})));
    printh(CALL(sub, ((TVSlice_t){(TValue_t[]){s, TNUM(-4), TNUM(-3)}, 3})));
    printh(CALL(sub, ((TVSlice_t){(TValue_t[]){s, TNUM(-2), T_NULL}, 3})));
    printh(CALL(sub, ((TVSlice_t){(TValue_t[]){s, TNUM(8), T_NULL}, 3})));
    return T_NULL;
}

TValue_t test_concat(TVSlice_t args) {
    printh(__str_ct_a_3);
    printh(_concat(__str_ct_a_3, __str_ct_b_4));
    TValue_t gc ones = T_NULL;
    _set(&ones, __str_ct_str_5);
    TValue_t gc twos = T_NULL;
    _set(&twos, __str_ct_str_6);
    printh(_concat(ones, twos));
    printh(_concat(__str_ct_a_3, TNUM(5)));
    printh(_concat(__str_ct_str_0, TNUM(5)));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_empty_thing, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_sub, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_concat, ((TVSlice_t){NULL, 0})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(7);
    _set(&__str_ct_str_0, TSTRi(_store_str_at_or_die(CONSTSTR(""), 0)));
    _set(&__str_ct_somethin_1, TSTRi(_store_str_at_or_die(CONSTSTR("something"), 1)));
    _set(&__str_ct_test_sub_2, TSTRi(_store_str_at_or_die(CONSTSTR("test_sub"), 2)));
    _set(&__str_ct_a_3, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 3)));
    _set(&__str_ct_b_4, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 4)));
    _set(&__str_ct_str_5, TSTRi(_store_str_at_or_die(CONSTSTR("11111111111111111111"), 5)));
    _set(&__str_ct_str_6, TSTRi(_store_str_at_or_die(CONSTSTR("22222222222222222222"), 6)));

    return T_NULL;
}