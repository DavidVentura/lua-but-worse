#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t b(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t b(TVSlice_t args) {
    printh(TNUM(5));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(b, ((TVSlice_t){NULL, 0})));
    }
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}