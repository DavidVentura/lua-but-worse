#ifndef LUA
#define LUA
#include <stdbool.h>
#include <stdint.h>
#include "fix32.h"

// It is important that NUL=0 -- a bunch of places have 
// memset(0) or calloc() and those zeroes are abused to 
// ensure the containers are full of T_NULL
// The remaining values don't need to be in any particular order
enum typetag_t {NUL=0, STR=1, TAB=2, FUN=3, NUM=4, BOOL=5};

typedef struct TValue_s TValue_t;
typedef struct TVSlice_s {
	TValue_t* elems;
	uint16_t num;
} TVSlice_t;
typedef TValue_t (*Func_t)(TVSlice_t);

struct TValue_s {
	union {
		uint16_t str_idx; // maybe high bit for short/long str?
		fix32_t num;
		uint16_t table_idx;
		Func_t fun;
	};
	uint16_t env_table_idx; // closure env for `fun`. putting these in a struct takes way more space
	enum typetag_t tag; // 3 bits used only
};

_Static_assert(sizeof(TValue_t) <= 64, "too big");
_Static_assert(sizeof(TValue_t) <= 16, "too big"); // 8 for pointer on 64bit, 2 for env, 2 for table_idx, 1 for enum, 3 for padding.
#if UINTPTR_MAX == UINT32_MAX
_Static_assert(sizeof(TValue_t) <= 12, "too big"); // 12 total size on 32bit
#endif


typedef struct KV_s {
	TValue_t key;
	TValue_t value;
} KV_t;

typedef struct  __attribute__((__packed__)) KVSlice_s {
	KV_t* kvs;
	uint16_t len;
} KVSlice_t;
_Static_assert(sizeof(KVSlice_t) <= 10, "too big");

typedef struct Metamethod_s {
	TValue_t __index;
	TValue_t __add;
	TValue_t __mul;
	TValue_t __sub;
	TValue_t __div;
	TValue_t __unm;
	TValue_t __pow;
	TValue_t __concat;
} Metamethod_t;

typedef struct Table_s {
	Metamethod_t* mm;
	KVSlice_t kvp;
	uint16_t metatable_idx;
	uint16_t count;
	uint8_t refcount;
} Table_t;

// 8 on TValue_t __index 		(8 on 32bit)
// 16 on kvs  				    (8 on 32bit)
// 5 on count/refcount/metatable_idx
// 3 on padding
_Static_assert(sizeof(Table_t) <= 24, "too big");
#if UINTPTR_MAX == UINT32_MAX
_Static_assert(sizeof(Table_t) <= 16, "too big");
#endif


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


#define TNUM(x)        ((TValue_t){.tag = NUM,  .num = (x)})
#define TNUM8(x)       ((TValue_t){.tag = NUM,  .num = (fix32_from_int8(x))})
#define TNUM16(x)      ((TValue_t){.tag = NUM,  .num = (fix32_from_int16(x))})
#define TSTR(x)        ((TValue_t){.tag = STR,  .str_idx = (make_str(x))})
#define TSTRi(i)       ((TValue_t){.tag = STR,  .str_idx = (i)})
#define CONSTSTR(x)    ((Str_t){.data = (uint8_t*)(x), .len=(sizeof((x))-1), .refcount=1})
// CONSTSTR has .refcount=1 because it must operate on `const char*`
#define TBOOL(x)       ((TValue_t){.tag = BOOL, .num = (fix32_from_int8(x))})
#define TFUN(x)        ((TValue_t){.tag = FUN,  .fun = (x), .env_table_idx=UINT16_MAX})
#define TCLOSURE(x,y)  ((TValue_t){.tag = FUN,  .fun = (x), .env_table_idx=(y).table_idx})
#define TTAB(x)        ((TValue_t){.tag = TAB,  .table_idx = x})

#define CALL(x, y)  		_Generic(x, TValue_t: __call, Func_t: __direct_call)((x), (y))
#define _bool(x) 			_Generic((x), TValue_t: __bool, bool: __mbool)(x)

// Declaring this as a `const TValue_t` still raises warnings
// the warnings are solved by making the enum const (wtf?)
// but that raises other warnings
#define T_NULL ((TValue_t){.tag = NUL})
static const fix32_t _zero = {.i=0, .f=0};
static const fix32_t _one  = {.i=1, .f=0};
static const TValue_t T_TRUE =  {.tag = BOOL, .num = {.i=1, .f=0}};
static const TValue_t T_FALSE = {.tag = BOOL, .num = {.i=0, .f=0}};
static const Str_t STR__INDEX = CONSTSTR("__index");
static const Str_t STR__ADD   = CONSTSTR("__add");
static const Str_t STR__SUB   = CONSTSTR("__sub");


#define gc __attribute__((__cleanup__(__decref)))
#define printh print_tvalue

// grep -P '^\w+.*{$' lua.c | sed 's/\s\+{/;/'
TValue_t __direct_call(Func_t f, TVSlice_t args);
TValue_t __call(TValue_t t, TVSlice_t args);
TValue_t printh_lambda(TVSlice_t);
void print_bool(bool b);
void print_str(char* c);
TValue_t print_tvalue(TValue_t v);
void print_tvalue_ptr(TValue_t* v);
bool _streq(Str_t a, Str_t b);
bool equal(TValue_t a, TValue_t b);
TValue_t _equal(TValue_t a, TValue_t b);
void grow_table(uint16_t idx);
void set_tabvalue(TValue_t t, TValue_t key, TValue_t v);
TValue_t get_tabvalue(TValue_t u, TValue_t key);
TValue_t del_tabvalue(TValue_t u, TValue_t key);
TValue_t _mult(TValue_t a, TValue_t b);
TValue_t _add(TValue_t a, TValue_t b);
TValue_t _sub(TValue_t a, TValue_t b);
TValue_t _floor_div(TValue_t a, TValue_t b);
TValue_t _div(TValue_t a, TValue_t b);
TValue_t _sqrt(TValue_t a);
TValue_t _pow(TValue_t a, TValue_t b);
TValue_t _ceil(TValue_t a);
void _pluseq(TValue_t* a, TValue_t b);
void _muleq(TValue_t* a, TValue_t b);
void _minuseq(TValue_t* a, TValue_t b);
void _diveq(TValue_t* a, TValue_t b);
void _modeq(TValue_t* a, TValue_t b);
TValue_t _geq(TValue_t a, TValue_t b);
TValue_t _gt(TValue_t a, TValue_t b);
TValue_t _leq(TValue_t a, TValue_t b);
TValue_t _lt(TValue_t a, TValue_t b);
TValue_t _invert_sign(TValue_t a);
TValue_t _notequal(TValue_t a, TValue_t b);
TValue_t _not(TValue_t a);
TValue_t _mod(TValue_t a, TValue_t b);
bool __mbool(bool b);
bool __bool(TValue_t a);
TValue_t _or(TValue_t a, TValue_t b);
TValue_t _and(TValue_t a, TValue_t b);
uint16_t make_table(uint16_t size);
void free_tvalue(TValue_t tv);
TValue_t flr(TValue_t f);
TValue_t getmetatable(TValue_t t);
void setmetatable(TValue_t t, TValue_t meta);
void iadd_tab(TValue_t t, TValue_t key, TValue_t v);
void isub_tab(TValue_t t, TValue_t key, TValue_t v);
void imul_tab(TValue_t t, TValue_t key, TValue_t v);
void idiv_tab(TValue_t t, TValue_t key, TValue_t v);
uint16_t _find_str_index(Str_t s);
uint16_t _store_str(Str_t s);
uint16_t _store_str_at_or_die(Str_t s, uint16_t idx);
void _grow_strings_to(uint16_t new_len);
uint16_t make_str(char* c);
void _tab_decref(Table_t* t, uint16_t cur_idx);
void _decref(TValue_t v);
void __decref(TValue_t* v);
void _incref(TValue_t v);
void _set(TValue_t* dst, TValue_t src);
TValue_t _concat(TValue_t a, TValue_t b);
TValue_t __internal_debug_str_len();
TValue_t __internal_debug_str_used();
TValue_t tostring(TValue_t v);
void __internal_debug_assert_eq(TValue_t got, TValue_t expected);
TValue_t __get_array_index_capped(TVSlice_t arr, uint8_t idx);
int16_t  __get_int(TVSlice_t args, uint8_t idx);
int16_t  __opt_int(TVSlice_t args, uint8_t idx, int16_t _default);
bool     __get_bool(TVSlice_t args, uint8_t idx);
bool     __opt_bool(TVSlice_t args, uint8_t idx, bool _default);
fix32_t  __get_num(TVSlice_t args, uint8_t idx);
fix32_t  __opt_num(TVSlice_t args, uint8_t idx, fix32_t _default);
Str_t*   __get_str(TVSlice_t args, uint8_t idx);
Table_t* __get_tab(TVSlice_t args, uint8_t idx);
TValue_t __opt_value(TVSlice_t args, uint8_t idx, TValue_t _default);
TValue_t type(TValue_t arg);
TValue_t sub(TVSlice_t args);
TValue_t _length(TValue_t arg);

Str_t* GETSTRP(TValue_t x);
Str_t GETSTR(TValue_t x);
Table_t* GETTAB(TValue_t x);
Table_t GETMETATAB(Table_t x);
#endif
