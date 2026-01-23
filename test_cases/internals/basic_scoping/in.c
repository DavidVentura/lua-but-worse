#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc b;
    _set(&b, TNUM(1));
    TValue_t gc a;
    _set(&a, TNUM(1));
    if (__bool(_and(_add(a, TNUM(1)), _add(b, TNUM(1))))) {
            _set(&a, TNUM(2));
        }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}