#include "stdlib.h"
#include "lua.h"

TValue_t count(TVSlice_t args) {
	Table_t* tab = __get_tab(args, 0);
	TValue_t val = __opt_value(args, 1, T_NULL);
	if(val.tag == NUL) return TNUM(tab->count);
	assert(false);
}

TValue_t* all(TValue_t t) {
	assert(t.tag == TAB);
	Table_t* tab = GETTAB(t);
	// this calloc sets tag = NUL
	// +1 to ensure a T_NULL can always be a delimiter
	TValue_t* ret = calloc(tab->count+1, sizeof(TValue_t));
	uint16_t idx = 0;
	for(uint16_t i=0; i<tab->kvp.len; i++) {
		if(tab->kvp.kvs[i].value.tag != NUL) {
			assert(idx<tab->count);
			ret[idx] = tab->kvp.kvs[i].value;
			idx++;
		}
	}
	ret[idx] = T_NULL;
	return ret;
}
TValue_t add(TValue_t tab, TValue_t v) {
	// TODO: optional index field
	assert(tab.tag == TAB);

	int16_t wanted = 1;
	while(true) {
		if(get_tabvalue(tab, TNUM(wanted)).tag == NUL) {
			break;
		}
		wanted++;
	}
	// wanted was not found as a contiguous number, add there
	set_tabvalue(tab, TNUM(wanted), v);
	return v;
}

TValue_t del(TValue_t tab, TValue_t v) {
	assert(tab.tag == TAB);

	int16_t wanted = 1;
	TValue_t last_contiguous_key = T_NULL;
	TValue_t found_value = T_NULL;
	while(true) {
		TValue_t key = TNUM(wanted);
		TValue_t val = get_tabvalue(tab, key);
		if(val.tag == NUL) {
			break;
		}
		if(last_contiguous_key.tag == NUL && equal(val, v)) {
			last_contiguous_key = key;
			found_value = val;
			continue;
		}
		set_tabvalue(tab, last_contiguous_key, val); // copy val over to previous key
		last_contiguous_key = key;
		wanted++;
	}
	if(last_contiguous_key.tag != NUL) {
		del_tabvalue(tab, last_contiguous_key);
	}
	return found_value;
}

TValue_t deli(TVSlice_t varargs) {
	assert(false);
}

void _foreach(TValue_t t, Func_t f) {
	assert(t.tag == TAB);
	Table_t* tab = GETTAB(t);
	for(uint16_t i=0; i<tab->kvp.len; i++) {
		if(tab->kvp.kvs[i].key.tag != NUL) {
			f((TVSlice_t){.elems=(TValue_t[1]){tab->kvp.kvs[i].value}, .num=1});
		}
	}
}


void _foreach_tvalue(TValue_t t, TValue_t f) {
	assert(f.tag == FUN);
	_foreach(t, f.fun);
}

KV_t* pairs(TValue_t t) {
	assert(t.tag == TAB);
	Table_t* tab = GETTAB(t);
	KV_t* ret = calloc(tab->count+1, sizeof(KV_t));
	uint16_t j = 0;
	for(uint16_t i=0; i<tab->kvp.len; i++) {
		if(tab->kvp.kvs[i].key.tag == NUL) continue;
		ret[j] = tab->kvp.kvs[i];
		j++;
	}
	ret[j] = (KV_t){.key = T_NULL, .value = T_NULL};
	return ret;
}

KV_t* ipairs(TValue_t t) {
	// based on `add`
	assert(t.tag == TAB);
	Table_t* tab = GETTAB(t);
	KV_t* ret = calloc(tab->count+1, sizeof(KV_t));
	int16_t wanted = 1;
	while(true) {
		TValue_t _key = TNUM(wanted);
		TValue_t _val = get_tabvalue(t, _key);
		if(_val.tag == NUL) {
			break;
		}
		// wanted is "1-based indexing"
		ret[wanted-1] = (KV_t){.key = _key, .value = _val};
		wanted++;
	}

	ret[wanted-1] = (KV_t){.key = T_NULL, .value = T_NULL};
	return ret;
}

