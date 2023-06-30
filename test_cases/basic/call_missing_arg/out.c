#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t f(uint8_t argc, TValue_t *function_arguments);

TValue_t f(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  _set(&b, __get_array_index_capped(function_arguments, argc, 1)); // unknown type
  _set(&c, __get_array_index_capped(function_arguments, argc, 2)); // unknown type
  printh(a);
  printh(b);
  printh(c);
}

TValue_t __main() {
  printh(TSTR("all"));
  CALL((f), 3, ((TValue_t[3]){TNUM16(1), TNUM16(2), TNUM16(3)}));
  printh(TSTR("2 missing"));
  CALL((f), 1, ((TValue_t[1]){TNUM16(1)}));
  printh(TSTR("all missing"));
  CALL((f), 0, NULL);
}

TValue_t __preinit() {}
