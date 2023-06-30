#include "lua.h"
#include "pico8.h"
TValue_t e = T_NULL;
TValue_t d = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t test = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&test, T_FALSE);

  if (_bool(test)) {
    return TNUM16(5);
  }

  if (_bool(test)) {
    return TNUM16(5);
  } else {
    _set(&a, TNUM16(1));
  }

  if (_bool(test)) {
    return TNUM16(5);
  } else {
    _set(&b, TNUM16(2));
    _set(&c, TNUM16(3));
  }

  if (!_bool(test)) {
    _set(&d, _invert_sign(TNUM16(4))); // unknown type
  } else {
    _set(&e, TNUM16(0));
  }
  printh(a);
  printh(b);
  printh(c);
  printh(d);

  if (_bool(test)) {
  }
  return TNUM16(0);
}

TValue_t __preinit() {}
