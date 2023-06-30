#include "lua.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __a_method(uint8_t argc, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(1)));
  set_tabvalue(a, TSTR("method"), TFUN(__a_method));
  printh(get_tabvalue(a, TSTR("x")));
  CALL((get_tabvalue(a, TSTR("method"))), 2, ((TValue_t[2]){a, TNUM16(5)}));
  printh(get_tabvalue(a, TSTR("x")));
}

TValue_t __a_method(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc value = T_NULL;
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, argc, 0));  // unknown type
  _set(&value, __get_array_index_capped(function_arguments, argc, 1)); // unknown type
  set_tabvalue(self, TSTR("x"), value);
  return T_NULL;
}

TValue_t __preinit() {}
