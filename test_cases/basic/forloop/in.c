#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    {
            TValue_t gc i = T_NULL;
            _set(&i, TNUM(1));
            TValue_t gc tmp_0 = T_NULL;
            _set(&tmp_0, TNUM(7));
            while (__bool(_leq(i, tmp_0))) {
                        printh(i);
                        _move(&i, _add(i, TNUM8(1)));
                    }
        }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}