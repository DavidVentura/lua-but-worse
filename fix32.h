#ifndef FIX32
#define FIX32
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct __attribute__((__packed__))fix32_s {
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

int32_t fix32_to_bits(fix32_t f) {
	return ((int32_t)f.i << 16) | (f.f);
}

fix32_t fix32_from_bits(int32_t b) {
	// TODO: can i just assign the 32 bits directly
	return (fix32_t){.i = (int16_t)((b & 0xffff0000) >> 16), .f = (uint16_t)(b & 0xffff)};
}

fix32_t fix32_invert_sign(fix32_t a) {
	if (a.f == 0) return (fix32_t){.i = (int16_t)(-a.i), .f = 0};
	return (fix32_t){.i = (int16_t)(-a.i-1), .f = (uint16_t)(0xFFFF - a.f + 1)};
}

fix32_t fix32_from_float(float f) {
	return fix32_from_bits((int32_t)((int64_t)(f * 65536.0f)));
	if(f >= 0.0f) return fix32_from_bits((int32_t)((int64_t)(f * 65536.0f)));
	return fix32_invert_sign(fix32_from_bits((int32_t)((int64_t)(f * 65536.0f))));
}

fix32_t fix32_sub(fix32_t a, fix32_t b) {
	int32_t _a = fix32_to_bits(a);
	int32_t _b = fix32_to_bits(b);
	return fix32_from_bits(_a - _b);
}

fix32_t fix32_add(fix32_t a, fix32_t b) {
	int32_t _a = fix32_to_bits(a);
	int32_t _b = fix32_to_bits(b);
	return fix32_from_bits(_a + _b);
}

fix32_t fix32_mul(fix32_t a, fix32_t b) {
	int32_t _a = fix32_to_bits(a);
	int32_t _b = fix32_to_bits(b);
	int32_t res = ((int64_t)_a * _b) >> 16;
	return fix32_from_bits(res);
}

fix32_t fix32_div(fix32_t a, fix32_t b) {
	int32_t _a = fix32_to_bits(a);
	int32_t _b = fix32_to_bits(b);

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

void fix32_pluseq(fix32_t* a, fix32_t b) {
	*a = fix32_add(*a, b);
}

bool fix32_gt(fix32_t a, fix32_t b) {
	if( a.i < b.i ) return false;
	if( a.i == b.i && a.f <= b.f ) return false;
	return true;
}

bool fix32_lt(fix32_t a, fix32_t b) {
	if( a.i > b.i ) return false;
	if( a.i == b.i && a.f >= b.f ) return false;
	return true;
}

fix32_t fix32_abs(fix32_t v) {
	if (v.i >= 0) return v;
	return fix32_invert_sign(v);
}

fix32_t fix32_sqrt(fix32_t v) {
    // from fpsqrt.c, sqrt_fx16_16_to_fx16_16
    uint32_t t, q, b, r;
    r = (uint32_t)fix32_to_bits(v); // can't sqrt on negative
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


fix32_t fix32_cos(fix32_t x) {
	// A direct translation of https://stackoverflow.com/a/28050328
	// but without dividing by 2PI
    const fix32_t zero_two_two_five = fix32_from_float(0.225f);
    const fix32_t zero_two_five = fix32_from_float(0.25f);
    const fix32_t zero_five = fix32_from_float(0.5f);
    const fix32_t one = fix32_from_float(1.f);
    const fix32_t sixteen = fix32_from_float(16.f);

    // x -= (.25f) + floor(x + (.25f));
    x = fix32_sub(x, fix32_add(zero_two_five, fix32_flr(fix32_add(x, zero_two_five))));

    // x *= (16.f) * (fabsf(x) - (.5f));
	x = fix32_mul(x, fix32_mul(sixteen, fix32_sub(fix32_abs(x), zero_five)));

	// x += (.225f) * x * (fabsf(x) - (1.f));
    x = fix32_add(x, fix32_mul(zero_two_two_five, fix32_mul(x, fix32_sub(fix32_abs(x), one))));
    return x;
}


fix32_t fix32_sin(fix32_t n) {
	// In Pico-8, a full circle is represented in the range 0..1
	// https://pico-8.fandom.com/wiki/Sin

	n = fix32_invert_sign(n); // pico8 inverts the sign
	return fix32_cos(fix32_sub(n, (fix32_t){.i=0, .f=0x4000})); // sub 0.25 (1/4 of a turn)
}

/*
 * Multiplying by 100k gives accurate representation down to 0x0001,
 * though it requires spilling to 64bit values. Will revisit
 * if it's a performance concern
 *
 * >>> hex((0xFFFF * 100_000))
 * '0x1869e7960'
 * >>> hex((0xFFFF * 10_000))
 * '0x270fd8f0'
 */
#define FIX32_DEC_AS_INT(x) (((uint64_t)x) * 100000) >> 16
// TODO: no sprintf
void print_fix32(fix32_t num, char* buf) {
	if(num.f == 0) {
		sprintf(buf, "%d", num.i);
		return;
	}

	if(num.i < 0) {
		buf[0] = '-';
		print_fix32((fix32_t){.i=(int16_t)(-num.i-1), .f=(uint16_t)(0xFFFF-num.f+1)}, buf+1);
		return;
	}

	uint32_t dec_part = FIX32_DEC_AS_INT(num.f);
	uint8_t leading_zeroes = 5;
	while (dec_part % 10 == 0) {
		dec_part /= 10;
		leading_zeroes--;
	}
	sprintf(buf, "%d.%0*d", num.i, leading_zeroes, dec_part);
}
#endif
