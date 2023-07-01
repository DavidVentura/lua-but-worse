#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lua.h"
//#include "fix32.h"


TArena_t _tables = {.tables=NULL, .len=0, .used=0};
SArena_t _strings = {.strings=NULL, .len=0};

#ifdef DEBUG
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)
#else
  #define DEBUG_PRINT(...) do{ } while ( false )
#endif


/* Pending optimizations:
 *
 * Fast table access:
 * For known table indexes, like in the case of 
 * a = {x: 5, y: 6}
 * a.z = 7
 *
 * instead of using `get_tabvalue(a, "x")`, a direct index can be assigned for both
 * set & get.
 * This would always bypass key lookup on `get`; and _can_ bypass the current
 * linear lookup for a free slot for `set`; though that will be diminished with the
 * pool bitmap
 *
 * Bitmap for "free table slot"
 * Instead of iterating over the entire `tables` array to find a free slot,
 * keep a 32-bit bitmap that represents the availability of the first 32 entries.
 * Allocations will continue to be slow when there are >32 live tables.
 * Using `__cntlz` (`nsau`) should make the lookup only cost 1 instruction.
 *
 * Fix32 to_bits and from_bits could return the internal representation of a 32-bit value
 * which saves some masking & shifting
 *
 * Allocate all static strings, access them by index instead of lookup every time
 */


Table_t* ENV;
Func_t __direct_call(Func_t f) {
	assert(f != NULL);
	return f;
}

Func_t __call(TValue_t t) {
	assert(t.fun != NULL);
	return t.fun;
}
Func_t __call_ptr(TValue_t* t) {
	assert(t->fun != NULL);
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

TValue_t print_tvalue(TValue_t v) {
	char buf[12] = {0};
	switch(v.tag) {
		case NUM:
			print_fix32(v.num, buf);
			printf("%s\n", buf);
			break;
		case NUL:
			printf("nil\n");
			break;
		case STR:
			printf("%.*s\n", GETSTR(v).len, GETSTR(v).data);
			break;
		case BOOL:
			if(v.num.i == 0) {
				printf("false\n");
			} else {
				printf("true\n");
			}
			break;
		default:
			printf("idk how to print with tag %d\n", v.tag);
			break;
	}
	return v;
}
void print_tvalue_ptr(TValue_t* v) {
	print_tvalue(*v);
}

bool _streq(Str_t a, Str_t b) {
	if(a.len != b.len) return false;
	for(uint16_t i=0; i<a.len; i++) {
		if (a.data[i] != b.data[i]) return false;
	}
	return true;
}

bool equal(TValue_t a, TValue_t b) {
	if(a.tag != b.tag) return false;
	switch(a.tag) {
		case NUM:
			return fix32_equals(a.num, b.num);
		case STR:
			return _streq(GETSTR(a), GETSTR(b));
		case NUL:
			return true;
		case TAB:
			return a.table_idx == b.table_idx;
		default:
			printf("IDK how to compare type %d\n", a.tag);
			return false;
	}
}
TValue_t _equal(TValue_t a, TValue_t b) {
	return TBOOL(equal(a, b));
}


void grow_table(uint16_t idx)  {
#ifdef NO_GROW_TABLE
	assert(false);
#else
	Table_t* t = &_tables.tables[idx];
	uint16_t new_len = t->len * 2;
	new_len = (t->len == 0) ? 2 : new_len;
	// this sets key->tag to 0 (NUL) for all new spaces in KVs
	KV_t* new_kvs = calloc(sizeof(KV_t), new_len);
	if(t->len) {
		memcpy(new_kvs, t->kvs, sizeof(KV_t) * t->len);
	}
	free(t->kvs);
	t->kvs = new_kvs;
	t->len = new_len;
	// t->count does not change
#endif
}


void set_tabvalue(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	Table_t* u = GETTAB(t);
	assert(u != NULL);
	assert(key.tag != NUL); // lua throws "table index is nil"
	uint16_t first_null = UINT16_MAX;
	bool is_index = key.tag == STR && _streq(GETSTR(key), STR__INDEX);

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
	assert(first_null == UINT16_MAX);
	// did not find a matching key nor any NULs
	grow_table(t.table_idx);
	// cannot fail
	// TODO(OPT): assign straight into old_len+1
	return set_tabvalue(t, key, v);
}

TValue_t get_tabvalue(TValue_t u, TValue_t key) {
	if(key.tag == NUL) return T_NULL;
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
				return CALL(__index, 1, ((TValue_t[]){key}));
			default:
				assert(false);
		}
	}
	return T_NULL;
}

TValue_t del_tabvalue(TValue_t u, TValue_t key) {
	if(key.tag == NUL) return T_NULL;
	Table_t* t = GETTAB(u);
	for(uint16_t i=0; i<t->len; i++) {
		if (equal(t->kvs[i].key, key)) {
			TValue_t ret = t->kvs[i].value;
			t->kvs[i].key = T_NULL;
			t->kvs[i].value = T_NULL;
			return ret;
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

TValue_t _sub(TValue_t a, TValue_t b) {
	return TNUM(fix32_sub(a.num, b.num));
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
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	fix32_pluseq(&a->num, b.num);
}

void _minuseq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	fix32_minuseq(&a->num, b.num);
}

TValue_t _geq(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TBOOL(fix32_geq(a.num, b.num));
}

TValue_t _gt(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TBOOL(fix32_gt(a.num, b.num));
}

TValue_t _leq(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TBOOL(fix32_leq(a.num, b.num));
}

TValue_t _lt(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TBOOL(fix32_lt(a.num, b.num));
}

TValue_t _invert_sign(TValue_t a) {
	assert(a.tag == NUM);
	return TNUM(fix32_invert_sign(a.num));
}

TValue_t _notequal(TValue_t a, TValue_t b) {
	return TBOOL(!equal(a, b));
}

TValue_t _not(TValue_t a) {
	return TBOOL(!__bool(a));
}

TValue_t _mod(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return (TValue_t){.tag=NUM, .num=(fix32_mod(a.num, b.num))};
}

bool __mbool(bool b) {
	return b;
}
bool __bool(TValue_t a) {
	if(a.tag == NUL)
		return false;
	if(a.tag == BOOL)
		return a.num.i != 0;
	return true;
}

TValue_t _or(TValue_t a, TValue_t b) {
	// TODO: short-cut evaluation for function calls
	return __bool(a) ? a : b;
}

TValue_t _and(TValue_t a, TValue_t b) {
	// TODO: short-cut evaluation for function calls
	return __bool(a) && __bool(b) ? b : a;
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

	// TODO(CORR): find correct index for table
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
	assert(v.tag == NUM); //TODO(CORR): tables can override this
	TValue_t newval = _add(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

void isub_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO(CORR): tables can override this
	TValue_t newval = _sub(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

void idiv_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO(CORR): tables can override this
	TValue_t newval = _div(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

TValue_t count(TValue_t t) {
	assert(t.tag == TAB);
	uint16_t _count = GETTAB(t)->count;
	return TNUM(_count);
}

uint16_t _find_str_index(Str_t s) {
	for(uint16_t i=0; i<_strings.len; i++) {
		if (_streq(_strings.strings[i], s)) {
			return i;
		}
	}
	return UINT16_MAX;
}

uint16_t _store_str(Str_t s) {
	uint16_t ret = UINT16_MAX;
	for(uint16_t i = 0; i<_strings.len; i++) {
		if (_strings.strings[i].len == 0) {
			ret = i;
			break;
		}
	}
	if(ret == UINT16_MAX) {
		uint16_t old_len = _strings.len;
		uint16_t new_len = _strings.len == 0 ? 16 : _strings.len*2;
		if (_strings.strings == NULL) {
			_strings.strings = calloc(new_len, sizeof(Str_t));
			// this calloc sets `len` to 0
			// and we also set `len` to 0 when ref_count=0
		} else {
			_strings.strings = realloc(_strings.strings, new_len*sizeof(Str_t));
		}
		_strings.len = new_len;
		ret = old_len + 1;
	}
	DEBUG_PRINT("Allocating a str for '%.*s' at %d\n", s.len, s.data, ret);
	_strings.strings[ret] = s;
	return ret;
}

uint16_t make_str(char* c) {
	uint16_t len = strlen(c);
	uint8_t* data = malloc(len);
	memcpy(data, c, len);
	Str_t s = (Str_t){.len=len, .data=data};
	uint16_t strindex = _find_str_index(s);
	if (strindex == UINT16_MAX) {
		strindex = _store_str(s);
	} else {
		// TODO(OPT): less disgusting way of finding temp strings
		free(data);
	}
	return strindex;
}

void _tab_decref(Table_t* t, uint16_t cur_idx) {
	t->refcount--;
	if(t->refcount==0) {
		DEBUG_PRINT("nuked table %d\n", cur_idx);
	}
	if(t->metatable_idx != UINT16_MAX) {
		DEBUG_PRINT("now nuking its metatable %d\n", t->metatable_idx);
		_tab_decref(&_tables.tables[t->metatable_idx], t->metatable_idx);
	}
}
void _decref(TValue_t v) {
	switch(v.tag) {
		case NUL:
		case NUM:
		case FUN:
		case BOOL:
			// these are value types
			break;
		case TAB:
			assert(GETTAB(v)->refcount > 0);
			_tab_decref(GETTAB(v), v.table_idx);
			break;
		case STR:
			assert(GETSTRP(v)->refcount > 0);
			GETSTRP(v)->refcount--;
			if(GETSTRP(v)->refcount==0) {
				DEBUG_PRINT("nuked %.*s\n", GETSTRP(v)->len, GETSTRP(v)->data);
				GETSTRP(v)->len = 0;
				free(GETSTRP(v)->data);
			}
			break;
	}
}

void __decref(TValue_t* v) {
	switch(v->tag) {
		case NUL:
		case NUM:
		case FUN:
		case BOOL:
			// these are value types
			return;
		default:
			break;
	}
	DEBUG_PRINT("End of scope for: \n");
	_decref(*v);
	DEBUG_PRINT("/End of scope\n");
}
void _incref(TValue_t v) {
	switch(v.tag) {
		case NUL:
		case NUM:
		case FUN:
		case BOOL:
			// these are value types
			break;
		case TAB:
			//DEBUG_PRINT("added refc on tab '%d'\n", GETTAB(v)->, GETTAB(v)->data);
			GETTAB(v)->refcount++;
			break;
		case STR:
			DEBUG_PRINT("added refc on '%.*s'\n", GETSTRP(v)->len, GETSTRP(v)->data);
			GETSTRP(v)->refcount++;
			break;
	}
}

void _set(TValue_t* dst, TValue_t src) {
	_incref(src); // if dst == src, doing _decref first
				  // may end up freeing a value, which 
				  // immediately gets _incref'd
	_decref(*dst);
	memcpy(dst, &src, sizeof(TValue_t));
}

TValue_t _concat(TValue_t a, TValue_t b) {
	// FIXME: this is bugged:
	// when a string is "", its length is 0
	// making it overwritable
	// TODO(OPT): how to prevent unnecessary allocation
	// when the key exists?
	assert(a.tag == STR);
	assert(b.tag == STR);

	uint16_t alen = GETSTR(a).len;
	uint16_t blen = GETSTR(b).len;

	if(alen==0) return b;
	if(blen==0) return a;

	uint8_t* data = malloc(alen+blen);

	memcpy(data, 		GETSTR(a).data, alen);
	memcpy(data+alen, 	GETSTR(b).data, blen);
	Str_t ret = (Str_t){.len=alen+blen, .data=data};

	uint16_t strindex = _find_str_index(ret);
	if (strindex == UINT16_MAX) {
		strindex = _store_str(ret);
	} else {
		DEBUG_PRINT("Was an unnecessary allocation\n");
		free(data);
	}

	return (TValue_t){.tag=STR, .str_idx=strindex};
}

TValue_t __internal_debug_str_len() {
	return TNUM(_strings.len);
}

TValue_t __internal_debug_str_used() {
	uint16_t ret = 0;
	for(uint16_t i = 0; i<_strings.len; i++) {
		if(_strings.strings[i].len != 0) ret++;
	}
	return TNUM(ret);
}

TValue_t tostring(TValue_t v) {
	// TODO(OPT): use a static buff
	TValue_t ret;
	assert(v.tag == NUM);
	char* buf = calloc(12, 1);
	print_fix32(v.num, buf);
	ret = TSTR(buf);
	free(buf);
	return ret;
}
void __internal_debug_assert_eq(TValue_t got, TValue_t expected) {
	bool eq = equal(got, expected);
	if (eq) return;
	printf("FIXME Expected: ");
	//printh(expected);
	printf("Got: ");
	//printh(got);
}


TValue_t __get_array_index_capped(TValue_t* arr, uint8_t arrlen, uint8_t idx) {
	if(idx >= arrlen) return T_NULL;
	return arr[idx];
}

int16_t __get_int(TValue_t* arr, uint8_t arrlen, uint8_t idx) {
	assert(idx < arrlen);
	assert(arr[idx].tag == NUM);
	assert(arr[idx].num.f == 0);
	return arr[idx].num.i;
}

int16_t __opt_int(TValue_t* arr, uint8_t arrlen, uint8_t idx, int16_t _default) {
	if(idx >= arrlen) return _default;
	assert(arr[idx].tag == NUM);
	assert(arr[idx].num.f == 0);
	return arr[idx].num.i;
}

bool __get_bool(TValue_t* arr, uint8_t arrlen, uint8_t idx) {
	assert(idx < arrlen);
	assert(arr[idx].tag == BOOL);
	return __bool(arr[idx]);
}

bool __opt_bool(TValue_t* arr, uint8_t arrlen, uint8_t idx, bool _default) {
	if(idx >= arrlen) return _default;
	assert(arr[idx].tag == BOOL);
	return __bool(arr[idx]);
}

fix32_t __get_num(TValue_t* arr, uint8_t arrlen, uint8_t idx) {
	assert(idx < arrlen);
	assert(arr[idx].tag == NUM);
	return arr[idx].num;
}

fix32_t __opt_num(TValue_t* arr, uint8_t arrlen, uint8_t idx, fix32_t _default) {
	if(idx >= arrlen) return _default;
	assert(arr[idx].tag == NUM);
	return arr[idx].num;
}

Str_t* __get_str(TValue_t* arr, uint8_t arrlen, uint8_t idx) {
	assert(idx < arrlen);
	assert(arr[idx].tag == STR);
	return GETSTRP(arr[idx]);
}

Str_t* GETSTRP(TValue_t x) {
	return &_strings.strings[x.str_idx];
}
Str_t GETSTR(TValue_t x) {
	return _strings.strings[x.str_idx];
}
Table_t* GETTAB(TValue_t x) {
	return &_tables.tables[x.table_idx];
}
Table_t GETMETATAB(Table_t x) {
	return _tables.tables[x.metatable_idx];
}
