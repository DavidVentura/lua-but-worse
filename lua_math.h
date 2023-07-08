#include "lua.h"
TValue_t _cos(TValue_t angle);
TValue_t _sin(TValue_t angle);
TValue_t _atan2(TValue_t dx, TValue_t dy);

TValue_t _abs(TValue_t num);
TValue_t  sgn(TVSlice_t varargs);
TValue_t _min(TVSlice_t varargs);
TValue_t _max(TVSlice_t varargs);
TValue_t  mid(TValue_t a, TValue_t b, TValue_t c);

TValue_t shr(TValue_t num, TValue_t bits);
TValue_t shl(TValue_t num, TValue_t bits);
TValue_t rnd(TVSlice_t varargs);
