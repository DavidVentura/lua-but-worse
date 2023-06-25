#include "assert.h"
#include "lua.c"

#define stdlib(x) TValue_t x(uint8_t argc, TValue_t* argv)

stdlib(btn);
stdlib(map);
stdlib(spr);
stdlib(cls);

stdlib(btn) {
	assert(argc==1);
	TValue_t b = argv[0];
	return TNUM(1);
}
stdlib(spr) {
}
stdlib(map) {
}
stdlib(cls) {
}
