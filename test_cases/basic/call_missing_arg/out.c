#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str_all_missing = T_NULL;
TValue_t __str_2_missing = T_NULL;
TValue_t __str_all = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t f(TVSlice_t function_arguments);

TValue_t f(TVSlice_t function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, __get_array_index_capped(function_arguments, 0));
  _set(&b, __get_array_index_capped(function_arguments, 1));
  _set(&c, __get_array_index_capped(function_arguments, 2));
  printh(a);
  printh(b);
  printh(c);
}

TValue_t __main() {
  printh(__str_all);
  CALL((f), ((TVSlice_t){.elems = (TValue_t[3]){TNUM16(1), TNUM16(2), TNUM16(3)}, .num = 3}));
  printh(__str_2_missing);
  CALL((f), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(1)}, .num = 1}));
  printh(__str_all_missing);
  CALL((f), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_all, TSTRi(_store_str_at_or_die(CONSTSTR("all"), 2)));
  _set(&__str_2_missing, TSTRi(_store_str_at_or_die(CONSTSTR("2 missing"), 1)));
  _set(&__str_all_missing, TSTRi(_store_str_at_or_die(CONSTSTR("all missing"), 0)));
}
