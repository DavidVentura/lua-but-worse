#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;

TValue_t particles;

TValue_t add_particle(TVSlice_t args);
TValue_t _anon_0(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t add_particle(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t y = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(2)));
    set_tabvalue(_tmp1, __str_ct_x_0, x);
    set_tabvalue(_tmp1, __str_ct_y_1, y);
    {
        TValue_t gc _tmp;
        _set(&_tmp, add(particles, _tmp1));
    }
    return T_NULL;
}

TValue_t _anon_0(TVSlice_t args) {
    TValue_t particle = (args.num > 0) ? args.elems[0] : T_NULL;

    printh(get_tabvalue(particle, __str_ct_x_0));
    printh(get_tabvalue(particle, __str_ct_y_1));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(add_particle, ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(5)}, 2})));
    }
    foreach(particles, TFUN(_anon_0));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(2);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));

    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&particles, _tmp0);
    return T_NULL;
}