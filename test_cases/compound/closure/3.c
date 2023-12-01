#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_wrapped = T_NULL;
TValue_t __str_not_wrapped = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_obj = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __str_enclosing_arg_overlap = T_NULL;
TValue_t wrapped = T_NULL;
TValue_t wrapper = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_wrapped(TVSlice_t function_arguments);
TValue_t __nested_func_wrapper(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_enclosing_table_index(TVSlice_t function_arguments);

TValue_t test_enclosing_table_index(TVSlice_t function_arguments) {
  printh(__str_enclosing_arg_overlap);
  _set(&wrapper, TFUN(__nested_func_wrapper));
  CALL((wrapper), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(5), __str_a}, .num = 2}));
}

TValue_t __main() { CALL((test_enclosing_table_index), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __nested_func_wrapper(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&lambda_args, TTAB(make_table(2)));
  set_tabvalue(lambda_args, __str_y, y);
  set_tabvalue(lambda_args, __str_obj, TTAB(make_table(0)));
  set_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_x, __str_not_wrapped);
  set_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_a, __str_wrapped);
  _set(&wrapped, TCLOSURE(__nested_func_wrapped, lambda_args));
  CALL((wrapped), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __nested_func_wrapped(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_x));
  printh(get_tabvalue(get_tabvalue(lambda_args, __str_obj), get_tabvalue(lambda_args, __str_y)));
}

TValue_t __preinit() {
  _grow_strings_to(7);
  _set(&__str_enclosing_arg_overlap, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing arg overlap"), 6)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 5)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 4)));
  _set(&__str_obj, TSTRi(_store_str_at_or_die(CONSTSTR("obj"), 3)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 2)));
  _set(&__str_not_wrapped, TSTRi(_store_str_at_or_die(CONSTSTR("not wrapped"), 1)));
  _set(&__str_wrapped, TSTRi(_store_str_at_or_die(CONSTSTR("wrapped"), 0)));
}
