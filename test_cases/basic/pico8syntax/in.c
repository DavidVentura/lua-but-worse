#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t a;
TValue_t b;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    _set(&a, TNUM(fix32_from_parts(0, 0x8000)));
    _set(&b, TNUM(fix32_from_parts(0, 0x1000)));
    printh(a);
    printh(b);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}