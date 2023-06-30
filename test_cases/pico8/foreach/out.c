#include "lua.h"
#include "pico8.h"
TValue_t t = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&t, TTAB(make_table(5)));
  // Fields for table t
  set_tabvalue(t, TNUM16(1), TNUM16(1));
  set_tabvalue(t, TNUM16(2), TNUM16(3));
  set_tabvalue(t, TNUM16(3), TSTR("hello"));
  set_tabvalue(t, TNUM16(4), TNUM16(5));
  set_tabvalue(t, TNUM16(5), TNUM16(7));
  foreach (t, _printh)
    ;
}

TValue_t __preinit() {}
