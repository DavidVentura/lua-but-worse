#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_rem = T_NULL;
TValue_t __str_fractional_binary_literal = T_NULL;
TValue_t axis = T_NULL;
TValue_t obj = T_NULL;
TValue_t __tmp_or_var_0 = T_NULL;
TValue_t a = T_NULL;
TValue_t __tmp_and_var_1 = T_NULL;
TValue_t __tmp_and_var_0 = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t celeste(TVSlice_t function_arguments);
TValue_t bunny(TVSlice_t function_arguments);
TValue_t bunny2(TVSlice_t function_arguments);
TValue_t integer_div(TVSlice_t function_arguments);
TValue_t peeks(TVSlice_t function_arguments);
TValue_t short_while(TVSlice_t function_arguments);
TValue_t inplace_arith_for_bracket_table_assign(TVSlice_t function_arguments);
TValue_t mod_equal(TVSlice_t function_arguments);
TValue_t fractional_binary_literal(TVSlice_t function_arguments);
TValue_t or_call(TVSlice_t function_arguments);
TValue_t mult_or(TVSlice_t function_arguments);
TValue_t bool_weird(TVSlice_t function_arguments);
TValue_t comment(TVSlice_t function_arguments);

TValue_t comment(TVSlice_t function_arguments) {}

TValue_t bool_weird(TVSlice_t function_arguments) {
  TValue_t gc __tmp_and_var_1 = T_NULL;
  TValue_t gc __tmp_and_var_0 = T_NULL;
  _set(&__tmp_and_var_0, printh(TNUM16(0)));

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, CALL((mult_or), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(1)}, .num = 2})));
  }
  _set(&__tmp_and_var_1, printh(TNUM16(1111)));

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, CALL((mult_or), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(4444), TNUM16(5555)}, .num = 2})));
  }

  if (_bool(__tmp_and_var_0)) {
    _set(&a, TNUM16(1));
  } else if (_bool(__tmp_and_var_1)) {
    _set(&a, TNUM16(1));
  }
}

TValue_t mult_or(TVSlice_t function_arguments) {
  TValue_t gc sw = T_NULL;
  TValue_t gc w = T_NULL;
  _set(&w, TNUM16(2));
  _set(&sw, _mult(_or(w, TNUM16(1)), TNUM16(8)));
}

TValue_t or_call(TVSlice_t function_arguments) {
  TValue_t gc __tmp_or_var_0 = T_NULL;
  TValue_t gc obj = T_NULL;
  _set(&obj, T_FALSE);
  _set(&__tmp_or_var_0, obj);

  if (_bool(_not(__tmp_or_var_0))) {
    _set(&__tmp_or_var_0, T_NULL);
  }
  CALL((__tmp_or_var_0), ((TVSlice_t){.elems = (TValue_t[1]){obj}, .num = 1}));
}

TValue_t fractional_binary_literal(TVSlice_t function_arguments) {
  printh(__str_fractional_binary_literal);
  printh(TNUM(((fix32_t){.i = 32125, .f = 0x8000})));
}

TValue_t mod_equal(TVSlice_t function_arguments) { _modeq(&a, TNUM16(5)); }

TValue_t inplace_arith_for_bracket_table_assign(TVSlice_t function_arguments) {
  _set(&obj, TTAB(make_table(0)));
  _set(&axis, TNUM16(5));
  iadd_tab(get_tabvalue(obj, __str_rem), axis, axis);
}

TValue_t short_while(TVSlice_t function_arguments) {

  while (_bool(T_FALSE)) {
    _set(&a, TNUM16(5));
  }
}

TValue_t peeks(TVSlice_t function_arguments) {}

TValue_t integer_div(TVSlice_t function_arguments) {
  printh(_floor_div(TNUM16(7), TNUM16(5)));
  printh(_floor_div(TNUM16(8), TNUM16(5)));
  printh(_floor_div(TNUM16(11), TNUM16(5)));
}

TValue_t bunny2(TVSlice_t function_arguments) {

  if (_bool(T_FALSE)) {
    return T_NULL;
  }
  _set(&a, TNUM16(1));
}

TValue_t bunny(TVSlice_t function_arguments) {}

TValue_t celeste(TVSlice_t function_arguments) {

  if (_bool(T_FALSE)) {
    return T_NULL;
  }
}

TValue_t __main() {
  CALL((celeste), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((integer_div), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((bunny), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((fractional_binary_literal), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_fractional_binary_literal, TSTRi(_store_str_at_or_die(CONSTSTR("fractional_binary_literal"), 1)));
  _set(&__str_rem, TSTRi(_store_str_at_or_die(CONSTSTR("rem"), 0)));
}
