#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_x_0;

TValue_t member;
TValue_t empty;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp0;
    _set(&_tmp0, TTAB(make_table(0)));
    set_tabvalue(_tmp0, __str_ct_x_0, TNUM(1));
    _set(&member, _tmp0);
    TValue_t gc _tmp1;
    _set(&_tmp1, TTAB(make_table(0)));
    _set(&empty, _tmp1);
    set_tabvalue(member, empty, TNUM(5));
    printh(get_tabvalue(member, empty));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_x_0, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));

    return T_NULL;
}