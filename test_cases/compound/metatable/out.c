#include "pico8.h"
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(4)));
  // Fields for table a
  set_tabvalue(a, TSTR("x"), TNUM16(5));
  set_tabvalue(a, TSTR("__index"), a);
  _set(&b, TTAB(make_table(0)));
  setmetatable(b, a);
  printh(get_tabvalue(b, TSTR("x")));
  iadd_tab(b, TSTR("x"), TNUM16(5));
  printh(get_tabvalue(b, TSTR("x")));
  printh(get_tabvalue(b, TSTR("y")));
  set_tabvalue(a, TSTR("y"), TNUM16(7));
  printh(get_tabvalue(b, TSTR("y")));
  return TNUM16(0);
}

TValue_t __preinit() {}
