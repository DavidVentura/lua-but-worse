#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t c;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc a;
    _set(&a, TNUM(5));
    TValue_t gc b;
    _set(&b, TNUM(7));
    _set(&c, _mult(a, b));
    printh(c);
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}