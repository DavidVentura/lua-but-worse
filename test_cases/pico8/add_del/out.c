#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str__dash__dash__dash_ = T_NULL;
TValue_t __str_another_value = T_NULL;
TValue_t __str_a_value = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t test_del(TVSlice_t function_arguments);
TValue_t test_add(TVSlice_t function_arguments);

TValue_t test_add(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  _set(&t, TTAB(make_table(0)));
  add(t, __str_a_value);
  printh(get_tabvalue(t, TNUM16(1)));
  printh(get_tabvalue(t, TNUM16(2)));
  add(t, __str_another_value);
  printh(get_tabvalue(t, TNUM16(1)));
  printh(get_tabvalue(t, TNUM16(2)));
  printh(get_tabvalue(t, TNUM16(3)));
}

TValue_t test_del(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  _set(&t, TTAB(make_table(0)));
  add(t, __str_a_value);
  add(t, __str_another_value);
  printh(get_tabvalue(t, TNUM16(1)));
  del(t, __str_a_value);
  printh(get_tabvalue(t, TNUM16(1)));
}

TValue_t __main() {
  CALL((test_add), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(__str__dash__dash__dash_);
  CALL((test_del), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_a_value, TSTRi(_store_str_at_or_die(CONSTSTR("a value"), 2)));
  _set(&__str_another_value, TSTRi(_store_str_at_or_die(CONSTSTR("another value"), 1)));
  _set(&__str__dash__dash__dash_, TSTRi(_store_str_at_or_die(CONSTSTR("---"), 0)));
}
