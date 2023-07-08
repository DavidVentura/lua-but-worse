#include "lua.h"
TValue_t count(TVSlice_t varargs);

TValue_t add(TValue_t tab, TValue_t v);
TValue_t del(TValue_t tab, TValue_t v);
TValue_t* all(TValue_t tab);
KV_t* pairs(TValue_t t);
KV_t* ipairs(TValue_t t);

#define foreach(x, y)  _Generic(y, TValue_t: _foreach_tvalue, Func_t: _foreach)(x, y)
void _foreach(TValue_t t, Func_t f);
void _foreach_tvalue(TValue_t t, TValue_t f);
