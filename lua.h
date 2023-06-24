#ifndef LUA
#define LUA
#include <stdbool.h>
#include <stdint.h>
#include "fix32.h"

enum __attribute__((__packed__)) typetag_t {NUL=0, STR=1, TAB=2, FUN=3, NUM=4, BOOL=5};

typedef struct TValue_s TValue_t;
typedef TValue_t (*Func_t)(uint8_t c, TValue_t*);

struct TValue_s {
	// no size advantage on replacing the two pointers (str, num)
	// with u16 indexes -- on embedded targets, the pointers are 32 bits
	// same as the fix32_t type.
	union {
		uint16_t str_idx; // maybe high bit for short/long str?
		fix32_t num;
		Func_t fun;
		uint16_t table_idx;
	};
	enum typetag_t tag; // 3 bits used only
};

_Static_assert(sizeof(TValue_t) <= 64, "too big");
_Static_assert(sizeof(TValue_t) <= 16, "too big"); // 8 for pointer on 64bit, 8 for padding. could be 1 with packed attr
												   // 8 total size on 32bit
//_Static_assert(sizeof(TValue_t) == 8, "too big"); // 8 for pointer on 64bit

typedef struct KV_s {
	TValue_t key;
	TValue_t value;
} KV_t;

typedef struct Table_s {
	KV_t* kvs;
	TValue_t __index;
	uint16_t metatable_idx;
	uint16_t len;
	uint16_t count;
	uint8_t refcount;
} Table_t;
// 8 on pointer 				(4 on 32bit)
// 16 on TValue_t __idnex 		(8 on 32bit)
// 7 on count/refcount/len/metatable_idx
// 1 on padding
_Static_assert(sizeof(Table_t) <= 32, "too big");
// 20 on 32bit
// _Static_assert(sizeof(Table_t) <= 20, "too big");


typedef struct TArena_s {
	Table_t* tables;
	uint16_t len;
	uint16_t used;
} TArena_t;

typedef struct Str_s {
	uint8_t* data;
	uint16_t len;
	uint8_t refcount;
} Str_t;

typedef struct SArena_s {
	Str_t* strings;
	uint16_t len;
} SArena_t;

const Str_t STR__INDEX = {.data=(uint8_t*)"__index", .len=7};


#define TNUM(x)        ((TValue_t){.tag = NUM,  .num = (x)})
#define TNUM8(x)       ((TValue_t){.tag = NUM,  .num = (fix32_from_int8(x))})
#define TNUM16(x)      ((TValue_t){.tag = NUM,  .num = (fix32_from_int16(x))})
#define TSTR(x)        ((TValue_t){.tag = STR,  .str_idx = (make_str(x))})
#define TBOOL(x)       ((TValue_t){.tag = BOOL, .num = (fix32_from_int8(x))})
#define TFUN(x)        ((TValue_t){.tag = FUN,  .fun = (x)})
#define TTAB(x)        ((TValue_t){.tag = TAB,  .table_idx = x})
#define GETSTRP(x)     (&_strings.strings[(x).str_idx])
#define GETSTR(x)      (_strings.strings[(x).str_idx])
#define GETTAB(x)      (&_tables.tables[(x).table_idx])
#define GETMETATAB(x)  (_tables.tables[(x).metatable_idx])

#define CALL(x, y)     		_Generic(x, TValue_t: __call, Func_t: __direct_call)(x)(COUNT_OF(y), y)
#define print(x)	   		_Generic(x, TValue_t: print_tvalue, char*: print_str, bool: print_bool)(x)
#define _bool(x) 			_Generic((x), TValue_t: __bool, bool: __mbool)(x)

const TValue_t T_NULL = {.tag = NUL};
const fix32_t _zero = (fix32_t){.i=0, .f=0};
const fix32_t _one  = (fix32_t){.i=1, .f=0};
const TValue_t T_TRUE =  {.tag = BOOL, .num = _one};
const TValue_t T_FALSE = {.tag = BOOL, .num = _zero};

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define gc __attribute__((__cleanup__(__decref)))
#endif
