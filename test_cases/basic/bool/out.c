#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __tmp_and_var_4 = T_NULL;
  TValue_t gc __tmp_and_var_3 = T_NULL;
  TValue_t gc __tmp_or_var_1 = T_NULL;
  TValue_t gc __tmp_or_var_0 = T_NULL;
  TValue_t gc __tmp_and_var_2 = T_NULL;
  TValue_t gc __tmp_and_var_1 = T_NULL;
  TValue_t gc __tmp_and_var_0 = T_NULL;
  _set(&__tmp_and_var_0, TNUM16(4));

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, TNUM16(5));
  }
  printh(__tmp_and_var_0);
  _set(&__tmp_and_var_1, T_NULL);

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, TNUM16(13));
  }
  printh(__tmp_and_var_1);
  _set(&__tmp_and_var_2, T_FALSE);

  if (_bool(__tmp_and_var_2)) {
    _set(&__tmp_and_var_2, TNUM16(13));
  }
  printh(__tmp_and_var_2);
  _set(&__tmp_or_var_0, TNUM16(4));

  if (_bool(_not(__tmp_or_var_0))) {
    _set(&__tmp_or_var_0, TNUM16(5));
  }
  printh(__tmp_or_var_0);
  _set(&__tmp_or_var_1, T_FALSE);

  if (_bool(_not(__tmp_or_var_1))) {
    _set(&__tmp_or_var_1, TNUM16(5));
  }
  printh(__tmp_or_var_1);
  _set(&__tmp_and_var_3, T_TRUE);

  if (_bool(__tmp_and_var_3)) {
    _set(&__tmp_and_var_3, _not(T_TRUE));
  }
  printh(__tmp_and_var_3);
  _set(&__tmp_and_var_4, T_TRUE);

  if (_bool(__tmp_and_var_4)) {
    _set(&__tmp_and_var_4, _not(T_FALSE));
  }
  printh(__tmp_and_var_4);
}

TValue_t __preinit() { _grow_strings_to(0); }
