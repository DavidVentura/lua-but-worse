#include "lua.c"
TValue_t t;
TValue_t u;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t _anonymous_table_0;
  TValue_t o;
  o = TTAB(make_table(0));
  set_tabvalue(o, TSTR("key"), TSTR("value"));
  u = TTAB(make_table(1));
  // Fields for table u
  set_tabvalue(u, TSTR("a"), TNUM16(1));
  _anonymous_table_0 = TTAB(make_table(2));
  // Fields for table _anonymous_table_0
  set_tabvalue(_anonymous_table_0, TNUM16(7), TNUM16(8));
  set_tabvalue(_anonymous_table_0, TNUM16(8), TNUM16(7));
  t = get_tabvalue(_anonymous_table_0, TNUM16(7)); // unknown type
  print(get_tabvalue(o, TSTR("key")));
  print(get_tabvalue(u, TSTR("a")));
  print(t);
  free_tvalue(o);
  free_tvalue(_anonymous_table_0);
  return TNUM16(0);
}

TValue_t __preinit() {}
