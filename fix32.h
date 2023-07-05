#ifndef FIX32_H
#define FIX32_H
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct __attribute__((__packed__))fix32_s {
	int16_t i;
	uint16_t f;
} fix32_t;


static const fix32_t __ZERO = (fix32_t){.i=0, .f=0};
// grep -P '^\w+.*{$' fix32.c | sed 's/\s\+{/;/'

int32_t fix32_to_int32(fix32_t f);
fix32_t fix32_from_int16(int16_t i);
fix32_t fix32_from_uint16(uint16_t i);
fix32_t fix32_from_int8(int8_t i);
void fix32_inc(fix32_t* f);
int32_t fix32_to_bits(fix32_t f);
fix32_t fix32_from_bits(int32_t b);
fix32_t fix32_invert_sign(fix32_t a);
fix32_t fix32_from_float(float f);
float fix32_to_float(fix32_t a);
fix32_t fix32_pow(fix32_t a, fix32_t b);
fix32_t fix32_sub(fix32_t a, fix32_t b);
fix32_t fix32_ceil(fix32_t a);
fix32_t fix32_add(fix32_t a, fix32_t b);
fix32_t fix32_mul(fix32_t a, fix32_t b);
fix32_t fix32_div(fix32_t a, fix32_t b);
bool fix32_equals(fix32_t a, fix32_t b);
fix32_t fix32_flr(fix32_t a);
void fix32_pluseq(fix32_t* a, fix32_t b);
void fix32_muleq(fix32_t* a, fix32_t b);
void fix32_minuseq(fix32_t* a, fix32_t b);
bool fix32_geq(fix32_t a, fix32_t b);
bool fix32_gt(fix32_t a, fix32_t b);
bool fix32_lt(fix32_t a, fix32_t b);
bool fix32_leq(fix32_t a, fix32_t b);
fix32_t fix32_abs(fix32_t v);
fix32_t fix32_mod(fix32_t a, fix32_t b);
fix32_t fix32_sqrt(fix32_t v);
fix32_t fix32_cos(fix32_t x);
fix32_t fix32_sin(fix32_t n);
void print_fix32(fix32_t num, char* buf);
#endif
