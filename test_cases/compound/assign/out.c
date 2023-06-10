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
  TValue_t x;
  x = TNUM16(1);
  _pluseq(&x, TNUM16(2));
  a = TNUM16(1);
  b = TNUM16(2);
  c = flr(TNUM(fix32_from_float(1.5f))); // ?
  d = flr(TNUM(fix32_from_float(2.5f))); // ?
  e = TNUM16(1);
  e = TNUM16(2);
  o = TTAB(make_table(4));
  u = TTAB(make_table(4));
  set_tabvalue(u.table, TSTR("a"), TNUM16(1));
  ;
  set_tabvalue(o.table, TSTR("a"), get_tabvalue(u.table, TSTR("a")));
  t = TNUM16(7);
  t = get_tabvalue(TTAB(make_table(4)); set_tabvalue(T_IDK.table, TNUM16(7), TNUM16(8)); set_tabvalue(T_IDK.table, TNUM16(8), TNUM16(7));
                       .table, t); // ?
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print(get_tabvalue(o.table, TSTR("a")));
  print(t);
  return TNUM16(0);
}

TValue_t __preinit() {}
