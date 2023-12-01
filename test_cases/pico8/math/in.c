#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  printh(_atan2(TNUM16(1), TNUM16(0)));
  printh(_atan2(TNUM16(1), TNUM16(1)));
  printh(_atan2(TNUM16(0), TNUM16(1)));
  printh(_atan2(_invert_sign(TNUM16(1)), TNUM16(1)));
  printh(_atan2(_invert_sign(TNUM16(1)), TNUM16(0)));
  printh(_atan2(_invert_sign(TNUM16(1)), _invert_sign(TNUM16(1))));
  printh(_atan2(TNUM16(0), _invert_sign(TNUM16(1))));
  printh(_atan2(TNUM16(1), _invert_sign(TNUM16(1))));
}

TValue_t __preinit() { _grow_strings_to(0); }
