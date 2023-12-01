#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_arg = T_NULL;
TValue_t __str_captured_args = T_NULL;
TValue_t f2 = T_NULL;
TValue_t captures_6 = T_NULL;
TValue_t captures_5 = T_NULL;
TValue_t f = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_f2(TVSlice_t function_arguments);
TValue_t __nested_func_f(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_function_args_captured(TVSlice_t function_arguments);

TValue_t test_function_args_captured(TVSlice_t function_arguments) {
  printh(__str_captured_args);
  _set(&f, TFUN(__nested_func_f));
  _set(&captures_5, CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  _set(&captures_6, CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(6)}, .num = 1})));
  CALL((captures_5), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((captures_6), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((captures_5), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __main() { CALL((test_function_args_captured), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __nested_func_f(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc arg = T_NULL;
  _set(&arg, __get_array_index_capped(function_arguments, 0));
  _set(&lambda_args, TTAB(make_table(1)));
  set_tabvalue(lambda_args, __str_arg, arg);
  _set(&f2, TCLOSURE(__nested_func_f2, lambda_args));
  _mark_for_gc(f2);
  return f2;
}

TValue_t __nested_func_f2(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(lambda_args, __str_arg));
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_captured_args, TSTRi(_store_str_at_or_die(CONSTSTR("captured args"), 1)));
  _set(&__str_arg, TSTRi(_store_str_at_or_die(CONSTSTR("arg"), 0)));
}
