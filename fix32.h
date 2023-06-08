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

bool fix32_equals(fix32_t a, fix32_t b) {
	return a.i == b.i && a.f == b.f;
}

fix32_t fix32_flr(fix32_t a) {
	return (fix32_t){.i = a.i, .f = 0};
}
void fix32_pluseq(fix32_t* a, fix32_t b) {
	*a = fix32_add(*a, b);
}

bool fix32_lt(fix32_t a, fix32_t b) {
	if( a.i > b.i ) return false;
	if( a.i == b.i && a.f >= b.f ) return false;
	return true;
}
