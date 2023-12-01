#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_double_enclosing_fornum_iterator = T_NULL;
TValue_t whatever_func = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_whatever_func(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_double_enclosing_fornum_iterator(TVSlice_t function_arguments);

TValue_t test_double_enclosing_fornum_iterator(TVSlice_t function_arguments) {
  printh(__str_double_enclosing_fornum_iterator);

  for (TValue_t x = TNUM16(1); __bool(_leq(x, TNUM16(2))); x = _add(x, TNUM16(1))) {
    TValue_t gc lambda_args = T_NULL;
    _set(&lambda_args, TTAB(make_table(1)));
    set_tabvalue(lambda_args, __str_x, x);

    for (TValue_t y = TNUM16(1); __bool(_leq(y, TNUM16(2))); y = _add(y, TNUM16(1))) {
      set_tabvalue(lambda_args, __str_y, y);
      _set(&whatever_func, TCLOSURE(__nested_func_whatever_func, lambda_args));
      CALL((whatever_func), ((TVSlice_t){.elems = NULL, .num = 0}));
    }
  }
}

TValue_t __main() { CALL((test_double_enclosing_fornum_iterator), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __nested_func_whatever_func(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(_mult(get_tabvalue(lambda_args, __str_x), get_tabvalue(lambda_args, __str_y)));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_double_enclosing_fornum_iterator, TSTRi(_store_str_at_or_die(CONSTSTR("double_enclosing fornum iterator"), 2)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
}
