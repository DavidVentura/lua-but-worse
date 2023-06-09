#include "lua.c"
TValue_t c;
void __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t b;
  TValue_t a;
  a = TNUM(fix32_from_int16(5));
  b = TNUM(fix32_from_int16(7));
  c = _mult(a, b); // ?
  print(c);
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}
