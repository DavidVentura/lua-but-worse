#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    printh(_and(TNUM(4), TNUM(5)));
    printh(_and(T_NULL, TNUM(13)));
    printh(_and(T_FALSE, TNUM(13)));
    printh(_or(TNUM(4), TNUM(5)));
    printh(_or(T_FALSE, TNUM(5)));
    printh(_and(T_TRUE, _not(T_TRUE)));
    printh(_and(T_TRUE, _not(T_FALSE)));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}