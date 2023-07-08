#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str_hello = T_NULL;
TValue_t t = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&t, TTAB(make_table(5)));
  // Fields for table t
  set_tabvalue(t, TNUM16(1), TNUM16(1));
  set_tabvalue(t, TNUM16(2), TNUM16(3));
  set_tabvalue(t, TNUM16(3), __str_hello);
  set_tabvalue(t, TNUM16(4), TNUM16(5));
  set_tabvalue(t, TNUM16(5), TNUM16(7));
  foreach (t, printh_lambda)
    ;
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_hello, TSTRi(_store_str_at_or_die(CONSTSTR("hello"), 0)));
}
