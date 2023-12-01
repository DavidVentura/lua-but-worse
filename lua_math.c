#include "fix32.h"
#include "lua.h"
#include "math.h"
#include "lua_table.h"
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
	assert(dx.tag == NUM);
	assert(dy.tag == NUM);

	// TODO: do in fix32
	float a2f = atan2f(fix32_to_float(dx.num), fix32_to_float(dy.num));
	/*
	fix32_t TAU = fix32_from_float(6.283185307179586f);
	fix32_t a2 = fix32_from_float(a2f);
	fix32_t res = fix32_add(fix32_from_float(0.7500001f), fix32_div(a2, TAU));
	fix32_t one = fix32_from_parts(1, 0);
	if(fix32_geq(res, one)) {
		res = fix32_sub(res, one);
	}
	*/
	float TAU = 6.283185307179586f;
	float a = 0.75f + a2f / TAU;
	if(a>=1) a-=1;
	return TNUM(fix32_from_float(a));
}

TValue_t _abs(TValue_t num){
	assert(false);
}
TValue_t  sgn(TVSlice_t varargs){
	assert(false);
}
TValue_t _min(TVSlice_t varargs){
	fix32_t first  = __get_num(varargs, 0);
	fix32_t second = __opt_num(varargs, 1, fix32_from_parts(0, 0));
	if(fix32_lt(first, second)) return TNUM(first);
	return TNUM(second);
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
	assert(num.tag == NUM);
	assert(bits.tag == NUM);
	assert(bits.num.f == 0);
	assert(bits.num.i >= 0);
	uint32_t numbits = fix32_to_bits(num.num);

	return TNUM(fix32_from_bits(numbits >> bits.num.i));
}

TValue_t shl(TValue_t num, TValue_t bits){
	assert(num.tag == NUM);
	assert(bits.tag == NUM);
	assert(bits.num.f == 0);
	assert(bits.num.i >= 0);
	uint32_t numbits = fix32_to_bits(num.num);

	return TNUM(fix32_from_bits(numbits << bits.num.i));
}

TValue_t rnd(TVSlice_t varargs){
	_Static_assert(RAND_MAX >= UINT16_MAX, "Rand is not big enough to use trivially");
	if (varargs.num == 0) {
		return TNUM(fix32_from_bits(rand() % fix32_to_bits(fix32_from_bits(1))));
	}
	assert(varargs.num == 1);
	if (varargs.elems[0].tag == NUM) {
		return TNUM(fix32_from_bits(rand() % fix32_to_bits(varargs.elems[0].num)));
	}
	if (varargs.elems[0].tag == TAB) {
		int16_t seq_until = _sequential_until(varargs.elems[0]);
		if (seq_until == 0) return T_NULL;
		return get_tabvalue(varargs.elems[0], TNUM((rand() % seq_until)+1));
		// rand() returns [0, n), but we need [1, n], adding 1 to the original range we get [1, n+1) which is the same

	}
	return TNUM(0);
}

