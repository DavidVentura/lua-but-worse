#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct___index_0;
TValue_t __str_ct_x_1;
TValue_t __str_ct_method_2;

TValue_t a;
TValue_t b;

TValue_t a_method(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t a_method(TVSlice_t args) {
    TValue_t self = (args.num > 0) ? args.elems[0] : T_NULL;

    set_tabvalue(self, __str_ct_x_1, TNUM(5));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&a, _tmp0);
    set_tabvalue(a, __str_ct___index_0, a);
    set_tabvalue(a, __str_ct_method_2, TFUN(a_method));
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    _set(&b, _tmp1);
    setmetatable(b, a);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(get_tabvalue(b, TSTR("method")), ((TVSlice_t){(TValue_t[]){b}, 1})));
    }
    printh(get_tabvalue(a, __str_ct_x_1));
    printh(get_tabvalue(b, __str_ct_x_1));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(get_tabvalue(a, TSTR("method")), ((TVSlice_t){(TValue_t[]){a}, 1})));
    }
    printh(get_tabvalue(a, __str_ct_x_1));
    printh(get_tabvalue(b, __str_ct_x_1));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct___index_0, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 0)));
    _set(&__str_ct_x_1, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
    _set(&__str_ct_method_2, TSTRi(_store_str_at_or_die(CONSTSTR("method"), 2)));

    return T_NULL;
}