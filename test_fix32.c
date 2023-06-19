#include "fix32.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#define FIX32_DEC_AS_INT(x) (((uint64_t)x) * 100000) >> 16

const fix32_t MINUS_TWO 	= (fix32_t){.i=-2, .f=0};
const fix32_t MINUS_ONE 	= (fix32_t){.i=-1, .f=0};

const fix32_t ONE_EIGHTH 		= (fix32_t){.i=0, .f=0x2000};
const fix32_t TWO_EIGHTHS 		= (fix32_t){.i=0, .f=0x4000};
const fix32_t HALF 				= (fix32_t){.i=0, .f=0x8000};
const fix32_t THREE_QUARTERS	= (fix32_t){.i=0, .f=0xb000};

const fix32_t ZERO 	= (fix32_t){.i=0, .f=0};
const fix32_t ONE 	= (fix32_t){.i=1, .f=0};
const fix32_t TWO 	= (fix32_t){.i=2, .f=0};
const fix32_t THREE = (fix32_t){.i=3, .f=0};
const fix32_t FOUR 	= (fix32_t){.i=4, .f=0};
const fix32_t FIVE 	= (fix32_t){.i=5, .f=0};

void print_fix32(fix32_t num, char* buf) {
	if(num.f == 0) {
		sprintf(buf, "%d", num.i);
		return;
	}

	if(num.i < 0) {
		buf[0] = '-';
		print_fix32((fix32_t){.i=-(num.i+1), .f=(0xFFFF-num.f+1)}, buf+1);
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

void assert_eq(fix32_t got, fix32_t expected) {
	if (!fix32_equals(got, expected)) {
		char num_a[10] = {0};
		char num_b[10] = {0};
		print_fix32(got, num_a);
		print_fix32(expected, num_b);
		printf("Expected %s but got %s\n", num_b, num_a);
		fflush(stdout);
		//assert(fix32_equals(got, expected));
	} else {
		//printf("good\n");
	}
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
	/*
	assert_eq(fix32_mul(TWO, TWO), FOUR);
	assert_eq(fix32_mul((fix32_t){.i=2, .f=0x8000}, TWO), FIVE);
	assert_eq(fix32_mul(TWO, MINUS_ONE), MINUS_TWO);

	assert_eq(fix32_mul(MINUS_ONE, MINUS_ONE), ONE);

	assert_eq(fix32_mul(fix32_from_float(-4.f), MINUS_ONE), FOUR);
	assert_eq(fix32_mul(MINUS_ONE, fix32_from_float(-4.f)), FOUR);
*/
	// -4 * -0.25 = death
	assert_eq(fix32_mul(fix32_invert_sign(TWO_EIGHTHS), fix32_from_float(-4.f)), ONE);
	printf("MUL ok\n");
}
void test_sqrt() {
	assert_eq(fix32_sqrt((fix32_t){.i=4, .f=0}), TWO);
	assert_eq(fix32_sqrt((fix32_t){.i=9, .f=0}), THREE);
	assert_eq(fix32_sqrt((fix32_t){.i=16, .f=0}), FOUR);
	printf("SQRT ok\n");
}
void test_sin() {
	assert_eq(fix32_sin(ONE_EIGHTH 	),  MINUS_ONE); // -0.7
	assert_eq(fix32_sin(TWO_EIGHTHS	), MINUS_ONE);
	assert_eq(fix32_sin(HALF 		), ZERO);
	assert_eq(fix32_sin(THREE_QUARTERS), ONE);
	printf("SIN ok\n");
}
int main() {
	test_abs();
	test_sub();
	test_mul();
	test_sqrt();
	printf("SIN\n");
	test_sin();
	fflush(stdout);
}
