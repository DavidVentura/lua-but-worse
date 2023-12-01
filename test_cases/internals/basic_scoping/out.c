#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __tmp_and_var_0 = T_NULL;
  TValue_t gc a = T_NULL;
  TValue_t gc b = T_NULL;
  _set(&b, TNUM16(1));
  _set(&a, TNUM16(1));
  _set(&__tmp_and_var_0, _add(a, TNUM16(1)));

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, _add(b, TNUM16(1)));
  }

  if (_bool(__tmp_and_var_0)) {
    _set(&a, TNUM16(2));
  }
}

TValue_t __preinit() { _grow_strings_to(0); }
