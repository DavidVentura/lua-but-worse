#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_x = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_move_y = T_NULL;
TValue_t __str_obj = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_move_y(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_celeste_init_object_captures_obj(TVSlice_t function_arguments);

TValue_t test_celeste_init_object_captures_obj(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, TTAB(make_table(1)));
  set_tabvalue(lambda_args, __str_obj, TTAB(make_table(0)));
  set_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_move_y, TFUN(__anonymous_function_move_y));
}

TValue_t __main() { CALL((test_celeste_init_object_captures_obj), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __anonymous_function_move_y(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc amount = T_NULL;
  _set(&amount, __get_array_index_capped(function_arguments, 0));
  _set(&lambda_args, __get_array_index_capped(function_arguments, 1));
  set_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_y, TNUM16(0));

  for (TValue_t _hidden_i = TNUM16(0); __bool(_leq(_hidden_i, TNUM16(2))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc i = T_NULL;
    _set(&i, _hidden_i);
    set_tabvalue(get_tabvalue(lambda_args, __str_obj), __str_x, TNUM16(0));
  }
}

TValue_t __preinit() {
  _grow_strings_to(4);
  _set(&__str_obj, TSTRi(_store_str_at_or_die(CONSTSTR("obj"), 3)));
  _set(&__str_move_y, TSTRi(_store_str_at_or_die(CONSTSTR("move_y"), 2)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 1)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
}
