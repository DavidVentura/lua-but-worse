#include "lua.c"
TValue_t e;
TValue_t d;
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t test;
void __preinit();
TValue_t __main();

TValue_t __main() {
  test = T_FALSE;

  if (_bool(test)) {
    return TNUM(fix32_from_int16(5));
  }

  if (_bool(test)) {
    return TNUM(fix32_from_int16(5));
  } else {
    a = TNUM(fix32_from_int16(1));
  }

  if (_bool(test)) {
    return TNUM(fix32_from_int16(5));
  } else {
    b = TNUM(fix32_from_int16(2));
    c = TNUM(fix32_from_int16(3));
  }

  if (!_bool(test)) {
    d = _invert_sign(TNUM(fix32_from_int16(4))); // ?
  } else {
    e = TNUM(fix32_from_int16(0));
  }
  print(a);
  print(b);
  print(c);
  print(d);

  if (_bool(test)) {
  }
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}