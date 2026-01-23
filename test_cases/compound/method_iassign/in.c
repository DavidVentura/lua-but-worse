#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct___index_0;
TValue_t __str_ct_x_1;
TValue_t __str_ct_y_2;
TValue_t __str_ct_len_3;
TValue_t __str_ct_a_4;
TValue_t __str_ct_normal_5;
TValue_t __str_ct_new_6;

TValue_t vector;

TValue_t vector_len(TVSlice_t args);
TValue_t vector_normal(TVSlice_t args);
TValue_t vector_new(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t vector_len(TVSlice_t args) {
    TValue_t self = (args.num > 0) ? args.elems[0] : T_NULL;

    _return(_sqrt(_add(_mult(get_tabvalue(self, __str_ct_x_1), get_tabvalue(self, __str_ct_x_1)), _mult(get_tabvalue(self, __str_ct_y_2), get_tabvalue(self, __str_ct_y_2)))));
}

TValue_t vector_normal(TVSlice_t args) {
    TValue_t self = (args.num > 0) ? args.elems[0] : T_NULL;

    set_tabvalue(self, __str_ct_a_4, CALL(get_tabvalue(self, TSTR("len")), ((TVSlice_t){(TValue_t[]){self}, 1})));
    set_tabvalue(self, __str_ct_x_1, _div(get_tabvalue(self, __str_ct_x_1), CALL(get_tabvalue(self, TSTR("len")), ((TVSlice_t){(TValue_t[]){self}, 1}))));
    set_tabvalue(self, __str_ct_y_2, _div(get_tabvalue(self, __str_ct_y_2), CALL(get_tabvalue(self, TSTR("len")), ((TVSlice_t){(TValue_t[]){self}, 1}))));
    return T_NULL;
}

TValue_t vector_new(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t y = (args.num > 1) ? args.elems[1] : T_NULL;

    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(2)));
    set_tabvalue(_tmp1, __str_ct_x_1, x);
    set_tabvalue(_tmp1, __str_ct_y_2, y);
    TValue_t gc o = T_NULL;
    _set(&o, _tmp1);
    setmetatable(o, vector);
    _return(o);
}

TValue_t main(TVSlice_t args) {
    TValue_t gc zero = T_NULL;
    _move(&zero, CALL(get_tabvalue(vector, __str_ct_new_6), ((TVSlice_t){(TValue_t[]){TNUM(0), TNUM(0)}, 2})));
    printh(CALL(get_tabvalue(zero, TSTR("len")), ((TVSlice_t){(TValue_t[]){zero}, 1})));
    TValue_t gc one = T_NULL;
    _move(&one, CALL(get_tabvalue(vector, __str_ct_new_6), ((TVSlice_t){(TValue_t[]){TNUM(1), TNUM(1)}, 2})));
    printh(CALL(get_tabvalue(one, TSTR("len")), ((TVSlice_t){(TValue_t[]){one}, 1})));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(7);
    _set(&__str_ct___index_0, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 0)));
    _set(&__str_ct_x_1, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
    _set(&__str_ct_y_2, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 2)));
    _set(&__str_ct_len_3, TSTRi(_store_str_at_or_die(CONSTSTR("len"), 3)));
    _set(&__str_ct_a_4, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 4)));
    _set(&__str_ct_normal_5, TSTRi(_store_str_at_or_die(CONSTSTR("normal"), 5)));
    _set(&__str_ct_new_6, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 6)));

    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&vector, _tmp0);
    set_tabvalue(vector, __str_ct___index_0, vector);
    set_tabvalue(vector, __str_ct_len_3, TFUN(vector_len));
    set_tabvalue(vector, __str_ct_normal_5, TFUN(vector_normal));
    set_tabvalue(vector, __str_ct_new_6, TFUN(vector_new));
    return T_NULL;
}