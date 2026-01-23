#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct______0;
TValue_t __str_ct_visible_1;
TValue_t __str_ct_visible2_2;
TValue_t __str_ct_also_vis_3;
TValue_t __str_ct_NOT_visi_4;
TValue_t __str_ct_also_NOT_5;
TValue_t __str_ct_AND_NOT__6;
TValue_t __str_ct_OR_NOT_V_7;
TValue_t __str_ct_yep_8;

TValue_t a;

TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t main(TVSlice_t args) {
    _move(&a, _and(_and(TNUM(4), TNUM(5)), TNUM(6)));
    printh(a);
    _move(&a, _and(T_NULL, TNUM(13)));
    printh(a);
    _move(&a, _and(T_FALSE, TNUM(13)));
    printh(a);
    _move(&a, _or(TNUM(4), TNUM(5)));
    printh(a);
    _move(&a, _or(T_FALSE, TNUM(5)));
    printh(a);
    printh(__str_ct______0);
    _move(&a, _and(TNUM(5), printh(__str_ct_visible_1)));
    _move(&a, _and(_and(TNUM(5), printh(__str_ct_visible2_2)), printh(__str_ct_also_vis_3)));
    _move(&a, _and(_and(T_FALSE, printh(__str_ct_NOT_visi_4)), printh(__str_ct_also_NOT_5)));
    _move(&a, _and(T_FALSE, printh(__str_ct_AND_NOT__6)));
    _move(&a, _or(T_TRUE, printh(__str_ct_OR_NOT_V_7)));
    if (__bool(_and(_and(TNUM(4), TNUM(5)), TNUM(6)))) {
            printh(__str_ct_yep_8);
        }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(9);
    _set(&__str_ct______0, TSTRi(_store_str_at_or_die(CONSTSTR("----"), 0)));
    _set(&__str_ct_visible_1, TSTRi(_store_str_at_or_die(CONSTSTR("visible"), 1)));
    _set(&__str_ct_visible2_2, TSTRi(_store_str_at_or_die(CONSTSTR("visible2"), 2)));
    _set(&__str_ct_also_vis_3, TSTRi(_store_str_at_or_die(CONSTSTR("also visible"), 3)));
    _set(&__str_ct_NOT_visi_4, TSTRi(_store_str_at_or_die(CONSTSTR("NOT visible"), 4)));
    _set(&__str_ct_also_NOT_5, TSTRi(_store_str_at_or_die(CONSTSTR("also NOT visible"), 5)));
    _set(&__str_ct_AND_NOT__6, TSTRi(_store_str_at_or_die(CONSTSTR("AND-NOT VISIBLE"), 6)));
    _set(&__str_ct_OR_NOT_V_7, TSTRi(_store_str_at_or_die(CONSTSTR("OR-NOT VISIBLE"), 7)));
    _set(&__str_ct_yep_8, TSTRi(_store_str_at_or_die(CONSTSTR("yep"), 8)));

    return T_NULL;
}