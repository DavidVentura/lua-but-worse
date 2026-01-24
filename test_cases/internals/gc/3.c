#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t _references_in_table_destroyed(TVSlice_t args);
TValue_t test_references_in_table_destroyed(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _references_in_table_destroyed(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    TValue_t gc tab = T_NULL;
    _set(&tab, _tmp0);
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(1));
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, TTAB(make_table(0)));
    set_tabvalue(tab, TNUM(0), _tmp1);
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(2));
    return T_NULL;
}

TValue_t test_references_in_table_destroyed(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(_references_in_table_destroyed, ((TVSlice_t){NULL, 0})));
    }
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_references_in_table_destroyed, ((TVSlice_t){NULL, 0})));
    }
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}