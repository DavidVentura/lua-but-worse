#include "lua.c"
TValue_t v;
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t captured;
TValue_t __preinit();
TValue_t __anonymous_function_v(TValue_t *function_arguments);
TValue_t __anonymous_function_f(TValue_t *function_arguments);
TValue_t __anonymous_function_b(TValue_t *function_arguments);
TValue_t __anonymous_function_a(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  captured = TNUM16(7);
  a = TFUN(__anonymous_function_a);
  print(CALL((a), ((TValue_t[]){TNUM16(5)})));
  b = TFUN(__anonymous_function_b);
  print(CALL((b), ((TValue_t[]){TNUM16(5), TNUM16(6)})));
  c = TTAB(make_table(0));
  set_tabvalue(c, TSTR("f"), TFUN(__anonymous_function_f));
  print(CALL((get_tabvalue(c, TSTR("f"))), ((TValue_t[]){})));
  v = TSTR("index");
  set_tabvalue(c, v, TFUN(__anonymous_function_v));
  print(CALL((get_tabvalue(c, v)), ((TValue_t[]){})));
  return TNUM16(0);
}

TValue_t __anonymous_function_a(TValue_t *function_arguments) {
  TValue_t x;
  x = function_arguments[0]; // unknown type
  return _mult(x, captured);
}

TValue_t __anonymous_function_b(TValue_t *function_arguments) {
  TValue_t y;
  TValue_t x;
  x = function_arguments[0]; // unknown type
  y = function_arguments[1]; // unknown type
  return _mult(x, y);
}

TValue_t __anonymous_function_f(TValue_t *function_arguments) { return TSTR("works inside a table"); }

TValue_t __anonymous_function_v(TValue_t *function_arguments) { return TSTR("works inside a table, via hashmap"); }

TValue_t __preinit() {}
