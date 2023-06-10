#include "lua.c"
TValue_t c;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t b;
  TValue_t a;
  a = TNUM16(5);
  b = TNUM16(7);
  c = _mult(a, b); // ?
  print(c);
  return TNUM16(0);
}

TValue_t __preinit() {}
