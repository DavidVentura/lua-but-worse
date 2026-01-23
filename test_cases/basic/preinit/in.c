#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t a;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    printh(a);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _set(&a, TNUM(5));
    _move(&a, _add(a, TNUM(1)));
    if (__bool(_lt(a, TNUM(3)))) {
            _set(&a, TNUM(7));
        }
    return T_NULL;
}