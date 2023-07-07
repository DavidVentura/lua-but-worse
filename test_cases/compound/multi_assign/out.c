#include "lua.h"
#include "pico8.h"
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(2)));
  set_tabvalue(a, __str_x, TNUM16(5));
  set_tabvalue(a, __str_y, TNUM16(6));
  printh(get_tabvalue(a, __str_x));
  printh(get_tabvalue(a, __str_y));
  _set(&b, TNUM16(1));
  _set(&c, TNUM16(2));
  printh(b);
  printh(c);
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
}
