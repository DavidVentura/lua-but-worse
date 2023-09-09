#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __tmp_and_var_2 = T_NULL;
TValue_t __tmp_and_var_1 = T_NULL;
TValue_t __tmp_and_var_0 = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __tmp_and_var_2 = T_NULL;
  TValue_t gc __tmp_and_var_1 = T_NULL;
  TValue_t gc __tmp_and_var_0 = T_NULL;
  printh(_and(TNUM16(4), TNUM16(5)));
  _set(&__tmp_and_var_0, T_NULL);

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, TNUM16(13));
  }
  printh(__tmp_and_var_0);
  printh(_and(T_FALSE, TNUM16(13)));
  printh(_or(TNUM16(4), TNUM16(5)));
  printh(_or(T_FALSE, TNUM16(5)));
  _set(&__tmp_and_var_1, T_TRUE);

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, _not(T_TRUE));
  }
  printh(__tmp_and_var_1);
  _set(&__tmp_and_var_2, T_TRUE);

  if (_bool(__tmp_and_var_2)) {
    _set(&__tmp_and_var_2, _not(T_FALSE));
  }
  printh(__tmp_and_var_2);
}

TValue_t __preinit() { _grow_strings_to(0); }
