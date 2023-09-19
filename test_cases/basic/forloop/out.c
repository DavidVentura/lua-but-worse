#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {

  for (TValue_t i = TNUM16(1); __bool(_leq(i, TNUM16(7))); i = _add(i, TNUM16(1))) {
    printh(i);
  }
  return TNUM16(0);
}

TValue_t __preinit() { _grow_strings_to(0); }
