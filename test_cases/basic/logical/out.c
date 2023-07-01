#include "lua.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __tmp_and_var_10 = T_NULL;
  TValue_t gc __tmp_and_var_11 = T_NULL;
  TValue_t gc __tmp_or_var_2 = T_NULL;
  TValue_t gc __tmp_and_var_9 = T_NULL;
  TValue_t gc __tmp_and_var_7 = T_NULL;
  TValue_t gc __tmp_and_var_8 = T_NULL;
  TValue_t gc __tmp_and_var_5 = T_NULL;
  TValue_t gc __tmp_and_var_6 = T_NULL;
  TValue_t gc __tmp_and_var_4 = T_NULL;
  TValue_t gc __tmp_or_var_1 = T_NULL;
  TValue_t gc __tmp_or_var_0 = T_NULL;
  TValue_t gc __tmp_and_var_3 = T_NULL;
  TValue_t gc __tmp_and_var_2 = T_NULL;
  TValue_t gc __tmp_and_var_0 = T_NULL;
  TValue_t gc __tmp_and_var_1 = T_NULL;
  _set(&__tmp_and_var_1, TNUM16(4));

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, TNUM16(5));
  }
  _set(&__tmp_and_var_0, __tmp_and_var_1);

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, TNUM16(6));
  }
  _set(&a, __tmp_and_var_0); // unknown type
  printh(a);
  _set(&__tmp_and_var_2, T_NULL);

  if (_bool(__tmp_and_var_2)) {
    _set(&__tmp_and_var_2, TNUM16(13));
  }
  _set(&a, __tmp_and_var_2); // unknown type
  printh(a);
  _set(&__tmp_and_var_3, T_FALSE);

  if (_bool(__tmp_and_var_3)) {
    _set(&__tmp_and_var_3, TNUM16(13));
  }
  _set(&a, __tmp_and_var_3); // unknown type
  printh(a);
  _set(&__tmp_or_var_0, TNUM16(4));

  if (_bool(_not(__tmp_or_var_0))) {
    _set(&__tmp_or_var_0, TNUM16(5));
  }
  _set(&a, __tmp_or_var_0); // unknown type
  printh(a);
  _set(&__tmp_or_var_1, T_FALSE);

  if (_bool(_not(__tmp_or_var_1))) {
    _set(&__tmp_or_var_1, TNUM16(5));
  }
  _set(&a, __tmp_or_var_1); // unknown type
  printh(a);
  printh(TSTR("----"));
  _set(&__tmp_and_var_4, TNUM16(5)); // unknown type

  if (_bool(__tmp_and_var_4)) {
    _set(&__tmp_and_var_4, printh(TSTR("visible"))); // unknown type
  }
  _set(&a, __tmp_and_var_4);         // unknown type
  _set(&__tmp_and_var_6, TNUM16(5)); // unknown type

  if (_bool(__tmp_and_var_6)) {
    _set(&__tmp_and_var_6, printh(TSTR("visible2"))); // unknown type
  }
  _set(&__tmp_and_var_5, __tmp_and_var_6); // unknown type

  if (_bool(__tmp_and_var_5)) {
    _set(&__tmp_and_var_5, printh(TSTR("also visible"))); // unknown type
  }
  _set(&a, __tmp_and_var_5);       // unknown type
  _set(&__tmp_and_var_8, T_FALSE); // unknown type

  if (_bool(__tmp_and_var_8)) {
    _set(&__tmp_and_var_8, printh(TSTR("NOT visible"))); // unknown type
  }
  _set(&__tmp_and_var_7, __tmp_and_var_8); // unknown type

  if (_bool(__tmp_and_var_7)) {
    _set(&__tmp_and_var_7, printh(TSTR("also NOT visible"))); // unknown type
  }
  _set(&a, __tmp_and_var_7);       // unknown type
  _set(&__tmp_and_var_9, T_FALSE); // unknown type

  if (_bool(__tmp_and_var_9)) {
    _set(&__tmp_and_var_9, printh(TSTR("AND-NOT VISIBLE"))); // unknown type
  }
  _set(&a, __tmp_and_var_9);     // unknown type
  _set(&__tmp_or_var_2, T_TRUE); // unknown type

  if (_bool(_not(__tmp_or_var_2))) {
    _set(&__tmp_or_var_2, printh(TSTR("OR-NOT VISIBLE"))); // unknown type
  }
  _set(&a, __tmp_or_var_2); // unknown type
  _set(&__tmp_and_var_11, TNUM16(4));

  if (_bool(__tmp_and_var_11)) {
    _set(&__tmp_and_var_11, TNUM16(5));
  }
  _set(&__tmp_and_var_10, __tmp_and_var_11);

  if (_bool(__tmp_and_var_10)) {
    _set(&__tmp_and_var_10, TNUM16(6));
  }

  if (_bool(__tmp_and_var_10)) {
    printh(TSTR("yep"));
  }
}

TValue_t __preinit() {}
