#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&a, TNUM(((fix32_t){.i = 0, .f = 0x8000})));
  _set(&b, TNUM(((fix32_t){.i = 0, .f = 0x1000})));
  printh(a);
  printh(b);
  return TNUM16(0);
}

TValue_t __preinit() { _grow_strings_to(0); }
