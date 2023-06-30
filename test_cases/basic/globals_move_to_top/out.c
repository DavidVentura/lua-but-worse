#include "lua.h"
#include "pico8.h"
TValue_t c = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, TNUM16(5));
  _set(&b, TNUM16(7));
  _set(&c, _mult(a, b)); // unknown type
  printh(c);
  return TNUM16(0);
}

TValue_t __preinit() {}
