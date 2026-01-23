#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;
TValue_t __str_ct_method_1;

TValue_t a;

TValue_t a_method(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t a_method(TVSlice_t args) {
    TValue_t self = (args.num > 0) ? args.elems[0] : T_NULL;
    TValue_t value = (args.num > 1) ? args.elems[1] : T_NULL;

    set_tabvalue(self, __str_ct_x_0, value);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&a, _tmp0);
    set_tabvalue(a, __str_ct_method_1, TFUN(a_method));
    printh(get_tabvalue(a, __str_ct_x_0));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(get_tabvalue(a, TSTR("method")), ((TVSlice_t){(TValue_t[]){a, TNUM(5)}, 2})));
    }
    printh(get_tabvalue(a, __str_ct_x_0));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(2);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
    _set(&__str_ct_method_1, TSTRi(_store_str_at_or_die(CONSTSTR("method"), 1)));

    return T_NULL;
}