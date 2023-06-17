#include "lua.c"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t b;
  TValue_t a;
  a = TTAB(make_table(2));
  // Fields for table a
  set_tabvalue(a, TSTR("x"), TNUM16(1));
  set_tabvalue(a, TSTR("y"), TNUM16(2));
  b = TTAB(make_table(0));
  free_tvalue(a);
  free_tvalue(b);
  set_tabvalue(b, TSTR("a"), TNUM16(1));
}

TValue_t __preinit() {}
