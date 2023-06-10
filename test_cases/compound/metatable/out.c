#include "lua.c"
TValue_t b;
TValue_t a;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  a = TTAB(make_table(4));
  set_tabvalue(a.table, TSTR("x"), TNUM16(5));
  ;
  set_tabvalue(a.table, TSTR("__index"), a) = a; // ?
  b = TTAB(make_table(4));
  setmetatable(b, a);
  print(get_tabvalue(b.table, TSTR("x")));
  NOT_USED_set_tabvalue(b.table, FIELD_X, TNUM16(5));
  print(get_tabvalue(b.table, TSTR("x")));
  print(get_tabvalue(b.table, TSTR("y")));
  set_tabvalue(a.table, TSTR("y"), TNUM16(7)) = TNUM16(7); // ?
  print(get_tabvalue(b.table, TSTR("y")));
  return TNUM16(0);
}

TValue_t __preinit() {}