#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_field_0;

TValue_t returned_tables_1(TVSlice_t args);
TValue_t returned_tables(TVSlice_t args);
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

TValue_t returned_tables(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    TValue_t gc t = T_NULL;
    _move(&t, CALL(returned_tables_1, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(1));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(returned_tables, ((TVSlice_t){NULL, 0})));
    }
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_field_0, TSTRi(_store_str_at_or_die(CONSTSTR("field"), 0)));

    return T_NULL;
}