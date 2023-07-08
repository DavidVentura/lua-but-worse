#include "fix32.h"
#include "lua.h"
#include "assert.h"
#include <stdlib.h>

TValue_t _cos(TValue_t angle){
	assert(angle.tag == NUM);
	return TNUM(fix32_cos(angle.num));
}

TValue_t _sin(TValue_t angle){
	assert(angle.tag == NUM);
	return TNUM(fix32_sin(angle.num));
}
TValue_t _atan2(TValue_t dx, TValue_t dy){
	assert(false);
}

TValue_t _abs(TValue_t num){
	assert(false);
}
TValue_t  sgn(TVSlice_t varargs){
	assert(false);
}
TValue_t _min(TVSlice_t varargs){
	assert(false);
}
TValue_t _max(TVSlice_t varargs){
	assert(false);
}
TValue_t  mid(TValue_t a, TValue_t b, TValue_t c){
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	assert(c.tag == NUM);
	assert(false);
}

TValue_t shr(TValue_t num, TValue_t bits){
	assert(false);
	//return TNUM(fix32_shr(num.num, bits.num));
}
TValue_t shl(TValue_t num, TValue_t bits){
	assert(false);
	//return TNUM(fix32_shl(num.num, bits.num));
}

TValue_t rnd(TVSlice_t varargs){
	//_Static_assert(RAND_MAX >= UINT16_MAX, "Rand is not big enough to use trivially");
	assert(varargs.num == 1);
	//TODO .tag == NUM); // TODO table
	return TNUM(fix32_from_bits(rand() % fix32_to_bits(varargs.elems[0].num)));
}

