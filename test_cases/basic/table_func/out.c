#include "lua.h"
#include "pico8.h"
TValue_t vector = T_NULL;
TValue_t __preinit();
TValue_t __table_func_vector_new(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc one = T_NULL;
  TValue_t gc zero = T_NULL;
  _set(&zero, CALL((get_tabvalue(vector, TSTR("new"))), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  _set(&one, CALL((get_tabvalue(vector, TSTR("new"))), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(1)}, .num = 2})));
  printh(get_tabvalue(zero, TSTR("x")));
  printh(get_tabvalue(one, TSTR("x")));
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

TValue_t __preinit() {
  _set(&vector, TTAB(make_table(1)));
  set_tabvalue(vector, TSTR("new"), TFUN(__table_func_vector_new));
}
