#include "lua.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __a_method(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(1)));
  set_tabvalue(a, TSTR("method"), TFUN(__a_method));
  printh(get_tabvalue(a, TSTR("x")));
  CALL((get_tabvalue(a, TSTR("method"))), ((TVSlice_t){.elems = (TValue_t[2]){a, TNUM16(5)}, .num = 2}));
  printh(get_tabvalue(a, TSTR("x")));
}

TValue_t __a_method(TVSlice_t function_arguments) {
  TValue_t gc value = T_NULL;
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  _set(&value, __get_array_index_capped(function_arguments, 1));
  set_tabvalue(self, TSTR("x"), value);
  return T_NULL;
}

TValue_t __preinit() {}
