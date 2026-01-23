#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_a_0;
TValue_t __str_ct_b_1;
TValue_t __str_ct_c_2;
TValue_t __str_ct_d_3;
TValue_t __str_ct_e_4;
TValue_t __str_ct_f_5;
TValue_t __str_ct_ipairs_o_6;
TValue_t __str_ct_ipairs_o_7;
TValue_t __str_ct_n_8;
TValue_t __str_ct_x_9;
TValue_t __str_ct_y_10;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, __str_ct_a_0, TNUM(1));
    set_tabvalue(_tmp0, __str_ct_b_1, TNUM(1));
    set_tabvalue(_tmp0, __str_ct_c_2, TNUM(2));
    set_tabvalue(_tmp0, __str_ct_d_3, TNUM(3));
    set_tabvalue(_tmp0, __str_ct_e_4, TNUM(5));
    set_tabvalue(_tmp0, __str_ct_f_5, TNUM(8));
    TValue_t gc tab;
    _set(&tab, _tmp0);
    {
            TValue_t gc _iter;
            _set(&_iter, tab);
            Table_t* _tab = GETTAB(_iter);
            uint16_t tmp_0 = _tab->kvp.len;
            for (uint16_t __i = 0; (__i < tmp_0); __i++) {
                        TValue_t k = _get_key_at(_tab, __i);
                        if ((k.tag != NUL)) {
                                        TValue_t v = _get_val_at(_tab, __i);
                                        printh(k);
                                        printh(v);
                                    }
                    }
        }
    printh(__str_ct_ipairs_o_6);
    {
            TValue_t gc _iter;
            _set(&_iter, tab);
            int16_t tmp_1 = _sequential_until(_iter);
            for (int16_t __i = 1; (__i <= tmp_1); __i++) {
                        TValue_t gc k;
                        _set(&k, TNUM(__i));
                        TValue_t gc v;
                        _set(&v, get_tabvalue(_iter, k));
                        printh(k);
                        printh(v);
                    }
        }
    printh(__str_ct_ipairs_o_7);
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, TNUM(1), TNUM(123));
    set_tabvalue(_tmp1, TNUM(2), TNUM(456));
    set_tabvalue(_tmp1, __str_ct_n_8, TNUM(42));
    set_tabvalue(_tmp1, __str_ct_x_9, TNUM(100));
    set_tabvalue(_tmp1, __str_ct_y_10, TNUM(200));
    set_tabvalue(_tmp1, TNUM(3), TNUM(789));
    TValue_t gc numtab;
    _set(&numtab, _tmp1);
    {
            TValue_t gc _iter;
            _set(&_iter, numtab);
            int16_t tmp_2 = _sequential_until(_iter);
            for (int16_t __i = 1; (__i <= tmp_2); __i++) {
                        TValue_t gc k;
                        _set(&k, TNUM(__i));
                        TValue_t gc v;
                        _set(&v, get_tabvalue(_iter, k));
                        printh(v);
                    }
        }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(11);
    _set(&__str_ct_a_0, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));
    _set(&__str_ct_b_1, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 1)));
    _set(&__str_ct_c_2, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 2)));
    _set(&__str_ct_d_3, TSTRi(_store_str_at_or_die(CONSTSTR("d"), 3)));
    _set(&__str_ct_e_4, TSTRi(_store_str_at_or_die(CONSTSTR("e"), 4)));
    _set(&__str_ct_f_5, TSTRi(_store_str_at_or_die(CONSTSTR("f"), 5)));
    _set(&__str_ct_ipairs_o_6, TSTRi(_store_str_at_or_die(CONSTSTR("ipairs over fib table"), 6)));
    _set(&__str_ct_ipairs_o_7, TSTRi(_store_str_at_or_die(CONSTSTR("ipairs over num table"), 7)));
    _set(&__str_ct_n_8, TSTRi(_store_str_at_or_die(CONSTSTR("n"), 8)));
    _set(&__str_ct_x_9, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 9)));
    _set(&__str_ct_y_10, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 10)));

    return T_NULL;
}