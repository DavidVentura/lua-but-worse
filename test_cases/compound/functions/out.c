#include "pico8.h"
TValue_t v = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t captured = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_v(uint8_t argc, TValue_t *function_arguments);
TValue_t __anonymous_function_f(uint8_t argc, TValue_t *function_arguments);
TValue_t __anonymous_function_b(uint8_t argc, TValue_t *function_arguments);
TValue_t __anonymous_function_a(uint8_t argc, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&captured, TNUM16(7));
  _set(&a, TFUN(__anonymous_function_a));
  print(CALL((a), 1, ((TValue_t[1]){TNUM16(5)})));
  _set(&b, TFUN(__anonymous_function_b));
  print(CALL((b), 2, ((TValue_t[2]){TNUM16(5), TNUM16(6)})));
  _set(&c, TTAB(make_table(2)));
  set_tabvalue(c, TSTR("f"), TFUN(__anonymous_function_f));
  print(CALL((get_tabvalue(c, TSTR("f"))), 0, NULL));
  _set(&v, TSTR("index"));
  set_tabvalue(c, v, TFUN(__anonymous_function_v));
  print(CALL((get_tabvalue(c, v)), 0, NULL));
  return TNUM16(0);
}

TValue_t __anonymous_function_a(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  return _mult(x, captured);
}

TValue_t __anonymous_function_b(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, argc, 0)); // unknown type
  _set(&y, __get_array_index_capped(function_arguments, argc, 1)); // unknown type
  return _mult(x, y);
}

TValue_t __anonymous_function_f(uint8_t argc, TValue_t *function_arguments) { return TSTR("works inside a table"); }

TValue_t __anonymous_function_v(uint8_t argc, TValue_t *function_arguments) { return TSTR("works inside a table, via hashmap"); }

TValue_t __preinit() {}
