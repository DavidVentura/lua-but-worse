#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {

  for (TValue_t _hidden_i = TNUM16(1); __bool(_leq(_hidden_i, TNUM16(7))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc i = T_NULL;
    _set(&i, _hidden_i);
    printh(i);
  }
  return TNUM16(0);
}

TValue_t __preinit() { _grow_strings_to(0); }
