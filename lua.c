#define _GNU_SOURCE
#include <dlfcn.h> // debug symbols from functions
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lua.h"
//#include "fix32.h"


const uint16_t TBL_BMAP_LEN = 32;
TArena_t _tables = {.tables=NULL, .len=0, .used=0};
SArena_t _strings = {.strings=NULL, .len=0};
FArena_t _funcs = {.funcs=NULL, .len=0};

TVRefSlice_t _gc_to_visit = {.len=0, .ref=NULL};

Str_t _concat_buf = {.len=0, .data=NULL};

#define dbg_assert(x)  do { if(!(x)) { print_trace(); assert(x); } } while (0)

/* Pending optimizations:
 *
 * Bitmap for "free table slot":
 *   - Should probably use up to 4 bitmaps, as L1$ in ESP32 is 32 bytes.
 *   - Check that `__builtin_ffs` maps to `__cntlz` (`nsau`).
 *
 * Fix32 to_bits and from_bits could return the internal representation of a 32-bit value
 * which saves some masking & shifting
 *
 */

#include <execinfo.h>
void print_trace (void) {
  void *array[5];
  char **strings;
  int size, i;

  size = backtrace (array, 5);
  strings = backtrace_symbols (array, size);
  if (strings != NULL && size > 2) {
    printf ("Obtained %d stack frames.\n", size);
    for (i = 2; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);
}

Table_t* ENV;
void _debug_callstack(Func_t f) {
	Dl_info info;
	dladdr(f, &info);
	printf("eeey %s:%s\n", info.dli_fname, info.dli_sname);
}
inline TValue_t __direct_call(Func_t f, TVSlice_t args) {
	// can never be closure
#ifdef DEBUG
	dbg_assert(f != NULL);
#endif
	// _debug_callstack(f);
	return f(args);
}

TValue_t __call(TValue_t t, TVSlice_t args) {
	if (t.tag != FUN) {
		DEBUG_PRINT("attempt to call a nil value\n");
		print_trace();
		assert(false);
	}
	TFunc_t* f = GETTFUN(t);
	DEBUG2_PRINT("Calling %s\n", f->name);
	assert(f->fun != NULL);
	if(f->env_table_idx != UINT16_MAX) {
		// This is not stack allocated as _most_ of the time the code-path isn't taken
		// This can't use a static, shared buffer as it must be re-entrant, closures
		// can call other closures.
		// TODO: move to another function duh
		TValue_t* argarray = calloc(args.num+1, sizeof(TValue_t));
		memcpy(argarray, args.elems, sizeof(TValue_t)*args.num);
		argarray[args.num] = TTAB(f->env_table_idx);
		TValue_t ret = f->fun((TVSlice_t){.elems=argarray, .num=args.num+1});
		free(argarray);
		return ret;
	}
	return f->fun(args);
}

TVSlice_t concat_slice(TVSlice_t a, TVSlice_t b) {
	TValue_t* argarray = calloc(a.num+b.num, sizeof(TValue_t*));
	return (TVSlice_t){.num=a.num+b.num, .elems=argarray};
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

TValue_t printh_lambda(TVSlice_t a) {
	assert(a.num == 1);
	return print_tvalue(a.elems[0]);
}
TValue_t print_tvalue(TValue_t v) {
	char buf[MAX_STR_LEN_FIX32] = {0};
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
	uint16_t new_len = t->kvp.len == 0 ? 2 : t->kvp.len * 2;
	DEBUG2_PRINT("Growing table %d len to %d\n", idx, new_len);
	// this sets key->tag to 0 (NUL) for all new spaces in KVs
	KV_t* new_kvs = calloc(new_len, sizeof(KV_t));
	if(t->kvp.len) {
		memcpy(new_kvs, t->kvp.kvs, sizeof(KV_t) * t->kvp.len);
	}
	if(t->kvp.kvs != NULL) {
		free(t->kvp.kvs);
	}
	t->kvp.kvs = new_kvs;
	t->kvp.len = new_len;
	// t->count does not change
#endif
}


void set_tabvalue(TValue_t t, TValue_t key, TValue_t v) {
	DEBUG2_PRINT("Assigning on table idx %d\n", t.table_idx);
	assert(t.tag == TAB);
	Table_t* u = GETTAB(t);
	assert(u != NULL);
	if (v.tag == NUL) {
		// need to lower u->count iff old value was not null
		DEBUG_PRINT("tried to set_tabvalue to null\n");
		return;
	}
	assert(key.tag != NUL); // lua throws "table index is nil"
	uint16_t first_null = UINT16_MAX;

	if (key.tag == STR) {
		Str_t _maybe_meta = GETSTR(key);
		// 5 as "__str"/"__add" (shortest metamethod) is 5 long
		// it's just a silly optiimization as most comparisons are usually against
		// values like "x" or "y"
		if (_maybe_meta.len >= 5 && _maybe_meta.data[0] == '_' && _maybe_meta.data[1] == '_') {
			if (u->mm == NULL) {
				u->mm = malloc(sizeof(Metamethod_t));
			}
			if(_streq(_maybe_meta, STR__INDEX)) {
				u->mm->__index = v;
				return;
			}
			if(_streq(_maybe_meta, STR__ADD)) {
				u->mm->__add = v;
				return;
			}
			if(_streq(_maybe_meta, STR__SUB)) {
				u->mm->__sub = v;
				return;
			}
			// TODO: other operations
		}
	}

	for(uint16_t i=0; i<u->kvp.len; i++) {
		if (equal(u->kvp.kvs[i].key, key)) {
			_decref(u->kvp.kvs[i].value);
			u->kvp.kvs[i].value = v;
			_incref(v);
			return;
		}
		if(u->kvp.kvs[i].key.tag == NUL && first_null == UINT16_MAX) {
			first_null = i;
		}
	}
	if (first_null < UINT16_MAX) {
		u->kvp.kvs[first_null].key = key;
		u->kvp.kvs[first_null].value = v;
		_incref(v);
		u->count++;
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
	for(uint16_t i=0; i<t->kvp.len; i++) {
		if (equal(t->kvp.kvs[i].key, key)) {
			return t->kvp.kvs[i].value;
		}
	}
	if(t->metatable_idx != UINT16_MAX) {
		Table_t meta = GETMETATAB(*t);
		if (meta.mm == NULL) return T_NULL;
		TValue_t __index = meta.mm->__index;
		switch(__index.tag) {
			case TAB:
				if(__index.table_idx == u.table_idx) {
					// we've got `tab.__index = tab`
					// and have gotten all the way down the inheritance tree
					// up to `tab` itself, still not found the key -> is not there
					return T_NULL;
				}
				return get_tabvalue(TTAB(__index.table_idx), key);
			case FUN:
				return CALL(__index, ((TVSlice_t){.elems=(TValue_t[]){key}, .num=1}));
			case NUL:
				printf("metatab points to null, but metatable_idx is valid (%d)\n", t->metatable_idx);
				assert(false);
			default:
				printf("metatab points to type %d\n", __index.tag);
				assert(false);
		}
	}
	return T_NULL;
}

TValue_t del_tabvalue(TValue_t u, TValue_t key) {
	if(key.tag == NUL) return T_NULL;
	Table_t* t = GETTAB(u);
	for(uint16_t i=0; i<t->kvp.len; i++) {
		if (equal(t->kvp.kvs[i].key, key)) {
			TValue_t ret = t->kvp.kvs[i].value;
			// Add `ret` to gc instead of immediately calling `_decref` on it
			// as we must return the deleted value to the caller of `del`.
			if (ret.tag == TAB) {
				add_to_gc(ret.table_idx, TAB);
			} else if (ret.tag == STR) {
				add_to_gc(ret.str_idx, STR);
			}
			t->kvp.kvs[i].key = T_NULL;
			t->kvp.kvs[i].value = T_NULL;
			t->count--;
			return ret;
		}
	}
	return T_NULL;
}

TValue_t _mult(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TNUM(fix32_mul(a.num, b.num));
}

TValue_t _add(TValue_t a, TValue_t b) {
	if (a.tag == NUM && b.tag == NUM) {
		return TNUM(fix32_add(a.num, b.num));
	}
	if (a.tag == TAB && b.tag == TAB) {
		// what happens if they have different metatables or...
		Table_t* t = GETTAB(a);
		assert(t->metatable_idx != UINT16_MAX);
		Table_t mt = GETMETATAB(*t);
		assert(mt.mm != NULL);
		assert(mt.mm->__add.tag == FUN);
		return CALL(mt.mm->__add, ((TVSlice_t){.elems=(TValue_t[2]){a, b}, .num=2}));
	}
	assert(false);
}

TValue_t _sub(TValue_t a, TValue_t b) {
	if (a.tag == NUM && b.tag == NUM) {
		return TNUM(fix32_sub(a.num, b.num));
	}
	if (a.tag == TAB && b.tag == TAB) {
		// what happens if they have different metatables or...
		Table_t* t = GETTAB(a);
		assert(t->metatable_idx != UINT16_MAX);
		Table_t mt = GETMETATAB(*t);
		assert(mt.mm != NULL);
		assert(mt.mm->__sub.tag == FUN);
		return CALL(mt.mm->__sub, ((TVSlice_t){.elems=(TValue_t[2]){a, b}, .num=2}));
	}
	assert(false);
}

TValue_t _floor_div(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TNUM(fix32_flr(fix32_div(a.num, b.num)));
}

TValue_t _div(TValue_t a, TValue_t b) {
	assert(a.tag == NUM);
	assert(b.tag == NUM);
	return TNUM(fix32_div(a.num, b.num));
}

TValue_t _sqrt(TValue_t a) {
	assert(a.tag == NUM);
	return TNUM(fix32_sqrt(a.num));
}

TValue_t _pow(TValue_t a, TValue_t b){
	return TNUM(fix32_pow(a.num, b.num));
}

TValue_t _ceil(TValue_t a) {
	return TNUM(fix32_ceil(a.num));
}

void _pluseq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	fix32_pluseq(&a->num, b.num);
}

void _muleq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	fix32_muleq(&a->num, b.num);
}

void _minuseq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	fix32_minuseq(&a->num, b.num);
}

void _diveq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	a->num = fix32_div(a->num, b.num);
}

void _modeq(TValue_t* a, TValue_t b) {
	assert(a->tag == NUM);
	assert(b.tag == NUM);
	a->num = fix32_mod(a->num, b.num);
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
inline bool __bool(TValue_t a) {
	if(a.tag == NUL)
		return false;
	if(a.tag == BOOL)
		return a.num.i != 0;
	return true;
}

TValue_t _or(TValue_t a, TValue_t b) {
	return __bool(a) ? a : b;
}

TValue_t _and(TValue_t a, TValue_t b) {
	return __bool(a) ? b : a ;
}

uint32_t free_tables;
uint16_t make_table(uint16_t size) {
	if(_tables.len == _tables.used) {
		uint16_t new_len = _tables.len == 0 ? TBL_BMAP_LEN : _tables.len*2;
		if (_tables.tables == NULL) {
			_tables.tables = calloc(new_len, sizeof(Table_t));
			free_tables = 0xffffffff;
		} else {
			uint16_t old_len = _tables.len;
			assert(new_len > old_len);
			_tables.tables = realloc(_tables.tables, new_len*sizeof(Table_t));
			// only zero out the new part of the buffer
			memset(_tables.tables+old_len, 0, (new_len-old_len)*sizeof(Table_t));
		}
		_tables.len = new_len;
	}

	uint16_t retval = 0xfafa;
	if (free_tables > 0) {
		retval = __builtin_ffs(free_tables) - 1;
		free_tables ^= (1 << retval);
	} else {
		// TODO(OPT): use `used`+1 if just resized
		for(uint16_t i = TBL_BMAP_LEN; i<_tables.len; i++) {
			if(_tables.tables[i].refcount==0) {
				retval = i;
				break;
			}
		}
	}

	Table_t* tp = &_tables.tables[retval];
	if (tp->kvp.kvs == NULL && size > 0) {
		tp->kvp.kvs = calloc(size, sizeof(KV_t)); // this sets key->tag to 0 (NUL)
	} else if (tp->kvp.kvs != NULL && size > tp->kvp.len) {
		free(tp->kvp.kvs);
		tp->kvp.kvs = calloc(size, sizeof(KV_t));
	}

	tp->kvp.len = size;
	tp->metatable_idx = UINT16_MAX;
	tp->mm = NULL;
	tp->count = 0;
	tp->refcount = 1;
	add_to_gc(retval, TAB);

	_tables.used++;
	DEBUG_PRINT("Created <tab %d>\n", retval);
	return retval;
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
	DEBUG2_PRINT("<tab %d>.metatable = <tab %d>\n", t.table_idx, meta.table_idx);
	_incref(meta);
}

void iadd_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO(CORR): tables can override this
	TValue_t newval = _add(get_tabvalue(t, key), v);
	set_tabvalue(t, key, newval);
}

void imul_tab(TValue_t t, TValue_t key, TValue_t v) {
	assert(t.tag == TAB);
	assert(v.tag == NUM); //TODO(CORR): tables can override this
	TValue_t newval = _mult(get_tabvalue(t, key), v);
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

uint16_t _find_str_index(Str_t s) {
	for(uint16_t i=0; i<_strings.len; i++) {
		if (_streq(_strings.strings[i], s)) {
			return i;
		}
	}
	return UINT16_MAX;
}

uint16_t _store_str_at_or_die(Str_t s, uint16_t idx) {
	assert(_strings.len >= idx);
	assert(_strings.strings[idx].len == 0);
	_strings.strings[idx] = s;
	return idx;
}

void _grow_strings_to(uint16_t new_len) {
	if (_strings.strings == NULL) {
		DEBUG2_PRINT("Initializing strings with size %d\n", new_len);
		_strings.strings = calloc(new_len, sizeof(Str_t));
		// this calloc sets `len` to 0
		// and we also set `len` to 0 when ref_count=0
	} else {
		DEBUG2_PRINT("Growing strings to size %d\n", new_len);
		assert(new_len > _strings.len);
		uint16_t old_len = _strings.len;
		_strings.strings = realloc(_strings.strings, new_len*sizeof(Str_t));
		// only zero out the new part of the buffer
		memset(_strings.strings+old_len, 0, (new_len-old_len)*sizeof(Str_t));
	}
	_strings.len = new_len;
}

uint16_t _store_str(Str_t s) {
	// TODO(PERF): bitmap
	uint16_t ret = UINT16_MAX;
	for(uint16_t i = 0; i<_strings.len; i++) {
		if (_strings.strings[i].refcount == 0) {
			ret = i;
			break;
		}
	}

	if(ret == UINT16_MAX) {
		uint16_t old_len = _strings.len;
		uint16_t new_len = _strings.len == 0 ? 16 : _strings.len*2;
		_grow_strings_to(new_len);
		ret = old_len + 1;
	}

	DEBUG2_PRINT("Storing str '%.*s' at %d\n", s.len, s.data, ret);
	_strings.strings[ret] = s;
	return ret;
}

#ifdef DEBUG
uint16_t make_fun(Func_t f, uint16_t env_table_idx, const char* name) {
	TFunc_t t = (TFunc_t){.fun=f, .name=name, .env_table_idx=env_table_idx};
#else
uint16_t make_fun(Func_t f, uint16_t env_table_idx) {
	TFunc_t t = (TFunc_t){.fun=f, .env_table_idx=env_table_idx};
#endif
	uint16_t new_len = _funcs.len == 0 ? 32 : _funcs.len*2;
	uint16_t first_null = UINT16_MAX;

	if (_funcs.funcs == NULL) {
		_funcs.funcs = calloc(32, sizeof(TFunc_t));
		first_null = 0;
		_funcs.len = new_len;
	} else {
		for(uint16_t i=0; i<_funcs.len; i++) {
			if (_funcs.funcs[i].fun == f && _funcs.funcs[i].env_table_idx == env_table_idx) return i; // already stored
			if (_funcs.funcs[i].fun == NULL) {
				first_null = i;
				break;
			}
		}
	}
	if (first_null == UINT16_MAX) {
		_funcs.funcs = realloc(_funcs.funcs, new_len * sizeof(TFunc_t));
		memset(_funcs.funcs+_funcs.len, 0, (new_len-_funcs.len)*sizeof(TFunc_t));
		_funcs.len = new_len;
		first_null = 0;
	}

	_funcs.funcs[first_null] = t;

	return first_null;
}

uint16_t make_str(char* c) {
	uint8_t len = strlen(c);
	Str_t s = (Str_t){.data=(uint8_t*)c, .len=len, .refcount=1};
	uint16_t strindex = _find_str_index(s);
	if (strindex == UINT16_MAX) {
		uint8_t* buf = malloc(len);
		memcpy(buf, c, len);
		strindex = _store_str((Str_t){.len=len, .data=buf, .refcount=1});
		add_to_gc(strindex, STR);
	}
	return strindex;
}

void add_to_gc(uint16_t idx, enum typetag_t tag) {
	TVRef_t ref;
	ref = (TVRef_t){.idx = idx, .tag = tag };

	for(uint16_t i=0; i<_gc_to_visit.len; i++) {
		if (_gc_to_visit.ref[i].tag == NUL) {
			_gc_to_visit.ref[i] = ref;
			DEBUG2_PRINT("Added to GC!\n");
			return;
		}
	}
	// if we didn't find a slot so far, the array is full
	uint16_t new_len = _gc_to_visit.len == 0 ? 16 : _gc_to_visit.len * 2;
	TVRef_t* new_buf = calloc(new_len, sizeof(TVRef_t));
	memcpy(new_buf, _gc_to_visit.ref, _gc_to_visit.len*sizeof(TVRef_t));
	free(_gc_to_visit.ref);
	_gc_to_visit.ref = new_buf;
	_gc_to_visit.ref[_gc_to_visit.len] = ref;
	_gc_to_visit.len = new_len;
	DEBUG2_PRINT("Added to resized GC!\n");
}

void run_gc() {
	for(uint16_t i=0; i<_gc_to_visit.len; i++) {
		TVRef_t* ref = &_gc_to_visit.ref[i];
		TValue_t v = T_NULL;
		if (ref->tag == NUL) continue;
		if (ref->tag == TAB) {
			if(_tables.tables[ref->idx].refcount > 0) {
				DEBUG2_PRINT("Decref table %d by GC!\n", ref->idx);
				_tab_decref(&_tables.tables[ref->idx], ref->idx);
			}
		}
		if (ref->tag == STR) {
			if(_strings.strings[ref->idx].refcount > 0) {
				DEBUG2_PRINT("Decref string %d by GC!\n", ref->idx);
				_str_decref(&_strings.strings[ref->idx]);
			}
		}
		ref->tag = NUL;
	}
}

void _mark_for_gc(TValue_t val) {
	// This is called when returning values, we have to bump their
	// refcount so they survive going out of scope (automatic _decref)
	// and add them to the list of "objects to clean" -- if the
	// returned value still has exactly 1 reference by the time the GC runs,
	// it's the lingering reference that we add here.
	if(val.tag == TAB) {
		_incref(val);
		add_to_gc(val.table_idx, val.tag);
	}
	if(val.tag == STR) {
		_incref(val);
		add_to_gc(val.str_idx, val.tag);
	}
}

void _tab_decref(Table_t* t, uint16_t cur_idx) {
	DEBUG2_PRINT("decref <tab %d> %d->%d\n", cur_idx, t->refcount, t->refcount-1);
	t->refcount--;
	if(t->refcount!=0) {
		return;
	}
	DEBUG_PRINT("GC <tab %d>\n", cur_idx);

	if (t->kvp.len > 0) {
		for(uint16_t i=0; i<t->kvp.len; i++) {
			if(t->kvp.kvs[i].key.tag != NUL) {
				// FIXME(CORR): _decref(t->kvp.kvs[i].key);
				_decref(t->kvp.kvs[i].value);
			}
		}
		memset(t->kvp.kvs, 0, t->kvp.len * sizeof(KV_t));
	}

	if (t->mm != NULL) {
		memset(t->mm, 0, sizeof(Metamethod_t));
	}

	if (cur_idx < TBL_BMAP_LEN)
		free_tables |= (1 << cur_idx);
	_tables.used--;
	// this memset will set the `tag` on `key` and `value` to NUL
	// which means that the backing array can later be assigned to a new
	// table without an allocation
	t->count = 0;
	if(t->metatable_idx != UINT16_MAX) {
		Table_t* meta = &_tables.tables[t->metatable_idx];
		assert(meta->refcount > 0);
		DEBUG2_PRINT("  decref-ing its metatable (<tab %d>), %d->%d\n", t->metatable_idx, meta->refcount, meta->refcount-1);
		_tab_decref(meta, t->metatable_idx);
	}
}
void _str_decref(Str_t* s) {
	s->refcount--;
	if(s->refcount==0) {
		DEBUG2_PRINT("nuked %.*s\n", s->len, s->data);
		s->len = 0;
		free(s->data);
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
			_str_decref(GETSTRP(v));
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
		case TAB:
			DEBUG2_PRINT("End of scope for <tab %d>\n", v->table_idx);
			break;
		case STR:
			DEBUG2_PRINT("End of scope for '%.*s'\n", GETSTRP(*v)->len, GETSTRP(*v)->data);
			break;
	}
	_decref(*v);
	DEBUG2_PRINT("/End of scope\n");
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
			assert(GETTAB(v)->refcount < 250);
			GETTAB(v)->refcount++;
			DEBUG2_PRINT("added refc on <tab %d>=%d\n", v.table_idx, GETTAB(v)->refcount);
			break;
		case STR:
			assert(GETSTRP(v)->refcount < 250);
			DEBUG2_PRINT("added refc on '%.*s'\n", GETSTRP(v)->len, GETSTRP(v)->data);
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
	if (a.tag==NUL || b.tag==NUL) {
		DEBUG_PRINT("attempt to concatenate a nil value\n");
		assert(false);
	}

	uint8_t* a_data;
	uint8_t* b_data;
	uint16_t blen;
	uint16_t alen;
	char anumbuf[MAX_STR_LEN_FIX32] = {0};
	char bnumbuf[MAX_STR_LEN_FIX32] = {0};

	if(a.tag == NUM) {
		print_fix32(a.num, anumbuf);
		alen = strlen(anumbuf);
		a_data = (uint8_t*)anumbuf;
	} else {
		a_data = GETSTR(a).data;
		alen = GETSTR(a).len;
	}

	if(b.tag == NUM) {
		print_fix32(b.num, bnumbuf);
		blen = strlen(bnumbuf);
		b_data = (uint8_t*)bnumbuf;
	} else {
		b_data = GETSTR(b).data;
		blen = GETSTR(b).len;
	}

	if ((alen+blen) > _concat_buf.len) {
		_concat_buf.data = realloc(_concat_buf.data, alen+blen);
	}
	_concat_buf.len = alen+blen;

	memcpy(_concat_buf.data, 		a_data, alen);
	memcpy(_concat_buf.data+alen, 	b_data, blen);

	uint16_t strindex = _find_str_index(_concat_buf);
	TValue_t ret;
	if (strindex == UINT16_MAX) {
		uint8_t* buf = malloc(_concat_buf.len);
		memcpy(buf, _concat_buf.data, _concat_buf.len);
		strindex = _store_str((Str_t){.len=_concat_buf.len, .data=buf, .refcount=1});
		ret = (TValue_t){.tag=STR, .str_idx=strindex};
		add_to_gc(strindex, STR);
	} else {
		ret = (TValue_t){.tag=STR, .str_idx=strindex};
	}


	return ret;
}

TValue_t __internal_debug_str_len() {
	return TNUM(_strings.len);
}

TValue_t __internal_debug_str_used() {
	uint16_t ret = 0;
	for(uint16_t i = 0; i<_strings.len; i++) {
		if(_strings.strings[i].refcount != 0) ret++;
	}
	return TNUM(ret);
}

TValue_t __internal_debug_tables_used() {
	uint16_t ret = 0;
	for(uint16_t i = 0; i<_tables.len; i++) {
		if(_tables.tables[i].refcount != 0) ret++;
	}
	return TNUM(ret);
}

TValue_t tostring(TValue_t v) {
	if (v.tag == STR) {
		return v;
	}
	if (v.tag == TAB) {
		printf("tab %d\n", v.table_idx);
		return T_NULL;
	}
	TValue_t ret;
	assert(v.tag == NUM);
	char buf[MAX_STR_LEN_FIX32] = {0};
	print_fix32(v.num, buf);
	ret = TSTR(buf); // TSTR makes its own copy
    add_to_gc(ret.str_idx, STR);
	return ret;
}

void __internal_debug_assert_eq(TValue_t got, TValue_t expected) {
	bool eq = equal(got, expected);
	if (eq) return;
	printf("Expected: ");
	printh(expected);
	printf("Got: ");
	printh(got);
}


TValue_t __get_array_index_capped(TVSlice_t arr, uint8_t idx) {
	if(idx >= arr.num) return T_NULL;
	return arr.elems[idx];
}

int16_t __get_int(TVSlice_t args, uint8_t idx) {
	// This truncates the decimal part
	assert(idx < args.num);
	assert(args.elems[idx].tag == NUM);
	return args.elems[idx].num.i;
}

int16_t __opt_int(TVSlice_t args, uint8_t idx, int16_t _default) {
	// This truncates the decimal part
	if(idx >= args.num) return _default;
	if(args.elems[idx].tag == NUL) return _default;
	assert(args.elems[idx].tag == NUM);
	return args.elems[idx].num.i;
}

bool __get_bool(TVSlice_t args, uint8_t idx) {
	assert(idx < args.num);
	assert(args.elems[idx].tag == BOOL);
	return __bool(args.elems[idx]);
}

bool __opt_bool(TVSlice_t args, uint8_t idx, bool _default) {
	if(idx >= args.num) return _default;
	if(args.elems[idx].tag == NUL) return _default;
	assert(args.elems[idx].tag == BOOL);
	return __bool(args.elems[idx]);
}

fix32_t __get_num(TVSlice_t args, uint8_t idx) {
	assert(idx < args.num);
	assert(args.elems[idx].tag == NUM);
	return args.elems[idx].num;
}

fix32_t __opt_num(TVSlice_t args, uint8_t idx, fix32_t _default) {
	if(idx >= args.num) return _default;
	if(args.elems[idx].tag == NUL) return _default;
	assert(args.elems[idx].tag == NUM);
	return args.elems[idx].num;
}

Str_t* __get_str(TVSlice_t args, uint8_t idx) {
	assert(idx < args.num);
	assert(args.elems[idx].tag == STR);
	return GETSTRP(args.elems[idx]);
}
Table_t* __get_tab(TVSlice_t args, uint8_t idx) {
	assert(idx < args.num);
	assert(args.elems[idx].tag == TAB);
	return GETTAB(args.elems[idx]);
}

TValue_t __opt_value(TVSlice_t args, uint8_t idx, TValue_t _default) {
	if(idx >= args.num) return _default;
	return args.elems[idx];
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
TFunc_t* GETTFUN(TValue_t x) {
	return &_funcs.funcs[x.fun_idx];
}

Table_t GETMETATAB(Table_t x) {
	assert(x.metatable_idx != UINT16_MAX);
	return _tables.tables[x.metatable_idx];
}

TValue_t type(TValue_t arg) {
	// TODO(OPT): make these values constant
	switch(arg.tag) {
		case NUM:
			return TSTR("number");
		case STR:
			return TSTR("string");
		case BOOL:
			return TSTR("boolean");
		case TAB:
			return TSTR("table");
		case NUL:
			return TSTR("nil");
		case FUN:
			return TSTR("function");
	}
	assert(false);
}
TValue_t sub(TVSlice_t args) {
	Str_t* str = __get_str(args, 0);
	int16_t start = __get_int(args, 1);
	int16_t end = __opt_int(args, 2, -1);

	if (start < 0) start = str->len + start + 1;
	if (end < 0) end = str->len + end + 1;
	assert(end>start);
	uint8_t* new_data = malloc(end-start+1);
	// `start` and `end` are 1-based -- need to start copying 1 byte before
	memcpy(new_data, str->data+start-1, end-start+1);
	Str_t substr = {.data=new_data, .len=end-start+1, .refcount=1};
	uint16_t strindex = _store_str(substr);
	add_to_gc(strindex, STR);
	return TSTRi(strindex);
}

TValue_t _length(TValue_t arg) {
	assert(arg.tag == STR || arg.tag == TAB);
	if (arg.tag == STR) {
		return TNUM(GETSTR(arg).len);
	}
	return TNUM(GETTAB(arg)->count);
}
