#include "pico8.h"
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t captured = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(uint8_t argc, TValue_t *function_arguments);
TValue_t __anonymous_function_a(uint8_t argc, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&captured, TNUM16(7));
  _set(&a, TFUN(__anonymous_function_a));
  print(CALL((a), 1, ((TValue_t[1]){TNUM16(5)})));
  _set(&b, TFUN(__nested_func_b)); // unknown type
  CALL((b), 0, NULL);
  print(CALL((a), 1, ((TValue_t[1]){TNUM16(5)})));
  return TNUM16(0);
}

TValue_t __anonymous_function_a(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  return _mult(x, captured);
}

TValue_t __nested_func_b(uint8_t argc, TValue_t *function_arguments) { _set(&captured, TNUM16(1)); }

TValue_t __preinit() {}
