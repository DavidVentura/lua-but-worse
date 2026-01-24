#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_hi_0;
TValue_t __str_ct_str_1;
TValue_t __str_ct_hi2_2;

TValue_t _inner_dyn(TVSlice_t args);
TValue_t _inner(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _inner_dyn(TVSlice_t args) {
    TValue_t gc _tmp0 = T_NULL;
    _move(&_tmp0, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp0, TNUM(3));
    TValue_t gc a = T_NULL;
    _set(&a, __str_ct_hi_0);
    TValue_t gc b = T_NULL;
    _move(&b, tostring(TNUM(3)));
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp1, TNUM(4));
    TValue_t gc c = T_NULL;
    _move(&c, _concat(a, b));
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp2, TNUM(5));
    return T_NULL;
}

TValue_t _inner(TVSlice_t args) {
    TValue_t gc _tmp3 = T_NULL;
    _move(&_tmp3, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp3, TNUM(3));
    TValue_t gc a = T_NULL;
    _set(&a, __str_ct_hi_0);
    TValue_t gc _tmp4 = T_NULL;
    _move(&_tmp4, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp4, TNUM(3));
    TValue_t gc b = T_NULL;
    _set(&b, __str_ct_str_1);
    TValue_t gc _tmp5 = T_NULL;
    _move(&_tmp5, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp5, TNUM(3));
    TValue_t gc c = T_NULL;
    _move(&c, _concat(__str_ct_hi_0, __str_ct_str_1));
    TValue_t gc _tmp6 = T_NULL;
    _move(&_tmp6, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp6, TNUM(3));
    TValue_t gc c_1 = T_NULL;
    _set(&c_1, __str_ct_hi2_2);
    TValue_t gc _tmp7 = T_NULL;
    _move(&_tmp7, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp7, TNUM(3));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(_inner, ((TVSlice_t){NULL, 0})));
    }
    TValue_t gc _tmp8 = T_NULL;
    _move(&_tmp8, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp8, TNUM(3));
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(_inner_dyn, ((TVSlice_t){NULL, 0})));
    }
    TValue_t gc _tmp9 = T_NULL;
    _move(&_tmp9, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp9, TNUM(3));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(3);
    _set(&__str_ct_hi_0, TSTRi(_store_str_at_or_die(CONSTSTR("hi"), 0)));
    _set(&__str_ct_str_1, TSTRi(_store_str_at_or_die(CONSTSTR("2"), 1)));
    _set(&__str_ct_hi2_2, TSTRi(_store_str_at_or_die(CONSTSTR("hi2"), 2)));

    return T_NULL;
}