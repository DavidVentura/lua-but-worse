#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_a_0;

TValue_t a;
TValue_t b;
TValue_t c;
TValue_t d;
TValue_t e;
TValue_t o;
TValue_t u;
TValue_t t;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc x = T_NULL;
    _set(&x, TNUM(1));
    _move(&x, _add(x, TNUM(2)));
    _set(&a, TNUM(1));
    _set(&b, TNUM(2));
    _move(&c, flr(TNUM(fix32_from_float(1.5f))));
    _move(&d, flr(TNUM(fix32_from_float(2.5f))));
    _set(&e, TNUM(1));
    _set(&e, TNUM(2));
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&o, _tmp0);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_a_0, TNUM(1));
    _set(&u, _tmp1);
    set_tabvalue(o, __str_ct_a_0, get_tabvalue(u, __str_ct_a_0));
    _set(&t, TNUM(7));
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(0)));
    set_tabvalue(_tmp2, TNUM(7), TNUM(8));
    set_tabvalue(_tmp2, TNUM(8), TNUM(7));
    _move(&t, get_tabvalue(_tmp2, t));
    printh(_mult(a, c));
    printh(_mult(b, d));
    printh(e);
    printh(get_tabvalue(o, __str_ct_a_0));
    printh(t);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_a_0, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));

    return T_NULL;
}