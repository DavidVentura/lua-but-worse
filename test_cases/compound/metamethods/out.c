#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t vector = T_NULL;
TValue_t __str___sub = T_NULL;
TValue_t __str___add = T_NULL;
TValue_t __str___index = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_new = T_NULL;
TValue_t __preinit();
TValue_t __table_func_vector___sub(TVSlice_t function_arguments);
TValue_t __table_func_vector___add(TVSlice_t function_arguments);
TValue_t __table_func_vector_new(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc added = T_NULL;
  TValue_t gc delta = T_NULL;
  TValue_t gc v2 = T_NULL;
  TValue_t gc v1 = T_NULL;
  _set(&v1, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(5), TNUM16(3)}, .num = 2})));
  _set(&v2, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(2)}, .num = 2})));
  _set(&delta, _sub(v1, v2));
  printh(get_tabvalue(delta, __str_x));
  printh(get_tabvalue(delta, __str_y));
  _set(&added, _add(v1, v2));
  printh(get_tabvalue(added, __str_x));
  printh(get_tabvalue(added, __str_y));
}

TValue_t __table_func_vector_new(TVSlice_t function_arguments) {
  TValue_t gc o = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&o, TTAB(make_table(4)));
  // Fields for table o
  set_tabvalue(o, __str_x, x);
  set_tabvalue(o, __str_y, y);
  setmetatable(o, vector);
  _mark_for_gc(o);
  return o;
}

TValue_t __table_func_vector___add(TVSlice_t function_arguments) {
  TValue_t gc v = T_NULL;
  TValue_t gc v1 = T_NULL;
  TValue_t gc v0 = T_NULL;
  _set(&v0, __get_array_index_capped(function_arguments, 0));
  _set(&v1, __get_array_index_capped(function_arguments, 1));
  _set(&v, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  set_tabvalue(v, __str_x, _add(get_tabvalue(v0, __str_x), get_tabvalue(v1, __str_x)));
  set_tabvalue(v, __str_y, _add(get_tabvalue(v0, __str_y), get_tabvalue(v1, __str_y)));
  _mark_for_gc(v);
  return v;
}

TValue_t __table_func_vector___sub(TVSlice_t function_arguments) {
  TValue_t gc v = T_NULL;
  TValue_t gc v1 = T_NULL;
  TValue_t gc v0 = T_NULL;
  _set(&v0, __get_array_index_capped(function_arguments, 0));
  _set(&v1, __get_array_index_capped(function_arguments, 1));
  _set(&v, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  set_tabvalue(v, __str_x, _sub(get_tabvalue(v0, __str_x), get_tabvalue(v1, __str_x)));
  set_tabvalue(v, __str_y, _sub(get_tabvalue(v0, __str_y), get_tabvalue(v1, __str_y)));
  _mark_for_gc(v);
  return v;
}

TValue_t __preinit() {
  _grow_strings_to(6);
  _set(&__str_new, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 5)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 4)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 3)));
  _set(&__str___index, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 2)));
  _set(&__str___add, TSTRi(_store_str_at_or_die(CONSTSTR("__add"), 1)));
  _set(&__str___sub, TSTRi(_store_str_at_or_die(CONSTSTR("__sub"), 0)));
  _set(&vector, TTAB(make_table(4)));
  set_tabvalue(vector, __str___index, vector);
  set_tabvalue(vector, __str_new, TFUN(__table_func_vector_new));
  set_tabvalue(vector, __str___add, TFUN(__table_func_vector___add));
  set_tabvalue(vector, __str___sub, TFUN(__table_func_vector___sub));
}
