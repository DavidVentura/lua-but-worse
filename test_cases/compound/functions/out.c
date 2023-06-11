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
  print(a.fun((TValue_t[]){TNUM16(5)}));
  b = TFUN(__anonymous_function_b);
  print(b.fun((TValue_t[]){TNUM16(5), TNUM16(6)}));
  c = TTAB(make_table(0));
  set_tabvalue(c.table, TSTR("f"), TFUN(__anonymous_function_f));
  print(get_tabvalue(c.table, TSTR("f")).fun((TValue_t[]){}));
  v = TSTR("index");
  set_tabvalue(c.table, v, TFUN(__anonymous_function_v));
  print(get_tabvalue(c.table, v).fun((TValue_t[]){}));
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
