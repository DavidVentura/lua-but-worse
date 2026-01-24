#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct___0;
TValue_t __str_ct___1;

TValue_t _inner(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _inner(TVSlice_t args) {
    TValue_t gc s = T_NULL;
    _set(&s, __str_ct___0);
    TValue_t gc intermediate = T_NULL;
    _set(&intermediate, T_NULL);
    {
            TValue_t gc i = T_NULL;
            _set(&i, TNUM(1));
            TValue_t gc tmp_0 = T_NULL;
            _set(&tmp_0, TNUM(10));
            while (__bool(_leq(i, tmp_0))) {
                        TValue_t gc _tmp0 = T_NULL;
                        _move(&_tmp0, tostring(i));
                        _move(&s, _concat(__str_ct___1, _tmp0));
                        _move(&i, _add(i, TNUM8(1)));
                    }
        }
    TValue_t gc _tmp1 = T_NULL;
    _move(&_tmp1, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp1, TNUM(3));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    TValue_t gc _tmp2 = T_NULL;
    _move(&_tmp2, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp2, TNUM(2));
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(_inner, ((TVSlice_t){NULL, 0})));
    }
    TValue_t gc _tmp3 = T_NULL;
    _move(&_tmp3, CALL(__internal_debug_str_used, ((TVSlice_t){NULL, 0})));
    __internal_debug_assert_eq(_tmp3, TNUM(2));
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(2);
    _set(&__str_ct___0, TSTRi(_store_str_at_or_die(CONSTSTR("$"), 0)));
    _set(&__str_ct___1, TSTRi(_store_str_at_or_die(CONSTSTR("#"), 1)));

    return T_NULL;
}