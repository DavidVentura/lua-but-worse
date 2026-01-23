#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_y_1;
TValue_t __str_ct_new_2;

TValue_t vector;

TValue_t vector_new(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t vector_new(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t y = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_x_0, x);
    set_tabvalue(_tmp1, __str_ct_y_1, y);
    TValue_t gc o;
    _set(&o, _tmp1);
    setmetatable(o, vector);
    return o;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc zero;
    _set(&zero, CALL(get_tabvalue(vector, __str_ct_new_2), ((TVSlice_t){(TValue_t[]){TNUM(0), TNUM(0)}, 2})));
    TValue_t gc one;
    _set(&one, CALL(get_tabvalue(vector, __str_ct_new_2), ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(1)}, 2})));
    printh(get_tabvalue(zero, __str_ct_x_0));
    printh(get_tabvalue(one, __str_ct_x_0));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_y_1, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));
    _set(&__str_ct_new_2, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 2)));

    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&vector, _tmp0);
    set_tabvalue(vector, __str_ct_new_2, TFUN(vector_new));
    return T_NULL;
}