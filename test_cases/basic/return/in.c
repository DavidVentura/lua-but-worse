#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_field_0;

TValue_t returned_tables_1(TVSlice_t args);
TValue_t returned_tables_2(TVSlice_t args);
TValue_t returned_tables_3(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t returned_tables_1(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(1)));
    set_tabvalue(_tmp0, __str_ct_field_0, TNUM(1));
    TValue_t gc var = T_NULL;
    _set(&var, _tmp0);
    _return(var);
}

TValue_t returned_tables_2(TVSlice_t args) {
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(1)));
    set_tabvalue(_tmp1, __str_ct_field_0, TNUM(1));
    _return(_tmp1);
}

TValue_t returned_tables_3(TVSlice_t args) {
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(0)));
    _return(_tmp2);
}

TValue_t main(TVSlice_t args) {
    TValue_t gc t = T_NULL;
    _move(&t, CALL(returned_tables_1, ((TVSlice_t){NULL, 0})));
    printh(get_tabvalue(t, __str_ct_field_0));
    TValue_t gc t2 = T_NULL;
    _move(&t2, CALL(returned_tables_2, ((TVSlice_t){NULL, 0})));
    printh(get_tabvalue(t2, __str_ct_field_0));
    TValue_t gc t3 = T_NULL;
    _move(&t3, CALL(returned_tables_3, ((TVSlice_t){NULL, 0})));
    printh(_length(t3));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_field_0, TSTRi(_store_str_at_or_die(CONSTSTR("field"), 0)));

    return T_NULL;
}