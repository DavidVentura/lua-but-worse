#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc a;
    _set(&a, TNUM(5));
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, TNUM(1), TNUM(1));
    set_tabvalue(_tmp0, TNUM(2), TNUM(2));
    set_tabvalue(_tmp0, TNUM(3), TNUM(3));
    set_tabvalue(_tmp0, TNUM(4), TNUM(4));
    TValue_t gc t;
    _set(&t, _tmp0);
    {
            TValue_t gc _iter;
            _set(&_iter, t);
            int16_t tmp_0 = _sequential_until(_iter);
            for (int16_t __i = 1; (__i <= tmp_0); __i++) {
                        TValue_t gc item;
                        _set(&item, get_tabvalue(_iter, TNUM(__i)));
                        printh(item);
                    }
        }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}