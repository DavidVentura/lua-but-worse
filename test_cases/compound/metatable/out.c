#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_y = T_NULL;
TValue_t __str___index = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(4)));
  // Fields for table a
  set_tabvalue(a, __str_x, TNUM16(5));
  set_tabvalue(a, __str___index, a);
  _set(&b, TTAB(make_table(0)));
  setmetatable(b, a);
  printh(get_tabvalue(b, __str_x));
  iadd_tab(b, __str_x, TNUM16(5));
  printh(get_tabvalue(b, __str_x));
  printh(get_tabvalue(b, __str_y));
  set_tabvalue(a, __str_y, TNUM16(7));
  printh(get_tabvalue(b, __str_y));
  return TNUM16(0);
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 2)));
  _set(&__str___index, TSTRi(_store_str_at_or_die(CONSTSTR("__index"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
}
