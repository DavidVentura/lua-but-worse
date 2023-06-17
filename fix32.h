#include <stdint.h>
#include <stdbool.h>

typedef struct fix32_s {
	int16_t i;
	uint16_t f;
} fix32_t;


int32_t fix32_to_int32(fix32_t f) {
	return f.i;
}

fix32_t fix32_from_int16(int16_t i) {
	return (fix32_t){.i = i, .f = 0};
}

fix32_t fix32_from_int8(int8_t i) {
	return (fix32_t){.i = i, .f = 0};
}

void fix32_inc(fix32_t* f) {
	f->i++;
}

uint32_t fix32_to_bits(fix32_t f) {
	return (f.i << 16) | (f.f);
}

fix32_t fix32_from_bits(uint32_t b) {
	return (fix32_t){.i = (int16_t)((b & 0xffff0000) >> 16), .f = (uint16_t)(b & 0xffff)};
}

fix32_t fix32_from_float(float f) {
	return fix32_from_bits((int32_t)((int64_t)(f * 65536.0f)));
}

fix32_t fix32_add(fix32_t a, fix32_t b) {
	uint32_t _a = fix32_to_bits(a);
	uint32_t _b = fix32_to_bits(b);
	return fix32_from_bits(_a + _b);
}

fix32_t fix32_mul(fix32_t a, fix32_t b) {
	uint32_t _a = fix32_to_bits(a);
	uint32_t _b = fix32_to_bits(b);
	uint32_t res = ((uint64_t)_a * _b) >> 16;
	return fix32_from_bits(res);
}

fix32_t fix32_div(fix32_t a, fix32_t b) {
	uint32_t _a = fix32_to_bits(a);
	uint32_t _b = fix32_to_bits(b);

	// This special case ensures 0x8000/0x1 = 0x8000, not 0x8000.0001
	if (_b == 0x10000) return a;

	if (_b) {
		int64_t result = ((int64_t)_a) * 0x10000 / _b;
		int64_t pos_result = result & 0x7fffffffu; // drop sign bit
		if (pos_result <= 0x7fffffffu)
			return fix32_from_bits(((int32_t)result));
	}
	// Return 0x8000.0001 (not 0x8000.0000) for -Inf, just like PICO-8
	return fix32_from_bits((_a ^ _b) >= 0 ? 0x7fffffffu : 0x80000001u);
}


bool fix32_equals(fix32_t a, fix32_t b) {
	return a.i == b.i && a.f == b.f;
}

fix32_t fix32_flr(fix32_t a) {
	return (fix32_t){.i = a.i, .f = 0};
}

fix32_t fix32_invert_sign(fix32_t a) {
	return (fix32_t){.i = (int16_t)-a.i, .f = a.f};
}
void fix32_pluseq(fix32_t* a, fix32_t b) {
	*a = fix32_add(*a, b);
}

bool fix32_lt(fix32_t a, fix32_t b) {
	if( a.i > b.i ) return false;
	if( a.i == b.i && a.f >= b.f ) return false;
	return true;
}

fix32_t fix32_sqrt(fix32_t v) {
    // from fpsqrt.c, sqrt_fx16_16_to_fx16_16
    uint32_t t, q, b, r;
    r = fix32_to_bits(v);
    b = 0x40000000;
    q = 0;
    while( b > 0x40 ) {
        t = q + b;
        if( r >= t ) {
            r -= t;
            q = t + b; // equivalent to q += 2*b
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return fix32_from_bits(q);
}
