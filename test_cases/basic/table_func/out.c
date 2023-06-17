#include "lua.c"
TValue_t vector;
TValue_t __preinit();
TValue_t __table_func_vector_new(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t one;
  TValue_t zero;
  zero = CALL((get_tabvalue(vector, TSTR("new"))), ((TValue_t[]){TNUM16(0), TNUM16(0)})); // unknown type
  one = CALL((get_tabvalue(vector, TSTR("new"))), ((TValue_t[]){TNUM16(1), TNUM16(1)}));  // unknown type
  print(get_tabvalue(zero, TSTR("x")));
  print(get_tabvalue(one, TSTR("x")));
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

TValue_t __preinit() {
  vector = TTAB(make_table(1));
  set_tabvalue(vector, TSTR("new"), TFUN(__table_func_vector_new));
}
