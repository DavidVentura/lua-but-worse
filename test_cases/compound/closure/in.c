#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "stdlib.h"

TValue_t a;

TValue_t _anon_0(TVSlice_t args);
TValue_t b_fn(TVSlice_t args);
TValue_t main(TVSlice_t args);
TValue_t _lua_main(TVSlice_t args);

TValue_t _anon_0(TVSlice_t args) {
    TValue_t x = (args.num > 0) ? args.elems[0] : T_NULL;

    // Extract closure context
    TValue_t _closure_func = (args.num > 1) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* captured = &_captured.captured[_func->captured_indices[0]].value;

    return _mult(x, *captured);
}

TValue_t b_fn(TVSlice_t args) {
    // Extract closure context
    TValue_t _closure_func = (args.num > 0) ? args.elems[args.num - 1] : T_NULL;
    TFunc_t* _func = GETTFUN(_closure_func);

    TValue_t* captured = &_captured.captured[_func->captured_indices[0]].value;

    _set(captured, TNUM(2));
    return T_NULL;
}

TValue_t main(TVSlice_t args) {
    uint16_t _cap_idx_captured = _alloc_captured(TNUM(7));
    TValue_t* captured = &_captured.captured[_cap_idx_captured].value;
    TValue_t _tmp0 = TCLOSURE(_anon_0, 1);
    set_closure_arg(_tmp0, 0, _cap_idx_captured);
    _set(&a, _tmp0);
    printh(CALL(a, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    _set(captured, TNUM(1));
    printh(CALL(a, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    TValue_t b = TCLOSURE(b_fn, 1);
    set_closure_arg(b, 0, _cap_idx_captured);
    {
        TValue_t gc _tmp;
        _set(&_tmp, CALL(b, ((TVSlice_t){NULL, 0})));
    }
    printh(CALL(a, ((TVSlice_t){(TValue_t[]){TNUM(5)}, 1})));
    return TNUM(0);
}

TValue_t _lua_main(TVSlice_t args) {
    return T_NULL;
}