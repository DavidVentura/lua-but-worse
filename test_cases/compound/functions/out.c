#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t g = T_NULL;
TValue_t __str_works_inside_a_table__via_hashma = T_NULL;
TValue_t __str_works_inside_a_table = T_NULL;
TValue_t __str_index = T_NULL;
TValue_t __str_f = T_NULL;
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
  set_tabvalue(c, __str_f, TFUN(__anonymous_function_f));
  printh(CALL((get_tabvalue(c, __str_f)), ((TVSlice_t){.elems = NULL, .num = 0})));
  _set(&v, __str_index);
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

TValue_t __anonymous_function_f(TVSlice_t function_arguments) { return __str_works_inside_a_table; }

TValue_t __anonymous_function_v(TVSlice_t function_arguments) { return __str_works_inside_a_table__via_hashma; }

TValue_t __preinit() {
  _grow_strings_to(4);
  _set(&__str_f, TSTRi(_store_str_at_or_die(CONSTSTR("f"), 3)));
  _set(&__str_index, TSTRi(_store_str_at_or_die(CONSTSTR("index"), 2)));
  _set(&__str_works_inside_a_table, TSTRi(_store_str_at_or_die(CONSTSTR("works inside a table"), 1)));
  _set(&__str_works_inside_a_table__via_hashma, TSTRi(_store_str_at_or_die(CONSTSTR("works inside a table, via hashmap"), 0)));
  _set(&g, TFUN(__anonymous_function_g));
}
