#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_arg = T_NULL;
TValue_t __str_wrapped = T_NULL;
TValue_t __str_not_wrapped = T_NULL;
TValue_t __str_obj = T_NULL;
TValue_t __str_captured = T_NULL;
TValue_t __str_captured_args = T_NULL;
TValue_t __str_newCounter = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __str_enclosing_arg_overlap = T_NULL;
TValue_t __str_i = T_NULL;
TValue_t __str_enclosing_fornum_iterator = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_double_enclosing_fornum_iterator = T_NULL;
TValue_t f2 = T_NULL;
TValue_t wrapped = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t captures_6 = T_NULL;
TValue_t captures_5 = T_NULL;
TValue_t f = T_NULL;
TValue_t c2 = T_NULL;
TValue_t c1 = T_NULL;
TValue_t wrapper = T_NULL;
TValue_t func = T_NULL;
TValue_t whatever_func = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(TVSlice_t function_arguments);
TValue_t __anonymous_function_a(TVSlice_t function_arguments);
TValue_t __nested_func_f2(TVSlice_t function_arguments);
TValue_t __nested_func_f(TVSlice_t function_arguments);
TValue_t __anonymous_function_0(TVSlice_t function_arguments);
TValue_t __nested_func_wrapped(TVSlice_t function_arguments);
TValue_t __nested_func_wrapper(TVSlice_t function_arguments);
TValue_t __nested_func_func(TVSlice_t function_arguments);
TValue_t __nested_func_whatever_func(TVSlice_t function_arguments);
TValue_t __main();
TValue_t test_function_args_captured(TVSlice_t function_arguments);
TValue_t newCounter(TVSlice_t function_arguments);
TValue_t test_returning_lambda(TVSlice_t function_arguments);
TValue_t test_enclosing_table_index(TVSlice_t function_arguments);
TValue_t test_enclosing_fornum_iterator(TVSlice_t function_arguments);
TValue_t test_double_enclosing_fornum_iterator(TVSlice_t function_arguments);

TValue_t test_double_enclosing_fornum_iterator(TVSlice_t function_arguments) {
  printh(__str_double_enclosing_fornum_iterator);

  for (TValue_t _hidden_x = TNUM16(1); __bool(_leq(_hidden_x, TNUM16(2))); _hidden_x = _add(_hidden_x, TNUM16(1))) {
    TValue_t gc lambda_args = T_NULL;
    _set(&lambda_args, TTAB(make_table(1)));
    set_tabvalue(lambda_args, __str_x, _hidden_x);

    for (TValue_t _hidden_y = TNUM16(1); __bool(_leq(_hidden_y, TNUM16(2))); _hidden_y = _add(_hidden_y, TNUM16(1))) {
      TValue_t gc lambda_args = T_NULL;
      _set(&lambda_args, TTAB(make_table(1)));
      set_tabvalue(lambda_args, __str_y, _hidden_y);
      _set(&whatever_func, TCLOSURE(__nested_func_whatever_func, lambda_args));
      CALL((whatever_func), ((TVSlice_t){.elems = NULL, .num = 0}));
    }
  }
}

TValue_t test_enclosing_fornum_iterator(TVSlice_t function_arguments) {
  printh(__str_enclosing_fornum_iterator);

  for (TValue_t _hidden_i = TNUM16(1); __bool(_leq(_hidden_i, TNUM16(2))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc lambda_args = T_NULL;
    _set(&lambda_args, TTAB(make_table(1)));
    set_tabvalue(lambda_args, __str_i, _hidden_i);
    _set(&func, TCLOSURE(__nested_func_func, lambda_args));
    CALL((func), ((TVSlice_t){.elems = NULL, .num = 0}));
  }
}

TValue_t test_enclosing_table_index(TVSlice_t function_arguments) {
  printh(__str_enclosing_arg_overlap);
  _set(&wrapper, TFUN(__nested_func_wrapper));
  CALL((wrapper), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(5), __str_a}, .num = 2}));
}

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

TValue_t test_function_args_captured(TVSlice_t function_arguments) {
  printh(__str_captured_args);
  _set(&f, TFUN(__nested_func_f));
  _set(&captures_5, CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  _set(&captures_6, CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(6)}, .num = 1})));
  CALL((captures_5), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((captures_6), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((captures_5), ((TVSlice_t){.elems = NULL, .num = 0}));
}

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
  CALL((test_returning_lambda), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((test_function_args_captured), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((test_enclosing_table_index), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((test_enclosing_fornum_iterator), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((test_double_enclosing_fornum_iterator), ((TVSlice_t){.elems = NULL, .num = 0}));
  return TNUM16(0);
}

TValue_t __nested_func_whatever_func(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(_mult(get_tabvalue(lambda_args, __str_x), get_tabvalue(lambda_args, __str_y)));
}

TValue_t __nested_func_func(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(lambda_args, __str_i));
}

TValue_t __nested_func_wrapper(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&lambda_args, TTAB(make_table(2)));
  // Fields for table lambda_args
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

TValue_t __anonymous_function_0(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(lambda_args, __str_i, _add(get_tabvalue(lambda_args, __str_i), TNUM16(1)));
  return get_tabvalue(lambda_args, __str_i);
}

TValue_t __nested_func_f(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  TValue_t gc arg = T_NULL;
  _set(&arg, __get_array_index_capped(function_arguments, 0));
  _set(&lambda_args, TTAB(make_table(1)));
  // Fields for table lambda_args
  set_tabvalue(lambda_args, __str_arg, arg);
  _set(&f2, TCLOSURE(__nested_func_f2, lambda_args));
  _incref(f2);
  return f2;
}

TValue_t __nested_func_f2(TVSlice_t function_arguments) {
  TValue_t gc lambda_args = T_NULL;
  _set(&lambda_args, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(lambda_args, __str_arg));
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
  _grow_strings_to(14);
  _set(&__str_double_enclosing_fornum_iterator, TSTRi(_store_str_at_or_die(CONSTSTR("double_enclosing fornum iterator"), 13)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 12)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 11)));
  _set(&__str_enclosing_fornum_iterator, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing fornum iterator"), 10)));
  _set(&__str_i, TSTRi(_store_str_at_or_die(CONSTSTR("i"), 9)));
  _set(&__str_enclosing_arg_overlap, TSTRi(_store_str_at_or_die(CONSTSTR("enclosing arg overlap"), 8)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 7)));
  _set(&__str_newCounter, TSTRi(_store_str_at_or_die(CONSTSTR("newCounter"), 6)));
  _set(&__str_captured_args, TSTRi(_store_str_at_or_die(CONSTSTR("captured args"), 5)));
  _set(&__str_captured, TSTRi(_store_str_at_or_die(CONSTSTR("captured"), 4)));
  _set(&__str_obj, TSTRi(_store_str_at_or_die(CONSTSTR("obj"), 3)));
  _set(&__str_not_wrapped, TSTRi(_store_str_at_or_die(CONSTSTR("not wrapped"), 2)));
  _set(&__str_wrapped, TSTRi(_store_str_at_or_die(CONSTSTR("wrapped"), 1)));
  _set(&__str_arg, TSTRi(_store_str_at_or_die(CONSTSTR("arg"), 0)));
}
