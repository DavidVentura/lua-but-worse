#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t something(uint8_t argc, TValue_t *function_arguments);

TValue_t something(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc arg = T_NULL;
  _set(&arg, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  return arg;
}

TValue_t __main() {
  CALL((something), 1, ((TValue_t[1]){TNUM16(5)}));
  printh(CALL((something), 1, ((TValue_t[1]){TNUM16(6)})));
}

TValue_t __preinit() {}
