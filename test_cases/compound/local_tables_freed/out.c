#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_a = T_NULL;
TValue_t __str_value = T_NULL;
TValue_t __str_key = T_NULL;
TValue_t t = T_NULL;
TValue_t u = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc _anonymous_table_0 = T_NULL;
  TValue_t gc o = T_NULL;
  _set(&o, TTAB(make_table(1)));
  set_tabvalue(o, __str_key, __str_value);
  _set(&u, TTAB(make_table(2)));
  // Fields for table u
  set_tabvalue(u, __str_a, TNUM16(1));
  _set(&_anonymous_table_0, TTAB(make_table(2)));
  // Fields for table _anonymous_table_0
  set_tabvalue(_anonymous_table_0, TNUM16(7), TNUM16(8));
  set_tabvalue(_anonymous_table_0, TNUM16(8), TNUM16(7));
  _set(&t, get_tabvalue(_anonymous_table_0, TNUM16(7)));
  printh(get_tabvalue(o, __str_key));
  printh(get_tabvalue(u, __str_a));
  printh(t);
  return TNUM16(0);
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_key, TSTRi(_store_str_at_or_die(CONSTSTR("key"), 2)));
  _set(&__str_value, TSTRi(_store_str_at_or_die(CONSTSTR("value"), 1)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));
}
