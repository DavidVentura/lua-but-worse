#include "lua.c"
TValue_t vector;
TValue_t __preinit();
TValue_t __vector_normal(TValue_t *function_arguments);
TValue_t __vector_len(TValue_t *function_arguments);
TValue_t __table_func_vector_new(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t one;
  TValue_t zero;
  zero = CALL((get_tabvalue(vector, TSTR("new"))), ((TValue_t[]){TNUM16(0), TNUM16(0)})); // unknown type
  print(CALL((get_tabvalue(zero, TSTR("len"))), ((TValue_t[]){zero})));
  one = CALL((get_tabvalue(vector, TSTR("new"))), ((TValue_t[]){TNUM16(1), TNUM16(1)})); // unknown type
  print(CALL((get_tabvalue(one, TSTR("len"))), ((TValue_t[]){one})));
}

TValue_t __table_func_vector_new(TValue_t *function_arguments) {
  TValue_t o;
  TValue_t y;
  TValue_t x;
  x = function_arguments[0]; // unknown type
  y = function_arguments[1]; // unknown type
  o = TTAB(make_table(4));
  // Fields for table o
  set_tabvalue(o, TSTR("x"), x);
  set_tabvalue(o, TSTR("y"), y);
  setmetatable(o, vector);
  free_tvalue(o);
  return o;
}

TValue_t __vector_len(TValue_t *function_arguments) {
  TValue_t self;
  self = function_arguments[0]; // unknown type
  return _sqrt(_add(_sqr(get_tabvalue(self, TSTR("x"))), _sqr(get_tabvalue(self, TSTR("y")))));
}

TValue_t __vector_normal(TValue_t *function_arguments) {
  TValue_t self;
  self = function_arguments[0]; // unknown type
  set_tabvalue(self, TSTR("a"), CALL((get_tabvalue(self, TSTR("len"))), ((TValue_t[]){self})));
  idiv_tab(self, TSTR("x"), CALL((get_tabvalue(self, TSTR("len"))), ((TValue_t[]){self})));
  idiv_tab(self, TSTR("y"), CALL((get_tabvalue(self, TSTR("len"))), ((TValue_t[]){self})));
}

TValue_t __preinit() {
  vector = TTAB(make_table(4));
  set_tabvalue(vector, TSTR("__index"), vector);
  set_tabvalue(vector, TSTR("len"), TFUN(__vector_len));
  set_tabvalue(vector, TSTR("normal"), TFUN(__vector_normal));
  set_tabvalue(vector, TSTR("new"), TFUN(__table_func_vector_new));
}
