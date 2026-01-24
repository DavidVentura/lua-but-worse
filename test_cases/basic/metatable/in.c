#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t t;
TValue_t t1;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, TTAB(make_table(0)));
    _set(&t, _tmp0);
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, getmetatable(t));
    printh(_tmp1);
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, TTAB(make_table(0)));
    _set(&t1, _tmp2);
    setmetatable(t, t1);
    TValue_t gc _tmp3 = T_NULL;
    _move(&_tmp3, getmetatable(t));
    printh(_equal(_tmp3, t1));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}