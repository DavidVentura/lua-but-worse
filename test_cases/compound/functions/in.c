#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_works_in_0;
TValue_t __str_ct_f_1;
TValue_t __str_ct_index_2;
TValue_t __str_ct_works_in_3;

TValue_t g;
TValue_t captured;
TValue_t a;
TValue_t b;
TValue_t c;
TValue_t v;

TValue_t _anon_0(TVSlice_t args);
TValue_t _anon_1(TVSlice_t args);
TValue_t _anon_2(TVSlice_t args);
TValue_t _anon_3(TVSlice_t args);
TValue_t _anon_4(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _anon_0(TVSlice_t args) {
    TValue_t par = (args.num > 0) ? args.elems[0] : T_NULL;

    printh(par);
    return T_NULL;
}

TValue_t _anon_1(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;

    return _mult(x, captured);
}

TValue_t _anon_2(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t y = (args.num > 1) ? args.elems[1] : T_NULL;

    return _mult(x, y);
}

TValue_t _anon_3(TVSlice_t args) {
    return __str_ct_works_in_0;
}

TValue_t _anon_4(TVSlice_t args) {
    return __str_ct_works_in_3;
}

TValue_t main(TVSlice_t args) {
    _set(&captured, TNUM(7));
    _set(&a, TFUN(_anon_1));
    printh(CALL(a, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    _set(&b, TFUN(_anon_2));
    printh(CALL(b, ((TVSlice_t){(TValue_t[]){TNUM(5), TNUM(6)}, 2})));
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&c, _tmp0);
    set_tabvalue(c, __str_ct_f_1, TFUN(_anon_3));
    printh(CALL(get_tabvalue(c, __str_ct_f_1), ((TVSlice_t){NULL, 0})));
    _set(&v, __str_ct_index_2);
    set_tabvalue(c, v, TFUN(_anon_4));
    printh(CALL(get_tabvalue(c, v), ((TVSlice_t){NULL, 0})));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(g, ((TVSlice_t){(TValue_t[]){TNUM(55)}, 1})));
    }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(4);
    _set(&__str_ct_works_in_0, TSTRi(_store_str_at_or_die(CONSTSTR("works inside a table"), 0)));
    _set(&__str_ct_f_1, TSTRi(_store_str_at_or_die(CONSTSTR("f"), 1)));
    _set(&__str_ct_index_2, TSTRi(_store_str_at_or_die(CONSTSTR("index"), 2)));
    _set(&__str_ct_works_in_3, TSTRi(_store_str_at_or_die(CONSTSTR("works inside a table, via hashmap"), 3)));

    _set(&g, TFUN(_anon_0));
    return T_NULL;
}