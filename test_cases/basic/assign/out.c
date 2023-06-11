#include "lua.c"
TValue_t u;
TValue_t o;
TValue_t e;
TValue_t d;
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t x;
  x = TNUM16(1);
  _pluseq(&x, TNUM16(2));
  a = TNUM16(1);
  b = TNUM16(2);
  c = flr(TNUM(fix32_from_float(1.5f))); // unknown type
  d = flr(TNUM(fix32_from_float(2.5f))); // unknown type
  e = TNUM16(1);
  e = TNUM16(2);
  o = TTAB(make_table(0));
  // Fields for table o
  u = TTAB(make_table(1));
  // Fields for table u
  set_tabvalue(u.table, TSTR("a"), TNUM16(123));
  set_tabvalue(o.table, TSTR("a"), TNUM16(333));
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print(get_tabvalue(u.table, TSTR("a")));
  print(get_tabvalue(o.table, TSTR("a")));
  return TNUM16(0);
}

TValue_t __preinit() {}
