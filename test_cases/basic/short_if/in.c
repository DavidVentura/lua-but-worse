#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_this_sho_0;

TValue_t test;
TValue_t a;
TValue_t b;
TValue_t c;
TValue_t d;
TValue_t e;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    _set(&test, T_FALSE);
    if (__bool(test)) {
            _return(TNUM(5));
        }
    if (__bool(test)) {
            _return(TNUM(5));
        } else {
            _set(&a, TNUM(1));
        }
    if (__bool(test)) {
            _return(TNUM(5));
        } else {
            _set(&b, TNUM(2));
        }
    _set(&c, TNUM(3));
    if (__bool(_not(test))) {
            _set(&d, TNUM(-4));
        } else {
            _set(&e, TNUM(0));
        }
    printh(a);
    printh(b);
    printh(c);
    printh(d);
    if (__bool(test)) {
            printh(__str_ct_this_sho_0);
        }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_this_sho_0, TSTRi(_store_str_at_or_die(CONSTSTR("this should not be visible"), 0)));

    return T_NULL;
}