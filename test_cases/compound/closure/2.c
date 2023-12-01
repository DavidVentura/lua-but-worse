#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_i = T_NULL;
TValue_t __str_enclosing_fornum_iterator = T_NULL;
TValue_t func = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_func(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_enclosing_fornum_iterator(TVSlice_t function_arguments);

TValue_t test_enclosing_fornum_iterator(TVSlice_t function_arguments) {
  printh(__str_enclosing_fornum_iterator);

  for (TValue_t i = TNUM16(1); __bool(_leq(i, TNUM16(2))); i = _add(i, TNUM16(1))) {
    TValue_t gc lambda_args = T_NULL;
    _set(&lambda_args, TTAB(make_table(1)));
    set_tabvalue(lambda_args, __str_i, i);
    _set(&func, TCLOSURE(__nested_func_func, lambda_args));
    CALL((func), ((TVSlice_t){.elems = NULL, .num = 0}));
  }
}

TValue_t __main() { CALL((test_enclosing_fornum_iterator), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __nested_func_func(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(lambda_args, __str_i));
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_enclosing_fornum_iterator, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing fornum iterator"), 1)));
  _set(&__str_i, TSTRi(_store_str_at_or_die(CONSTSTR("i"), 0)));
}
