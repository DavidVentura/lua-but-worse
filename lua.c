#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fix32.h"

enum typetag_t {NUL=0, STR=1, TAB=2, FUN=3, NUM=4, BOOL=5};

typedef struct TValue_s TValue_t;
typedef struct Table_s Table_t;

struct TValue_s {
	enum typetag_t tag; // 3 bits used only
	union {
		char* str;
		fix32_t num;
		void* fun;
		Table_t* table;
	};
};

typedef struct KV_s {
	TValue_t key;
	TValue_t value;
} KV_t;

struct Table_s {
	Table_t* metatable;
	KV_t* kvs;
	uint16_t len;
};

Table_t* ENV;
#define TNUM8(x)  ((TValue_t){.tag = NUM,  .num = (fix32_from_int8(x))})
#define TNUM(x)   ((TValue_t){.tag = NUM,  .num = (x)})
#define TSTR(x)   ((TValue_t){.tag = STR,  .str = (x)})
#define TBOOL(x)  ((TValue_t){.tag = BOOL, .num = (fix32_from_int8(x))})

/*
 * Multiplying by 100k gives accurate measurements down to 0x0001,
 * though it requires spilling to 64bit values. Will revisit
 * if it's a performance concern
 *
 * >>> hex((0xFFFF * 100_000))
 * '0x1869e7960'
 * >>> hex((0xFFFF * 10_000))
 * '0x270fd8f0'
 */
#define FIX32_DEC_AS_INT(x) (((uint32_t)x) * 100000) >> 16

TValue_t T_NULL = {.tag = NUL};
const fix32_t _zero = (fix32_t){.i=0, .f=0};
const fix32_t _one  = (fix32_t){.i=1, .f=0};
TValue_t T_TRUE =  {.tag = BOOL, .num = _one};
TValue_t T_FALSE = {.tag = BOOL, .num = _zero};

void print(TValue_t v) {
	switch(v.tag) {
		case NUM:
			if(v.num.f == 0) {
				printf("%d\n", v.num.i);
			} else {
				uint32_t dec_part = FIX32_DEC_AS_INT(v.num.f);
				uint8_t leading_zeroes = 5;
				while (dec_part % 10 == 0) {
					dec_part /= 10;
					leading_zeroes--;
				}
				printf("%d.%0*d\n", v.num.i, leading_zeroes, dec_part);
			}
			break;
		default:
			printf("idk how to print with tag %d\n", v.tag);
			break;
	}
}

bool equal(TValue_t a, TValue_t b) {
	if(a.tag != b.tag) return false;
	switch(a.tag) {
		case NUM:
			return fix32_equals(a.num, b.num);
		case STR:
			return strncmp(a.str, b.str, UINT8_MAX) == 0;
		default:
			printf("IDK how to compare type %d\n", a.tag);
			return false;
	}
}


void grow_table(Table_t* t) {
	uint16_t new_len = t->len * 2;
	new_len = new_len == 0 ? 1 : new_len;
	t->kvs = realloc(t->kvs, new_len * sizeof(KV_t));
	t->len = new_len;
}

void set_tabvalue(Table_t* u, const TValue_t* key, TValue_t v) {
	uint16_t first_null = UINT16_MAX;
	for(uint16_t i=0; i<u->len; i++) {
		if (equal(u->kvs[i].key, *key)) {
			u->kvs[i].value = v;
			return;
		}
		if(u->kvs[i].key.tag == NUL && first_null == UINT16_MAX) {
			first_null = i;
		}
	}
	if (first_null < UINT16_MAX) {
		u->kvs[first_null].key = *key;
		u->kvs[first_null].value = v;
		return;
	}
	if (first_null == UINT16_MAX) {
		// did not find a matching key nor any NULs
		grow_table(u);
		return set_tabvalue(u, key, v);
	}
}

TValue_t get_tabvalue(Table_t* u, const TValue_t* key) {
	for(uint16_t i=0; i<u->len; i++) {
		if (equal(u->kvs[i].key, *key)) {
			return u->kvs[i].value;
		}
	}
	return T_NULL;
}

TValue_t _mult(TValue_t a, TValue_t b) {
	return TNUM(fix32_mul(a.num, b.num));
}

TValue_t _add(TValue_t a, TValue_t b) {
	return TNUM(fix32_add(a.num, b.num));
}

void _pluseq(TValue_t* a, TValue_t b) {
	fix32_pluseq(&a->num, b.num); // TODO assert
}

bool _lt(TValue_t a, TValue_t b) {
	return fix32_lt(a.num, b.num); // TODO assert
}

TValue_t _invert_sign(TValue_t a) {
	return TNUM(fix32_invert_sign(a.num)); // TODO assert
}

#define _bool(x) _Generic((x), TValue_t: __bool, bool: __mbool)(x)

bool __mbool(bool b) {
	return b;
}
bool __bool(TValue_t a) {
	if(a.tag == NUM)
		return a.num.i != 0 || a.num.f != 0;
	if(a.tag == STR)
		return a.str != NULL;
	if(a.tag == NUL)
		return false;
	if(a.tag == BOOL)
		return a.num.i != 0;
	return false;
}

Table_t* make_table(uint16_t size) {
	KV_t* kvs = calloc(sizeof(KV_t), size);
	Table_t* ret = malloc(sizeof(Table_t));
	ret->kvs = kvs;
	ret->len = size;
	ret->metatable = NULL;
	return ret;
}
