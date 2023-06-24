#include "pico8.h"
TValue_t t = T_NULL;
TValue_t u = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc _anonymous_table_0 = T_NULL;
  TValue_t gc o = T_NULL;
  _set(&o, TTAB(make_table(1)));
  set_tabvalue(o, TSTR("key"), TSTR("value"));
  _set(&u, TTAB(make_table(2)));
  // Fields for table u
  set_tabvalue(u, TSTR("a"), TNUM16(1));
  _set(&_anonymous_table_0, TTAB(make_table(2)));
  // Fields for table _anonymous_table_0
  set_tabvalue(_anonymous_table_0, TNUM16(7), TNUM16(8));
  set_tabvalue(_anonymous_table_0, TNUM16(8), TNUM16(7));
  _set(&t, get_tabvalue(_anonymous_table_0, TNUM16(7))); // unknown type
  print(get_tabvalue(o, TSTR("key")));
  print(get_tabvalue(u, TSTR("a")));
  print(t);
  return TNUM16(0);
}

TValue_t __preinit() {}
