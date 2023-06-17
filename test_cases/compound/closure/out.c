#include "lua.c"
TValue_t b;
TValue_t a;
TValue_t captured;
TValue_t __preinit();
TValue_t __nested_func_b(TValue_t *function_arguments);
TValue_t __anonymous_function_a(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  captured = TNUM16(7);
  a = TFUN(__anonymous_function_a);
  print(CALL((a), ((TValue_t[]){TNUM16(5)})));
  b = TFUN(__nested_func_b); // unknown type
  CALL((b), ((TValue_t[]){}));
  print(CALL((a), ((TValue_t[]){TNUM16(5)})));
  return TNUM16(0);
}

TValue_t __anonymous_function_a(TValue_t *function_arguments) {
  TValue_t x;
  x = function_arguments[0]; // unknown type
  return _mult(x, captured);
}

TValue_t __nested_func_b(TValue_t *function_arguments) { captured = TNUM16(1); }

TValue_t __preinit() {}
