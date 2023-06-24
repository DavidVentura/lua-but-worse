#include "pico8.h"
TValue_t t = T_NULL;
TValue_t u = T_NULL;
TValue_t o = T_NULL;
TValue_t e = T_NULL;
TValue_t d = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc _anonymous_table_0 = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, TNUM16(1));
  _pluseq(&x, TNUM16(2));
  _set(&a, TNUM16(1));
  _set(&b, TNUM16(2));
  _set(&c, flr(TNUM(fix32_from_float(1.5f)))); // unknown type
  _set(&d, flr(TNUM(fix32_from_float(2.5f)))); // unknown type
  _set(&e, TNUM16(1));
  _set(&e, TNUM16(2));
  _set(&o, TTAB(make_table(1)));
  _set(&u, TTAB(make_table(2)));
  // Fields for table u
  set_tabvalue(u, TSTR("a"), TNUM16(1));
  set_tabvalue(o, TSTR("a"), get_tabvalue(u, TSTR("a")));
  _set(&t, TNUM16(7));
  _set(&_anonymous_table_0, TTAB(make_table(2)));
  // Fields for table _anonymous_table_0
  set_tabvalue(_anonymous_table_0, TNUM16(7), TNUM16(8));
  set_tabvalue(_anonymous_table_0, TNUM16(8), TNUM16(7));
  _set(&t, get_tabvalue(_anonymous_table_0, t)); // unknown type
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print(get_tabvalue(o, TSTR("a")));
  print(t);
  return TNUM16(0);
}

TValue_t __preinit() {}
