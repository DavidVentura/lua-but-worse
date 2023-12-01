#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_x = T_NULL;
TValue_t __str_method = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __a_method(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(1)));
  set_tabvalue(a, __str_method, TFUN(__a_method));
  printh(get_tabvalue(a, __str_x));
  CALL((get_tabvalue(a, __str_method)), ((TVSlice_t){.elems = (TValue_t[2]){a, TNUM16(5)}, .num = 2}));
  printh(get_tabvalue(a, __str_x));
}

TValue_t __a_method(TVSlice_t function_arguments) {
  TValue_t gc value = T_NULL;
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  _set(&value, __get_array_index_capped(function_arguments, 1));
  set_tabvalue(self, __str_x, value);
  return T_NULL;
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_method, TSTRi(_store_str_at_or_die(CONSTSTR("method"), 1)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
}
