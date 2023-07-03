#include "lua.h"
#include "pico8.h"
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __a_method(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(2)));
  set_tabvalue(a, TSTR("__index"), a);
  set_tabvalue(a, TSTR("method"), TFUN(__a_method));
  _set(&b, TTAB(make_table(0)));
  setmetatable(b, a);
  CALL((get_tabvalue(b, TSTR("method"))), ((TVSlice_t){.elems = (TValue_t[1]){b}, .num = 1}));
  printh(get_tabvalue(a, TSTR("x")));
  printh(get_tabvalue(b, TSTR("x")));
  CALL((get_tabvalue(a, TSTR("method"))), ((TVSlice_t){.elems = (TValue_t[1]){a}, .num = 1}));
  printh(get_tabvalue(a, TSTR("x")));
  printh(get_tabvalue(b, TSTR("x")));
}

TValue_t __a_method(TVSlice_t function_arguments) {
  TValue_t gc self = T_NULL;
  _set(&self, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(self, TSTR("x"), TNUM16(5));
  return T_NULL;
}

TValue_t __preinit() {}
