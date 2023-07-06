#include "lua.h"
#include "pico8.h"
TValue_t vector = T_NULL;
TValue_t __str_normal = T_NULL;
TValue_t __str___index = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_len = T_NULL;
TValue_t __str_new = T_NULL;
TValue_t __preinit();
TValue_t __vector_normal(TVSlice_t function_arguments);
TValue_t __vector_len(TVSlice_t function_arguments);
TValue_t __table_func_vector_new(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc one = T_NULL;
  TValue_t gc zero = T_NULL;
  _set(&zero, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  printh(CALL((get_tabvalue(zero, __str_len)), ((TVSlice_t){.elems = (TValue_t[1]){zero}, .num = 1})));
  _set(&one, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(1)}, .num = 2})));
  printh(CALL((get_tabvalue(one, __str_len)), ((TVSlice_t){.elems = (TValue_t[1]){one}, .num = 1})));
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
  _incref(o);
  return o;
}

TValue_t __vector_len(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  return _sqrt(_add(_sqr(get_tabvalue(self, __str_x)), _sqr(get_tabvalue(self, __str_y))));
}

TValue_t __vector_normal(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(self, __str_a, CALL((get_tabvalue(self, __str_len)), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
  idiv_tab(self, __str_x, CALL((get_tabvalue(self, __str_len)), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
  idiv_tab(self, __str_y, CALL((get_tabvalue(self, __str_len)), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
}

TValue_t __preinit() {
  _grow_strings_to(7);
  _set(&__str_new, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 6)));
  _set(&__str_len, TSTRi(_store_str_at_or_die(CONSTSTR("len"), 5)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 4)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 3)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 2)));
  _set(&__str___index, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 1)));
  _set(&__str_normal, TSTRi(_store_str_at_or_die(CONSTSTR("normal"), 0)));
  _set(&vector, TTAB(make_table(4)));
  set_tabvalue(vector, __str___index, vector);
  set_tabvalue(vector, __str_len, TFUN(__vector_len));
  set_tabvalue(vector, __str_normal, TFUN(__vector_normal));
  set_tabvalue(vector, __str_new, TFUN(__table_func_vector_new));
}
