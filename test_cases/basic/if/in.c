#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_c = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t cond(TVSlice_t function_arguments);

TValue_t cond(TVSlice_t function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, __get_array_index_capped(function_arguments, 0));
  _set(&b, __get_array_index_capped(function_arguments, 1));
  _set(&c, __get_array_index_capped(function_arguments, 2));

  if (_bool(a)) {
    printh(__str_a);
  } else if (_bool(b)) {
    printh(__str_b);
  } else {
    printh(__str_c);
  }
}

TValue_t __main() {
  CALL((cond), ((TVSlice_t){.elems = (TValue_t[3]){T_TRUE, T_FALSE, T_FALSE}, .num = 3}));
  CALL((cond), ((TVSlice_t){.elems = (TValue_t[3]){T_FALSE, T_TRUE, T_FALSE}, .num = 3}));
  CALL((cond), ((TVSlice_t){.elems = (TValue_t[3]){T_FALSE, T_FALSE, T_TRUE}, .num = 3}));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 2)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 1)));
  _set(&__str_c, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 0)));
}
