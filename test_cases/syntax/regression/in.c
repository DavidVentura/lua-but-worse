#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_fraction_0;
TValue_t __str_ct_rem_1;

TValue_t a;
TValue_t obj;
TValue_t axis;

TValue_t comment(TVSlice_t args);
TValue_t bool_weird(TVSlice_t args);
TValue_t mult_or(TVSlice_t args);
TValue_t or_call(TVSlice_t args);
TValue_t fractional_binary_literal(TVSlice_t args);
TValue_t mod_equal(TVSlice_t args);
TValue_t inplace_arith_for_bracket_table_assign(TVSlice_t args);
TValue_t short_while(TVSlice_t args);
TValue_t peeks(TVSlice_t args);
TValue_t integer_div(TVSlice_t args);
TValue_t bunny2(TVSlice_t args);
TValue_t bunny(TVSlice_t args);
TValue_t celeste(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t comment(TVSlice_t args) {
    return T_NULL;
}

TValue_t bool_weird(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, printh(TNUM(0)));
    TValue_t gc _tmp1 = T_NULL;
    _set(&_tmp1, T_NULL);
    if (__bool(_tmp0)) {
            _move(&_tmp1, CALL(mult_or, ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(1)}, 2})));
        } else {
            _set(&_tmp1, _tmp0);
        }
    if (__bool(_tmp1)) {
            _set(&a, TNUM(1));
        } else {
            TValue_t gc _tmp2 = T_NULL;
            _move(&_tmp2, printh(TNUM(1111)));
            TValue_t gc _tmp3 = T_NULL;
            _set(&_tmp3, T_NULL);
            if (__bool(_tmp2)) {
                        _move(&_tmp3, CALL(mult_or, ((TVSlice_t){(TValue_t[]){TNUM(4444), TNUM(5555)}, 2})));
                    } else {
                        _set(&_tmp3, _tmp2);
                    }
            if (__bool(_tmp3)) {
                        _set(&a, TNUM(1));
                    }
        }
    return T_NULL;
}

TValue_t mult_or(TVSlice_t args) {
    TValue_t gc w = T_NULL;
    _set(&w, TNUM(2));
    TValue_t gc _tmp4 = T_NULL;
    _set(&_tmp4, w);
    TValue_t gc _tmp5 = T_NULL;
    _set(&_tmp5, T_NULL);
    if (__bool(_tmp4)) {
            _set(&_tmp5, _tmp4);
        } else {
            _set(&_tmp5, TNUM(1));
        }
    TValue_t gc sw = T_NULL;
    _move(&sw, _mult(_tmp5, TNUM(8)));
    return T_NULL;
}

TValue_t or_call(TVSlice_t args) {
    TValue_t gc _tmp6 = T_NULL;
    _set(&_tmp6, obj);
    TValue_t gc _tmp7 = T_NULL;
    _set(&_tmp7, T_NULL);
    if (__bool(_tmp6)) {
            _set(&_tmp7, _tmp6);
        } else {
            _set(&_tmp7, T_NULL);
        }
    TValue_t gc obj = T_NULL;
    _move(&obj, CALL(CALL(T_FALSE, ((TVSlice_t){(TValue_t[]){_tmp7}, 1})), ((TVSlice_t){(TValue_t[]){obj}, 1})));
    return T_NULL;
}

TValue_t fractional_binary_literal(TVSlice_t args) {
    printh(__str_ct_fraction_0);
    printh(TNUM(fix32_from_parts(0b0111110101111101, 0b1000000000000000)));
    return T_NULL;
}

TValue_t mod_equal(TVSlice_t args) {
    _move(&a, _mod(a, TNUM(5)));
    return T_NULL;
}

TValue_t inplace_arith_for_bracket_table_assign(TVSlice_t args) {
    TValue_t gc _tmp8 = T_NULL;
    _move(&_tmp8, TTAB(make_table(0)));
    _set(&obj, _tmp8);
    _set(&axis, TNUM(5));
    set_tabvalue(get_tabvalue(obj, __str_ct_rem_1), axis, _add(get_tabvalue(get_tabvalue(obj, __str_ct_rem_1), axis), axis));
    return T_NULL;
}

TValue_t short_while(TVSlice_t args) {
    while (__bool(T_FALSE)) {
            _set(&a, TNUM(5));
        }
    return T_NULL;
}

TValue_t peeks(TVSlice_t args) {
    return T_NULL;
}

TValue_t integer_div(TVSlice_t args) {
    printh(_floor_div(TNUM(7), TNUM(5)));
    printh(_floor_div(TNUM(8), TNUM(5)));
    printh(_floor_div(TNUM(11), TNUM(5)));
    return T_NULL;
}

TValue_t bunny2(TVSlice_t args) {
    if (__bool(T_FALSE)) {
            _return(T_NULL);
        }
    _set(&a, TNUM(1));
    return T_NULL;
}

TValue_t bunny(TVSlice_t args) {
    return T_NULL;
}

TValue_t celeste(TVSlice_t args) {
    if (__bool(T_FALSE)) {
            _return(T_NULL);
        }
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(celeste, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(integer_div, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(bunny, ((TVSlice_t){NULL, 0})));
    }
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(fractional_binary_literal, ((TVSlice_t){NULL, 0})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(2);
    _set(&__str_ct_fraction_0, TSTRi(_store_str_at_or_die(CONSTSTR("fractional_binary_literal"), 0)));
    _set(&__str_ct_rem_1, TSTRi(_store_str_at_or_die(CONSTSTR("rem"), 1)));

    return T_NULL;
}