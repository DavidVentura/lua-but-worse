#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, TTAB(make_table(1)));
  set_tabvalue(a, __str_a, TNUM16(1));
  _set(&b, TTAB(make_table(4)));
  // Fields for table b
  set_tabvalue(b, __str_x, TNUM16(1));
  set_tabvalue(b, __str_y, TNUM16(2));
  _set(&c, TTAB(make_table(5)));
  // Fields for table c
  set_tabvalue(c, __str_x, TNUM16(1));
  set_tabvalue(c, __str_y, TNUM16(2));
  set_tabvalue(c, __str_a, TNUM16(1));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 2)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
}
