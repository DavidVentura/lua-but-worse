#include "lua.c"

#define stdlib(x) TValue_t x(uint8_t c, TValue_t*);

stdlib(btn);
stdlib(map);
stdlib(spr);
stdlib(cls);
