#include "lua.h"
#include "pico8.h"
TValue_t __str_x = T_NULL;
TValue_t __str_method = T_NULL;
TValue_t __str___index = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __a_method(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(2)));
  set_tabvalue(a, __str___index, a);
  set_tabvalue(a, __str_method, TFUN(__a_method));
  _set(&b, TTAB(make_table(0)));
  setmetatable(b, a);
  CALL((get_tabvalue(b, __str_method)), ((TVSlice_t){.elems = (TValue_t[1]){b}, .num = 1}));
  printh(get_tabvalue(a, __str_x));
  printh(get_tabvalue(b, __str_x));
  CALL((get_tabvalue(a, __str_method)), ((TVSlice_t){.elems = (TValue_t[1]){a}, .num = 1}));
  printh(get_tabvalue(a, __str_x));
  printh(get_tabvalue(b, __str_x));
}

TValue_t __a_method(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(self, __str_x, TNUM16(5));
  return T_NULL;
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str___index, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 2)));
  _set(&__str_method, TSTRi(_store_str_at_or_die(CONSTSTR("method"), 1)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
}
