#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t c = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, TNUM16(5));
  _set(&b, TNUM16(7));
  _set(&c, _mult(a, b));
  printh(c);
  return TNUM16(0);
}

TValue_t __preinit() { _grow_strings_to(0); }
