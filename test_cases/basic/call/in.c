#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t something(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t something(TVSlice_t args) {
    TValue_t arg = (args.num > 0) ? args.elems[0] : T_NULL;

    _return(arg);
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(something, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    }
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, CALL(something, ((TVSlice_t){(TValue_t[]){TNUM(6)}, 1})));
    printh(_tmp0);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}