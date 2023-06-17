#include "lua.c"
TValue_t b;
TValue_t __preinit();
TValue_t __nested_func_b(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  b = TFUN(__nested_func_b); // unknown type
  CALL((b), ((TValue_t[]){}));
}

TValue_t __nested_func_b(TValue_t *function_arguments) { print(TNUM16(5)); }

TValue_t __preinit() {}
