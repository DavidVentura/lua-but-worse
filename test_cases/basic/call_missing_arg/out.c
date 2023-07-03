#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t f(TVSlice_t function_arguments);

TValue_t f(TVSlice_t function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, __get_array_index_capped(function_arguments, 0));
  _set(&b, __get_array_index_capped(function_arguments, 1));
  _set(&c, __get_array_index_capped(function_arguments, 2));
  printh(a);
  printh(b);
  printh(c);
}

TValue_t __main() {
  printh(TSTR("all"));
  CALL((f), ((TVSlice_t){.elems = (TValue_t[3]){TNUM16(1), TNUM16(2), TNUM16(3)}, .num = 3}));
  printh(TSTR("2 missing"));
  CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(1)}, .num = 1}));
  printh(TSTR("all missing"));
  CALL((f), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {}
