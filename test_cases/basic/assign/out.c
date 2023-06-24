#include "pico8.h"
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
  set_tabvalue(u, TSTR("a"), TNUM16(123));
  set_tabvalue(o, TSTR("a"), TNUM16(333));
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print(get_tabvalue(u, TSTR("a")));
  print(get_tabvalue(o, TSTR("a")));
  return TNUM16(0);
}

TValue_t __preinit() {}
