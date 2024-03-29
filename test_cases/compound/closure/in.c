#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_captured = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(TVSlice_t function_arguments);
TValue_t __anonymous_function_a(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, TTAB(make_table(1)));
  set_tabvalue(lambda_args, __str_captured, TNUM16(7));
  _set(&a, TCLOSURE(__anonymous_function_a, lambda_args));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  set_tabvalue(lambda_args, __str_captured, TNUM16(1));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  _set(&b, TCLOSURE(__nested_func_b, lambda_args));
  CALL((b), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  return TNUM16(0);
}

TValue_t __anonymous_function_a(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&lambda_args, __get_array_index_capped(function_arguments, 1));
  return _mult(x, get_tabvalue(lambda_args, __str_captured));
}

TValue_t __nested_func_b(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(lambda_args, __str_captured, TNUM16(2));
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_captured, TSTRi(_store_str_at_or_die(CONSTSTR("captured"), 0)));
}
