#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t t;
TValue_t t1;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    _set(&t, _tmp0);
    printh(getmetatable(t));
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    _set(&t1, _tmp1);
    setmetatable(t, t1);
    printh(_equal(getmetatable(t), t1));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}