#include "lua.h"
#include "pico8.h"
TValue_t vector = T_NULL;
TValue_t __preinit();
TValue_t __vector_normal(TVSlice_t function_arguments);
TValue_t __vector_len(TVSlice_t function_arguments);
TValue_t __table_func_vector_new(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc one = T_NULL;
  TValue_t gc zero = T_NULL;
  _set(&zero, CALL((get_tabvalue(vector, TSTR("new"))), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  printh(CALL((get_tabvalue(zero, TSTR("len"))), ((TVSlice_t){.elems = (TValue_t[1]){zero}, .num = 1})));
  _set(&one, CALL((get_tabvalue(vector, TSTR("new"))), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(1)}, .num = 2})));
  printh(CALL((get_tabvalue(one, TSTR("len"))), ((TVSlice_t){.elems = (TValue_t[1]){one}, .num = 1})));
}

TValue_t __table_func_vector_new(TVSlice_t function_arguments) {
  TValue_t gc o = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&o, TTAB(make_table(4)));
  // Fields for table o
  set_tabvalue(o, TSTR("x"), x);
  set_tabvalue(o, TSTR("y"), y);
  setmetatable(o, vector);
  return o;
}

TValue_t __vector_len(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  return _sqrt(_add(_sqr(get_tabvalue(self, TSTR("x"))), _sqr(get_tabvalue(self, TSTR("y")))));
}

TValue_t __vector_normal(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(self, TSTR("a"), CALL((get_tabvalue(self, TSTR("len"))), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
  idiv_tab(self, TSTR("x"), CALL((get_tabvalue(self, TSTR("len"))), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
  idiv_tab(self, TSTR("y"), CALL((get_tabvalue(self, TSTR("len"))), ((TVSlice_t){.elems = (TValue_t[1]){self}, .num = 1})));
}

TValue_t __preinit() {
  _set(&vector, TTAB(make_table(4)));
  set_tabvalue(vector, TSTR("__index"), vector);
  set_tabvalue(vector, TSTR("len"), TFUN(__vector_len));
  set_tabvalue(vector, TSTR("normal"), TFUN(__vector_normal));
  set_tabvalue(vector, TSTR("new"), TFUN(__table_func_vector_new));
}
