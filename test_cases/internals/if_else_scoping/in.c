#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t __str_ct_____0;

TValue_t test_if_else_same_name(TVSlice_t args);
TValue_t test_if_else_outer_access(TVSlice_t args);
TValue_t test_nested_if(TVSlice_t args);
TValue_t test_elseif_scoping(TVSlice_t args);
TValue_t test_while_scoping(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t test_if_else_same_name(TVSlice_t args) {
    TValue_t gc outer = T_NULL;
    _set(&outer, TNUM(1));
    if (__bool(T_TRUE)) {
            TValue_t gc x = T_NULL;
            _set(&x, TNUM(10));
            printh(x);
        } else {
            TValue_t gc x = T_NULL;
            _set(&x, TNUM(20));
            printh(x);
        }
    return T_NULL;
}

TValue_t test_if_else_outer_access(TVSlice_t args) {
    TValue_t gc a = T_NULL;
    _set(&a, TNUM(100));
    if (__bool(T_TRUE)) {
            TValue_t gc a = T_NULL;
            _set(&a, TNUM(200));
            printh(a);
        } else {
            TValue_t gc a = T_NULL;
            _set(&a, TNUM(300));
            printh(a);
        }
    printh(a);
    return T_NULL;
}

TValue_t test_nested_if(TVSlice_t args) {
    TValue_t gc val = T_NULL;
    _set(&val, TNUM(1));
    if (__bool(T_TRUE)) {
            TValue_t gc val = T_NULL;
            _set(&val, TNUM(2));
            printh(val);
            if (__bool(T_TRUE)) {
                        TValue_t gc val = T_NULL;
                        _set(&val, TNUM(3));
                        printh(val);
                    }
            printh(val);
        }
    printh(val);
    return T_NULL;
}

TValue_t test_elseif_scoping(TVSlice_t args) {
    TValue_t gc x = T_NULL;
    _set(&x, TNUM(0));
    if (__bool(T_FALSE)) {
            TValue_t gc x = T_NULL;
            _set(&x, TNUM(1));
            printh(x);
        } else {
            if (__bool(T_FALSE)) {
                        TValue_t gc x = T_NULL;
                        _set(&x, TNUM(2));
                        printh(x);
                    } else {
                        if (__bool(T_TRUE)) {
                                        TValue_t gc x = T_NULL;
                                        _set(&x, TNUM(3));
                                        printh(x);
                                    } else {
                                        TValue_t gc x = T_NULL;
                                        _set(&x, TNUM(4));
                                        printh(x);
                                    }
                    }
        }
    printh(x);
    return T_NULL;
}

TValue_t test_while_scoping(TVSlice_t args) {
    TValue_t gc i = T_NULL;
    _set(&i, TNUM(0));
    while (__bool(_lt(i, TNUM(3)))) {
            TValue_t gc x = T_NULL;
            _move(&x, _mult(i, TNUM(10)));
            printh(x);
            _move(&i, _add(i, TNUM(1)));
        }
    printh(i);
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_if_else_same_name, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_if_else_outer_access, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_nested_if, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_elseif_scoping, ((TVSlice_t){NULL, 0})));
    }
    printh(__str_ct_____0);
    {
        TValue_t gc _tmp = T_NULL;
        _move(&_tmp, CALL(test_while_scoping, ((TVSlice_t){NULL, 0})));
    }
    _return(TNUM(0));
}

TValue_t _lua_main(TVSlice_t args) {
    _grow_strings_to(1);
    _set(&__str_ct_____0, TSTRi(_store_str_at_or_die(CONSTSTR("---"), 0)));

    return T_NULL;
}

