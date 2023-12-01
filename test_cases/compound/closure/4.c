#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_i = T_NULL;
TValue_t __str_newCounter = T_NULL;
TValue_t c2 = T_NULL;
TValue_t c1 = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_0(TVSlice_t function_arguments);
TValue_t __main();
TValue_t newCounter(TVSlice_t function_arguments);
TValue_t test_returning_lambda(TVSlice_t function_arguments);

TValue_t test_returning_lambda(TVSlice_t function_arguments) {
  printh(__str_newCounter);
  _set(&c1, CALL((newCounter), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(CALL((c1), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(CALL((c1), ((TVSlice_t){.elems = NULL, .num = 0})));
  _set(&c2, CALL((newCounter), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(CALL((c2), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(CALL((c1), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(CALL((c2), ((TVSlice_t){.elems = NULL, .num = 0})));
}

TValue_t newCounter(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, TTAB(make_table(1)));
  set_tabvalue(lambda_args, __str_i, TNUM16(0));
  return TCLOSURE(__anonymous_function_0, lambda_args);
}

TValue_t __main() { CALL((test_returning_lambda), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __anonymous_function_0(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(lambda_args, __str_i, _add(get_tabvalue(lambda_args, __str_i), TNUM16(1)));
  return get_tabvalue(lambda_args, __str_i);
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_newCounter, TSTRi(_store_str_at_or_die(CONSTSTR("newCounter"), 1)));
  _set(&__str_i, TSTRi(_store_str_at_or_die(CONSTSTR("i"), 0)));
}
