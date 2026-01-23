#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t deleted_tables(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t deleted_tables(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    TValue_t gc tab = T_NULL;
    _set(&tab, _tmp0);
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(1));
    {
            TValue_t gc i = T_NULL;
            _set(&i, TNUM(1));
            TValue_t gc tmp_0 = T_NULL;
            _set(&tmp_0, TNUM(5));
            while (__bool(_leq(i, tmp_0))) {
                        TValue_t gc _tmp1 = T_NULL;
                        _move(&_tmp1, TTAB(make_table(0)));
                        set_tabvalue(tab, i, _tmp1);
                        _move(&i, _add(i, TNUM8(1)));
                    }
        }
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(6));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(deleted_tables, ((TVSlice_t){NULL, 0})));
    }
    __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM(0));
    return T_NULL;
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}