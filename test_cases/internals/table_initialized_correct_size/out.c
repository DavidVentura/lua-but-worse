#include "lua.c"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t c;
  TValue_t b;
  TValue_t a;
  a = TTAB(make_table(1));
  set_tabvalue(a, TSTR("a"), TNUM16(1));
  b = TTAB(make_table(4));
  // Fields for table b
  set_tabvalue(b, TSTR("x"), TNUM16(1));
  set_tabvalue(b, TSTR("y"), TNUM16(2));
  c = TTAB(make_table(5));
  // Fields for table c
  set_tabvalue(c, TSTR("x"), TNUM16(1));
  set_tabvalue(c, TSTR("y"), TNUM16(2));
  set_tabvalue(c, TSTR("a"), TNUM16(1));
  free_tvalue(a);
  free_tvalue(b);
  free_tvalue(c);
}

TValue_t __preinit() {}
