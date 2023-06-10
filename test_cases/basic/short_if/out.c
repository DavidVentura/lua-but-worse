#include "lua.c"
TValue_t e;
TValue_t d;
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t test;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  test = T_FALSE;

  if (_bool(test)) {
    return TNUM16(5);
  }

  if (_bool(test)) {
    return TNUM16(5);
  } else {
    a = TNUM16(1);
  }

  if (_bool(test)) {
    return TNUM16(5);
  } else {
    b = TNUM16(2);
    c = TNUM16(3);
  }

  if (!_bool(test)) {
    d = _invert_sign(TNUM16(4)); // unknown type
  } else {
    e = TNUM16(0);
  }
  print(a);
  print(b);
  print(c);
  print(d);

  if (_bool(test)) {
  }
  return TNUM16(0);
}

TValue_t __preinit() {}
