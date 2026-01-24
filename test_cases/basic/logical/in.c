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
    TValue_t gc _tmp0 = T_NULL;
    _set(&_tmp0, TNUM(4));
    TValue_t gc _tmp1 = T_NULL;
    _set(&_tmp1, T_NULL);
    if (__bool(_tmp0)) {
            _set(&_tmp1, TNUM(5));
        } else {
            _set(&_tmp1, _tmp0);
        }
    TValue_t gc _tmp2 = T_NULL;
    _set(&_tmp2, _tmp1);
    TValue_t gc _tmp3 = T_NULL;
    _set(&_tmp3, T_NULL);
    if (__bool(_tmp2)) {
            _set(&_tmp3, TNUM(6));
        } else {
            _set(&_tmp3, _tmp2);
        }
    _set(&a, _tmp3);
    printh(a);
    TValue_t gc _tmp4 = T_NULL;
    _set(&_tmp4, T_NULL);
    TValue_t gc _tmp5 = T_NULL;
    _set(&_tmp5, T_NULL);
    if (__bool(_tmp4)) {
            _set(&_tmp5, TNUM(13));
        } else {
            _set(&_tmp5, _tmp4);
        }
    _set(&a, _tmp5);
    printh(a);
    TValue_t gc _tmp6 = T_NULL;
    _set(&_tmp6, T_FALSE);
    TValue_t gc _tmp7 = T_NULL;
    _set(&_tmp7, T_NULL);
    if (__bool(_tmp6)) {
            _set(&_tmp7, TNUM(13));
        } else {
            _set(&_tmp7, _tmp6);
        }
    _set(&a, _tmp7);
    printh(a);
    TValue_t gc _tmp8 = T_NULL;
    _set(&_tmp8, TNUM(4));
    TValue_t gc _tmp9 = T_NULL;
    _set(&_tmp9, T_NULL);
    if (__bool(_tmp8)) {
            _set(&_tmp9, _tmp8);
        } else {
            _set(&_tmp9, TNUM(5));
        }
    _set(&a, _tmp9);
    printh(a);
    TValue_t gc _tmp10 = T_NULL;
    _set(&_tmp10, T_FALSE);
    TValue_t gc _tmp11 = T_NULL;
    _set(&_tmp11, T_NULL);
    if (__bool(_tmp10)) {
            _set(&_tmp11, _tmp10);
        } else {
            _set(&_tmp11, TNUM(5));
        }
    _set(&a, _tmp11);
    printh(a);
    printh(__str_ct______0);
    TValue_t gc _tmp12 = T_NULL;
    _set(&_tmp12, TNUM(5));
    TValue_t gc _tmp13 = T_NULL;
    _set(&_tmp13, T_NULL);
    if (__bool(_tmp12)) {
            _move(&_tmp13, printh(__str_ct_visible_1));
        } else {
            _set(&_tmp13, _tmp12);
        }
    _set(&a, _tmp13);
    TValue_t gc _tmp14 = T_NULL;
    _set(&_tmp14, TNUM(5));
    TValue_t gc _tmp15 = T_NULL;
    _set(&_tmp15, T_NULL);
    if (__bool(_tmp14)) {
            _move(&_tmp15, printh(__str_ct_visible2_2));
        } else {
            _set(&_tmp15, _tmp14);
        }
    TValue_t gc _tmp16 = T_NULL;
    _set(&_tmp16, _tmp15);
    TValue_t gc _tmp17 = T_NULL;
    _set(&_tmp17, T_NULL);
    if (__bool(_tmp16)) {
            _move(&_tmp17, printh(__str_ct_also_vis_3));
        } else {
            _set(&_tmp17, _tmp16);
        }
    _set(&a, _tmp17);
    TValue_t gc _tmp18 = T_NULL;
    _set(&_tmp18, T_FALSE);
    TValue_t gc _tmp19 = T_NULL;
    _set(&_tmp19, T_NULL);
    if (__bool(_tmp18)) {
            _move(&_tmp19, printh(__str_ct_NOT_visi_4));
        } else {
            _set(&_tmp19, _tmp18);
        }
    TValue_t gc _tmp20 = T_NULL;
    _set(&_tmp20, _tmp19);
    TValue_t gc _tmp21 = T_NULL;
    _set(&_tmp21, T_NULL);
    if (__bool(_tmp20)) {
            _move(&_tmp21, printh(__str_ct_also_NOT_5));
        } else {
            _set(&_tmp21, _tmp20);
        }
    _set(&a, _tmp21);
    TValue_t gc _tmp22 = T_NULL;
    _set(&_tmp22, T_FALSE);
    TValue_t gc _tmp23 = T_NULL;
    _set(&_tmp23, T_NULL);
    if (__bool(_tmp22)) {
            _move(&_tmp23, printh(__str_ct_AND_NOT__6));
        } else {
            _set(&_tmp23, _tmp22);
        }
    _set(&a, _tmp23);
    TValue_t gc _tmp24 = T_NULL;
    _set(&_tmp24, T_TRUE);
    TValue_t gc _tmp25 = T_NULL;
    _set(&_tmp25, T_NULL);
    if (__bool(_tmp24)) {
            _set(&_tmp25, _tmp24);
        } else {
            _move(&_tmp25, printh(__str_ct_OR_NOT_V_7));
        }
    _set(&a, _tmp25);
    TValue_t gc _tmp26 = T_NULL;
    _set(&_tmp26, TNUM(4));
    TValue_t gc _tmp27 = T_NULL;
    _set(&_tmp27, T_NULL);
    if (__bool(_tmp26)) {
            _set(&_tmp27, TNUM(5));
        } else {
            _set(&_tmp27, _tmp26);
        }
    TValue_t gc _tmp28 = T_NULL;
    _set(&_tmp28, _tmp27);
    TValue_t gc _tmp29 = T_NULL;
    _set(&_tmp29, T_NULL);
    if (__bool(_tmp28)) {
            _set(&_tmp29, TNUM(6));
        } else {
            _set(&_tmp29, _tmp28);
        }
    if (__bool(_tmp29)) {
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