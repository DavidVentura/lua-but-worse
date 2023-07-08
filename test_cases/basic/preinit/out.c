#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() { printh(a); }

TValue_t __preinit() {
  _grow_strings_to(0);
  _set(&a, TNUM16(5));
  _pluseq(&a, TNUM16(1));

  if (_bool(_lt(a, TNUM16(3)))) {
    _set(&a, TNUM16(7));
  }
}
