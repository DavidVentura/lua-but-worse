#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc b = T_NULL;
    _set(&b, TNUM(1));
    TValue_t gc a = T_NULL;
    _set(&a, TNUM(1));
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, _add(a, TNUM(1)));
    TValue_t gc _tmp1 = T_NULL;
    _set(&_tmp1, T_NULL);
    if (__bool(_tmp0)) {
            _move(&_tmp1, _add(b, TNUM(1)));
        } else {
            _set(&_tmp1, _tmp0);
        }
    if (__bool(_tmp1)) {
            _set(&a, TNUM(2));
        }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}