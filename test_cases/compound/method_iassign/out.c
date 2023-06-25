#include "pico8.h"
TValue_t vector = T_NULL;
TValue_t __preinit();
TValue_t __vector_normal(uint8_t argc, TValue_t *function_arguments);
TValue_t __vector_len(uint8_t argc, TValue_t *function_arguments);
TValue_t __table_func_vector_new(uint8_t argc, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc one = T_NULL;
  TValue_t gc zero = T_NULL;
  _set(&zero, CALL((get_tabvalue(vector, TSTR("new"))), 2, ((TValue_t[2]){TNUM16(0), TNUM16(0)}))); // unknown type
  print(CALL((get_tabvalue(zero, TSTR("len"))), 1, ((TValue_t[1]){zero})));
  _set(&one, CALL((get_tabvalue(vector, TSTR("new"))), 2, ((TValue_t[2]){TNUM16(1), TNUM16(1)}))); // unknown type
  print(CALL((get_tabvalue(one, TSTR("len"))), 1, ((TValue_t[1]){one})));
}

TValue_t __table_func_vector_new(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc o = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  _set(&y, __get_array_index_capped(function_arguments, argc, 1)); // unknown type
  _set(&o, TTAB(make_table(4)));
  // Fields for table o
  set_tabvalue(o, TSTR("x"), x);
  set_tabvalue(o, TSTR("y"), y);
  setmetatable(o, vector);
  return o;
}

TValue_t __vector_len(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  return _sqrt(_add(_sqr(get_tabvalue(self, TSTR("x"))), _sqr(get_tabvalue(self, TSTR("y")))));
}

TValue_t __vector_normal(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  set_tabvalue(self, TSTR("a"), CALL((get_tabvalue(self, TSTR("len"))), 1, ((TValue_t[1]){self})));
  idiv_tab(self, TSTR("x"), CALL((get_tabvalue(self, TSTR("len"))), 1, ((TValue_t[1]){self})));
  idiv_tab(self, TSTR("y"), CALL((get_tabvalue(self, TSTR("len"))), 1, ((TValue_t[1]){self})));
}

TValue_t __preinit() {
  _set(&vector, TTAB(make_table(4)));
  set_tabvalue(vector, TSTR("__index"), vector);
  set_tabvalue(vector, TSTR("len"), TFUN(__vector_len));
  set_tabvalue(vector, TSTR("normal"), TFUN(__vector_normal));
  set_tabvalue(vector, TSTR("new"), TFUN(__table_func_vector_new));
}
