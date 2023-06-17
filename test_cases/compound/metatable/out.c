#include "lua.c"
TValue_t b;
TValue_t a;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  a = TTAB(make_table(4));
  // Fields for table a
  set_tabvalue(a, TSTR("x"), TNUM16(5));
  set_tabvalue(a, TSTR("__index"), a);
  b = TTAB(make_table(0));
  setmetatable(b, a);
  print(get_tabvalue(b, TSTR("x")));
  iadd_tab(b, TSTR("x"), TNUM16(5));
  print(get_tabvalue(b, TSTR("x")));
  print(get_tabvalue(b, TSTR("y")));
  set_tabvalue(a, TSTR("y"), TNUM16(7));
  print(get_tabvalue(b, TSTR("y")));
  return TNUM16(0);
}

TValue_t __preinit() {}
