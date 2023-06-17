#include "lua.c"
TValue_t t;
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
  TValue_t _anonymous_table_0;
  TValue_t x;
  x = TNUM16(1);
  _pluseq(&x, TNUM16(2));
  a = TNUM16(1);
  b = TNUM16(2);
  c = flr(TNUM(fix32_from_float(1.5f))); // unknown type
  d = flr(TNUM(fix32_from_float(2.5f))); // unknown type
  e = TNUM16(1);
  e = TNUM16(2);
  o = TTAB(make_table(1));
  u = TTAB(make_table(2));
  // Fields for table u
  set_tabvalue(u, TSTR("a"), TNUM16(1));
  set_tabvalue(o, TSTR("a"), get_tabvalue(u, TSTR("a")));
  t = TNUM16(7);
  _anonymous_table_0 = TTAB(make_table(2));
  // Fields for table _anonymous_table_0
  set_tabvalue(_anonymous_table_0, TNUM16(7), TNUM16(8));
  set_tabvalue(_anonymous_table_0, TNUM16(8), TNUM16(7));
  t = get_tabvalue(_anonymous_table_0, t); // unknown type
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print(get_tabvalue(o, TSTR("a")));
  print(t);
  free_tvalue(_anonymous_table_0);
  return TNUM16(0);
}

TValue_t __preinit() {}
