#include "pico8.h"
TValue_t b = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(uint8_t argc, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&b, TFUN(__nested_func_b)); // unknown type
  CALL((b), 0, NULL);
}

TValue_t __nested_func_b(uint8_t argc, TValue_t *function_arguments) { print(TNUM16(5)); }

TValue_t __preinit() {}
