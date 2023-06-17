#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fix32.h"

enum typetag_t {NUL=0, STR=1, TAB=2, FUN=3, NUM=4, BOOL=5};

typedef struct TValue_s TValue_t;
typedef struct Table_s Table_t;
typedef TValue_t (*Func_t)(TValue_t*);

struct TValue_s {
	union {
		char* str;
		fix32_t num;
		Func_t fun;
		uint16_t table_idx;
	};
	enum typetag_t tag; // 3 bits used only
};

typedef struct KV_s {
	TValue_t key;
	TValue_t value;
} KV_t;

struct Table_s {
	KV_t* kvs;
	TValue_t __index;
	uint16_t metatable_idx;
	uint16_t len;
	uint16_t count;
};

Table_t* ENV;

typedef struct TArena_s {
	Table_t* tables;
	uint16_t len;
	uint16_t used;
} TArena_t;

TArena_t _tables = {.tables=NULL, .len=0, .used=0};

#define TNUM(x)        ((TValue_t){.tag = NUM,  .num = (x)})
#define TNUM8(x)       ((TValue_t){.tag = NUM,  .num = (fix32_from_int8(x))})
#define TNUM16(x)      ((TValue_t){.tag = NUM,  .num = (fix32_from_int16(x))})
#define TSTR(x)        ((TValue_t){.tag = STR,  .str = (x)})
#define TBOOL(x)       ((TValue_t){.tag = BOOL, .num = (fix32_from_int8(x))})
#define TFUN(x)        ((TValue_t){.tag = FUN,  .fun = (x)})
#define TTAB(x)        ((TValue_t){.tag = TAB,  .table_idx = x})
#define GETTAB(x)      (&_tables.tables[(x).table_idx])
#define GETMETATAB(x)  (_tables.tables[(x).metatable_idx])

#define set_tabvalue(x,y,z)	_Generic(z, TValue_t: _set_tabvalue)(x,y,z)
#define CALL(x, y)     		_Generic(x, TValue_t: __call, Func_t: __direct_call)(x)(y)
#define print(x)	   		_Generic(x, TValue_t: print_tvalue, char*: print_str, bool: print_bool)(x)
#define _bool(x) 			_Generic((x), TValue_t: __bool, bool: __mbool)(x)

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
#define FIX32_DEC_AS_INT(x) (((uint64_t)x) * 100000) >> 16

TValue_t T_NULL = {.tag = NUL};
const fix32_t _zero = (fix32_t){.i=0, .f=0};
const fix32_t _one  = (fix32_t){.i=1, .f=0};
TValue_t T_TRUE =  {.tag = BOOL, .num = _one};
TValue_t T_FALSE = {.tag = BOOL, .num = _zero};


Func_t __direct_call(Func_t f) {
	return f;
}

Func_t __call(TValue_t t) {
	return t.fun;
}
Func_t __call_ptr(TValue_t* t) {
	return t->fun;
}
void print_bool(bool b) {
	if(b) {
		printf("true\n");
	} else {
		printf("false\n");
	}
}
void print_str(char* c) {
	printf("%s\n", c);
}

void print_tvalue(TValue_t v) {
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
		case NUL:
			printf("nil\n");
			break;
		case STR:
			printf("%s\n", v.str);
			break;
		default:
			printf("idk how to print with tag %d\n", v.tag);
			break;
	}
}
void print_tvalue_ptr(TValue_t* v) {
	print_tvalue(*v);
}

bool equal(TValue_t a, TValue_t b) {
	if(a.tag != b.tag) return false;
	switch(a.tag) {
		case NUM:
			return fix32_equals(a.num, b.num);
		case STR:
			return strncmp(a.str, b.str, UINT8_MAX) == 0;
		case NUL:
			return true;
		case TAB:
			return a.table_idx == b.table_idx;
		default:
			printf("IDK how to compare type %d\n", a.tag);
			return false;
	}
}
bool _equal(TValue_t a, TValue_t b) {
	return equal(a, b);
}


void grow_table(uint16_t idx) {
	Table_t* t = &_tables.tables[idx];
	uint16_t new_len = t->len * 2;
	new_len = (t->len == 0) ? 2 : new_len;
	//t->kvs = realloc(t->kvs, new_len * sizeof(KV_t));
	// this sets key->tag to 0 (NUL) for all new spaces in KVs
	KV_t* new_kvs = calloc(sizeof(KV_t), new_len);
	if(t->len) {
		memcpy(new_kvs, t->kvs, sizeof(KV_t) * t->len);
	}
	free(t->kvs);
	t->kvs = new_kvs;
	t->len = new_len;
	// t->count does not change
}


void _set_tabvalue(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	Table_t* u = GETTAB(t);
	assert(u != NULL);
	assert(key.tag != NUL); // lua throws "table index is nil"
	uint16_t first_null = UINT16_MAX;
	bool is_index = equal(key, TSTR("__index"));

	for(uint16_t i=0; i<u->len; i++) {
		if (equal(u->kvs[i].key, key)) {
			u->kvs[i].value = v;
			if(is_index) {
				u->__index = u->kvs[i].value;
			}
			return;
		}
		if(u->kvs[i].key.tag == NUL && first_null == UINT16_MAX) {
			first_null = i;
		}
	}
	if (first_null < UINT16_MAX) {
		u->kvs[first_null].key = key;
		u->kvs[first_null].value = v;
		u->count++;
		if(is_index) {
			u->__index = u->kvs[first_null].value;
		}
		return;
	}
	if (first_null == UINT16_MAX) {
		// did not find a matching key nor any NULs
		grow_table(t.table_idx);
		// cannot fail
		// TODO: assign straight into old_len+1
		return _set_tabvalue(t, key, v);
	}
}
void _set_tabvalue_ptr(TValue_t t, TValue_t key, TValue_t* v) {
	_set_tabvalue(t, key, *v);
}

TValue_t get_tabvalue(TValue_t u, TValue_t key) {
	Table_t* t = GETTAB(u);
	for(uint16_t i=0; i<t->len; i++) {
		if (equal(t->kvs[i].key, key)) {
			return t->kvs[i].value;
		}
	}
	if(t->metatable_idx != UINT16_MAX) {
		TValue_t __index = GETMETATAB(*t).__index;
		switch(__index.tag) {
			case TAB:
				return get_tabvalue(TTAB(__index.table_idx), key);
			case FUN:
				return CALL(__index, ((TValue_t[]){key}));
			default:
				assert(false);
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

TValue_t _div(TValue_t a, TValue_t b) {
	return TNUM(fix32_div(a.num, b.num));
}

TValue_t _sqr(TValue_t a) {
	return TNUM(fix32_mul(a.num, a.num));
}

TValue_t _sqrt(TValue_t a) {
	return TNUM(fix32_sqrt(a.num));
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

uint16_t make_table(uint16_t size) {
	if(_tables.len == _tables.used) {
		uint16_t new_len = _tables.len == 0 ? 16 : _tables.len*2;
		if (_tables.tables == NULL) {
			_tables.tables = calloc(new_len, sizeof(Table_t));
		} else {
			_tables.tables = realloc(_tables.tables, new_len*sizeof(Table_t));
		}
		_tables.len = new_len;
	}

	KV_t* kvs = NULL;
	if (size > 0)
		kvs = calloc(sizeof(KV_t), size); // this sets key->tag to 0 (NUL)

	Table_t ret = {
		.kvs = kvs,
		.len = size,
		.metatable_idx = UINT16_MAX,
		.__index = T_NULL,
	};

	_tables.used++;
	_tables.tables[_tables.used] = ret;
	return _tables.used;
}

void free_tvalue(TValue_t tv) {
	if(tv.tag != TAB) return;
	if(tv.table_idx == UINT16_MAX) return;
	//assert(false); // need to dec refcount
}

TValue_t flr(TValue_t f) {
	return TNUM(fix32_flr(f.num));
}

TValue_t getmetatable(TValue_t t) {
	if(t.tag != TAB) return T_NULL;
	if(GETTAB(t)->metatable_idx == UINT16_MAX) return T_NULL;
	return TTAB(GETTAB(t)->metatable_idx);
}

void setmetatable(TValue_t t, TValue_t meta) {
	assert(t.tag == TAB);
	assert(meta.tag == TAB);
	GETTAB(t)->metatable_idx = meta.table_idx;
}

void iadd_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO not true, tables can override this
	TValue_t newval = _add(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

void idiv_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO not true, tables can override this
	TValue_t newval = _div(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

TValue_t count(TValue_t t) {
	assert(t.tag == TAB);
	return TNUM(GETTAB(t)->count);
}
