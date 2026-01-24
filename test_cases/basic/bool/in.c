#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

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
    printh(_tmp1);
    TValue_t gc _tmp2 = T_NULL;
    _set(&_tmp2, T_NULL);
    TValue_t gc _tmp3 = T_NULL;
    _set(&_tmp3, T_NULL);
    if (__bool(_tmp2)) {
            _set(&_tmp3, TNUM(13));
        } else {
            _set(&_tmp3, _tmp2);
        }
    printh(_tmp3);
    TValue_t gc _tmp4 = T_NULL;
    _set(&_tmp4, T_FALSE);
    TValue_t gc _tmp5 = T_NULL;
    _set(&_tmp5, T_NULL);
    if (__bool(_tmp4)) {
            _set(&_tmp5, TNUM(13));
        } else {
            _set(&_tmp5, _tmp4);
        }
    printh(_tmp5);
    TValue_t gc _tmp6 = T_NULL;
    _set(&_tmp6, TNUM(4));
    TValue_t gc _tmp7 = T_NULL;
    _set(&_tmp7, T_NULL);
    if (__bool(_tmp6)) {
            _set(&_tmp7, _tmp6);
        } else {
            _set(&_tmp7, TNUM(5));
        }
    printh(_tmp7);
    TValue_t gc _tmp8 = T_NULL;
    _set(&_tmp8, T_FALSE);
    TValue_t gc _tmp9 = T_NULL;
    _set(&_tmp9, T_NULL);
    if (__bool(_tmp8)) {
            _set(&_tmp9, _tmp8);
        } else {
            _set(&_tmp9, TNUM(5));
        }
    printh(_tmp9);
    TValue_t gc _tmp10 = T_NULL;
    _set(&_tmp10, T_TRUE);
    TValue_t gc _tmp11 = T_NULL;
    _set(&_tmp11, T_NULL);
    if (__bool(_tmp10)) {
            _move(&_tmp11, _not(T_TRUE));
        } else {
            _set(&_tmp11, _tmp10);
        }
    printh(_tmp11);
    TValue_t gc _tmp12 = T_NULL;
    _set(&_tmp12, T_TRUE);
    TValue_t gc _tmp13 = T_NULL;
    _set(&_tmp13, T_NULL);
    if (__bool(_tmp12)) {
            _move(&_tmp13, _not(T_FALSE));
        } else {
            _set(&_tmp13, _tmp12);
        }
    printh(_tmp13);
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}