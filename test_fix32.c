#include "fix32.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define FIX32_DEC_AS_INT(x) (((uint64_t)x) * 100000) >> 16

const fix32_t MINUS_TWO 	= (fix32_t){.i=-2, .f=0};
const fix32_t MINUS_ONE 	= (fix32_t){.i=-1, .f=0};

const fix32_t ONE_EIGHTH 		= (fix32_t){.i=0, .f=0x2000};
const fix32_t TWO_EIGHTHS 		= (fix32_t){.i=0, .f=0x4000};
const fix32_t HALF 				= (fix32_t){.i=0, .f=0x8000};
const fix32_t THREE_QUARTERS	= (fix32_t){.i=0, .f=0xC000};

const fix32_t ZERO 	= (fix32_t){.i=0, .f=0};
const fix32_t ONE 	= (fix32_t){.i=1, .f=0};
const fix32_t TWO 	= (fix32_t){.i=2, .f=0};
const fix32_t THREE = (fix32_t){.i=3, .f=0};
const fix32_t FOUR 	= (fix32_t){.i=4, .f=0};
const fix32_t FIVE 	= (fix32_t){.i=5, .f=0};


void assert_very_close_fp(float got, float expected) {
	float delta = got - expected;
	if(delta > 0.01f) {
		printf("Expected ~%f but got %f (delta %f)\n", got, expected, delta);
		fflush(stdout);
		assert(false);
	}
}
void assert_very_close(fix32_t got, fix32_t expected) {
	fix32_t epsilon = (fix32_t){.i=0, .f=1};
	fix32_t delta = fix32_abs(fix32_sub(got, expected));
	if (fix32_gt(delta, epsilon)) {
		char num_a[10] = {0};
		char num_b[10] = {0};
		char num_d[10] = {0};
		print_fix32(got, num_a);
		print_fix32(expected, num_b);
		print_fix32(delta, num_d);
		printf("Expected ~%s but got %s (delta %s)\n", num_b, num_a, num_d);
		fflush(stdout);
		assert(fix32_equals(got, expected));
	}
}

void assert_eq(fix32_t got, fix32_t expected) {
	if (!fix32_equals(got, expected)) {
		char num_a[10] = {0};
		char num_b[10] = {0};
		print_fix32(got, num_a);
		print_fix32(expected, num_b);
		printf("Expected %s but got %s\n", num_b, num_a);
		fflush(stdout);
		assert(fix32_equals(got, expected));
	}
}

void test_invert_sign() {
	assert_eq(fix32_invert_sign(fix32_from_float(0.25f)), fix32_from_float(-0.25f));
	assert_eq(fix32_invert_sign(MINUS_ONE), ONE);
	assert_eq(fix32_invert_sign(ONE), MINUS_ONE);
	assert_eq(fix32_invert_sign(fix32_from_float(1.5f)), (fix32_t){.i=-2, .f=0x8000});
	printf("invert sign ok\n");
}

void test_abs() {
	assert_eq(fix32_abs(fix32_sub(ZERO, TWO_EIGHTHS)), fix32_from_float(0.25f));
	printf("ABS ok\n");
}
void test_sub() {
	assert_eq(fix32_sub(ZERO, ONE), MINUS_ONE);
	assert_eq(fix32_sub(ZERO, TWO_EIGHTHS), fix32_from_float(-0.25f));
	printf("SUB ok\n");
}
void test_mul() {
	assert_eq(fix32_mul(TWO, TWO), FOUR);
	assert_eq(fix32_mul((fix32_t){.i=2, .f=0x8000}, TWO), FIVE);
	assert_eq(fix32_mul(TWO, MINUS_ONE), MINUS_TWO);

	assert_eq(fix32_mul(MINUS_ONE, MINUS_ONE), ONE);

	assert_eq(fix32_mul(fix32_from_float(-4.f), MINUS_ONE), FOUR);
	assert_eq(fix32_mul(MINUS_ONE, fix32_from_float(-4.f)), FOUR);

	assert_eq(fix32_mul(fix32_invert_sign(TWO_EIGHTHS), fix32_from_float(-4.f)), ONE);
	printf("MUL ok\n");
}
void test_sqrt() {
	assert_eq(fix32_sqrt((fix32_t){.i=4, .f=0}), TWO);
	assert_eq(fix32_sqrt((fix32_t){.i=9, .f=0}), THREE);
	assert_eq(fix32_sqrt((fix32_t){.i=16, .f=0}), FOUR);
	printf("SQRT ok\n");
}

void test_cos_matches_fpcos() {
    int32_t max = 0, min = 0;
    for(uint16_t i = 0; i < UINT16_MAX; ++i) {
        int32_t fp_cos = lround(4096*cosf(2*M_PI * i / UINT16_MAX));
        fix32_t _fixed_cos = fix32_cos(fix32_from_float(-1.f*i / UINT16_MAX));
		int16_t fixed_cos = fix32_to_int32(fix32_mul((fix32_t){.i=4096, .f=0}, _fixed_cos));
        int32_t err = fp_cos - fixed_cos;
        if(err > max)
            max = err;
        if(err < min)
            min = err;
    }
	// output is scaled up to (-4096..4096)
	// an error of 7/8192 is 0.085%
	assert(min > -6);
	assert(max < 7);
	printf("COS-FPCOS ok\n");
}

void test_cos() {
	assert_eq(fix32_cos(ZERO 	),  		ONE);
	assert_very_close(fix32_cos(ONE_EIGHTH 	),  	(fix32_t){.i=0, .f=0xb532});
	assert_eq(fix32_cos(TWO_EIGHTHS	), 		ZERO);
	assert_eq(fix32_cos(HALF 		), 		MINUS_ONE);
	assert_eq(fix32_cos(THREE_QUARTERS), 	ZERO);
	assert_eq(fix32_cos(ONE), 				ONE);
	printf("COS ok\n");
}
void test_mod() {
	assert_eq(fix32_mod(ZERO, ZERO), ZERO);
	assert_eq(fix32_mod(ZERO, ONE), ZERO);
	assert_eq(fix32_mod(ZERO, TWO), ZERO);

	assert_eq(fix32_mod(ONE, ZERO), ONE);
	assert_eq(fix32_mod(TWO, ZERO), TWO);

	assert_eq(fix32_mod(TWO, ONE), ZERO);

	assert_eq(fix32_mod(ONE, THREE_QUARTERS), TWO_EIGHTHS);

	printf("MOD ok\n");
}

void test_sin() {
	assert_eq(fix32_sin(ZERO 	),  	ZERO);
	assert_very_close(fix32_sin(ONE_EIGHTH 	),   fix32_invert_sign((fix32_t){.i=0, .f=0xb534}));
	assert_eq(fix32_sin(TWO_EIGHTHS	), 	MINUS_ONE);
	assert_eq(fix32_sin(HALF 		), 	ZERO);
	assert_eq(fix32_sin(THREE_QUARTERS), ONE);
	assert_eq(fix32_sin(ONE), 			ZERO);
	printf("SIN ok\n");
}

void test_cmp() {
	assert(fix32_lt(ZERO, ONE));
	assert(!fix32_lt(ONE, ZERO));
	assert(!fix32_lt(ONE, ONE));

	assert(!fix32_gt(ZERO, ONE));
	assert(fix32_gt(ONE, ZERO));
	assert(!fix32_gt(ONE, ONE));

	assert(fix32_leq(ZERO, ONE));
	assert(!fix32_leq(ONE, ZERO));
	assert(fix32_leq(ONE, ONE));

	assert(!fix32_geq(ZERO, ONE));
	assert(fix32_geq(ONE, ZERO));
	assert(fix32_geq(ONE, ONE));
	printf("CMP ok\n");
}

void test_float() {
	assert_very_close_fp(5.0f, 5.0f);
	assert_very_close_fp(5.0f, fix32_to_float(fix32_from_float(5.0f)));
	assert_very_close_fp(5.1f, fix32_to_float(fix32_from_float(5.1f)));
	assert_very_close_fp(5.5f, fix32_to_float(fix32_from_float(5.5f)));
	assert_very_close_fp(-2.0f, fix32_to_float(fix32_from_float(-2.0f)));
	printf("FLOAT ok\n");
}

int main() {
	test_invert_sign();
	test_cmp();
	test_mod();
	test_abs();
	test_sub();
	test_mul();
	test_sqrt();
	test_cos_matches_fpcos();
	test_cos();
	test_sin();
	test_float();
}
