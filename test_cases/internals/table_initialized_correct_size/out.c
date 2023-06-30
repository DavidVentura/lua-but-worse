#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, TTAB(make_table(1)));
  set_tabvalue(a, TSTR("a"), TNUM16(1));
  _set(&b, TTAB(make_table(4)));
  // Fields for table b
  set_tabvalue(b, TSTR("x"), TNUM16(1));
  set_tabvalue(b, TSTR("y"), TNUM16(2));
  _set(&c, TTAB(make_table(5)));
  // Fields for table c
  set_tabvalue(c, TSTR("x"), TNUM16(1));
  set_tabvalue(c, TSTR("y"), TNUM16(2));
  set_tabvalue(c, TSTR("a"), TNUM16(1));
}

TValue_t __preinit() {}
