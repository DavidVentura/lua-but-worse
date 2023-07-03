#include "lua.h"
#include "pico8.h"
TValue_t g = T_NULL;
TValue_t v = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t captured = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_v(TVSlice_t function_arguments);
TValue_t __anonymous_function_f(TVSlice_t function_arguments);
TValue_t __anonymous_function_b(TVSlice_t function_arguments);
TValue_t __anonymous_function_a(TVSlice_t function_arguments);
TValue_t __anonymous_function_g(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&captured, TNUM16(7));
  _set(&a, TFUN(__anonymous_function_a));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  _set(&b, TFUN(__anonymous_function_b));
  printh(CALL((b), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(5), TNUM16(6)}, .num = 2})));
  _set(&c, TTAB(make_table(2)));
  set_tabvalue(c, TSTR("f"), TFUN(__anonymous_function_f));
  printh(CALL((get_tabvalue(c, TSTR("f"))), ((TVSlice_t){.elems = NULL, .num = 0})));
  _set(&v, TSTR("index"));
  set_tabvalue(c, v, TFUN(__anonymous_function_v));
  printh(CALL((get_tabvalue(c, v)), ((TVSlice_t){.elems = NULL, .num = 0})));
  CALL((g), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(55)}, .num = 1}));
  return TNUM16(0);
}

TValue_t __anonymous_function_g(TVSlice_t function_arguments) {
  TValue_t gc par = T_NULL;
  _set(&par, __get_array_index_capped(function_arguments, 0));
  printh(par);
}

TValue_t __anonymous_function_a(TVSlice_t function_arguments) {
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  return _mult(x, captured);
}

TValue_t __anonymous_function_b(TVSlice_t function_arguments) {
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  return _mult(x, y);
}

TValue_t __anonymous_function_f(TVSlice_t function_arguments) { return TSTR("works inside a table"); }

TValue_t __anonymous_function_v(TVSlice_t function_arguments) { return TSTR("works inside a table, via hashmap"); }

TValue_t __preinit() { _set(&g, TFUN(__anonymous_function_g)); }
