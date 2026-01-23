#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct___index_0;
TValue_t __str_ct_x_1;
TValue_t __str_ct_y_2;
TValue_t __str_ct_new_3;
TValue_t __str_ct___add_4;
TValue_t __str_ct___sub_5;

TValue_t vector;

TValue_t vector_new(TVSlice_t args);
TValue_t vector___add(TVSlice_t args);
TValue_t vector___sub(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t vector_new(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t y = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(_tmp1, __str_ct_x_1, x);
    set_tabvalue(_tmp1, __str_ct_y_2, y);
    TValue_t gc o;
    _set(&o, _tmp1);
    setmetatable(o, vector);
    return o;
}

TValue_t vector___add(TVSlice_t args) {
    TValue_t v0 = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t v1 = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc v;
    _set(&v, CALL(get_tabvalue(vector, __str_ct_new_3), ((TVSlice_t){(TValue_t[]){TNUM(0), TNUM(0)}, 2})));
    set_tabvalue(v, __str_ct_x_1, _add(get_tabvalue(v0, __str_ct_x_1), get_tabvalue(v1, __str_ct_x_1)));
    set_tabvalue(v, __str_ct_y_2, _add(get_tabvalue(v0, __str_ct_y_2), get_tabvalue(v1, __str_ct_y_2)));
    return v;
}

TValue_t vector___sub(TVSlice_t args) {
    TValue_t v0 = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t v1 = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc v;
    _set(&v, CALL(get_tabvalue(vector, __str_ct_new_3), ((TVSlice_t){(TValue_t[]){TNUM(0), TNUM(0)}, 2})));
    set_tabvalue(v, __str_ct_x_1, _sub(get_tabvalue(v0, __str_ct_x_1), get_tabvalue(v1, __str_ct_x_1)));
    set_tabvalue(v, __str_ct_y_2, _sub(get_tabvalue(v0, __str_ct_y_2), get_tabvalue(v1, __str_ct_y_2)));
    return v;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc v1;
    _set(&v1, CALL(get_tabvalue(vector, __str_ct_new_3), ((TVSlice_t){(TValue_t[]){TNUM(5), TNUM(3)}, 2})));
    TValue_t gc v2;
    _set(&v2, CALL(get_tabvalue(vector, __str_ct_new_3), ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(2)}, 2})));
    TValue_t gc delta;
    _set(&delta, _sub(v1, v2));
    printh(get_tabvalue(delta, __str_ct_x_1));
    printh(get_tabvalue(delta, __str_ct_y_2));
    TValue_t gc added;
    _set(&added, _add(v1, v2));
    printh(get_tabvalue(added, __str_ct_x_1));
    printh(get_tabvalue(added, __str_ct_y_2));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(6);
    _set(&__str_ct___index_0, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 0)));
    _set(&__str_ct_x_1, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
    _set(&__str_ct_y_2, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 2)));
    _set(&__str_ct_new_3, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 3)));
    _set(&__str_ct___add_4, TSTRi(_store_str_at_or_die(CONSTSTR("__add"), 4)));
    _set(&__str_ct___sub_5, TSTRi(_store_str_at_or_die(CONSTSTR("__sub"), 5)));

    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&vector, _tmp0);
    set_tabvalue(vector, __str_ct___index_0, vector);
    set_tabvalue(vector, __str_ct_new_3, TFUN(vector_new));
    set_tabvalue(vector, __str_ct___add_4, TFUN(vector___add));
    set_tabvalue(vector, __str_ct___sub_5, TFUN(vector___sub));
    return T_NULL;
}