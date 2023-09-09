#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_yep = T_NULL;
TValue_t __str_OR_dash_NOT_VISIBLE = T_NULL;
TValue_t __str_AND_dash_NOT_VISIBLE = T_NULL;
TValue_t __str_also_NOT_visible = T_NULL;
TValue_t __str_NOT_visible = T_NULL;
TValue_t __str_also_visible = T_NULL;
TValue_t __str_visible2 = T_NULL;
TValue_t __str_visible = T_NULL;
TValue_t __str__dash__dash__dash__dash_ = T_NULL;
TValue_t __tmp_and_var_6 = T_NULL;
TValue_t __tmp_or_var_0 = T_NULL;
TValue_t __tmp_and_var_5 = T_NULL;
TValue_t __tmp_and_var_4 = T_NULL;
TValue_t __tmp_and_var_3 = T_NULL;
TValue_t __tmp_and_var_2 = T_NULL;
TValue_t __tmp_and_var_1 = T_NULL;
TValue_t a = T_NULL;
TValue_t __tmp_and_var_0 = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __tmp_and_var_6 = T_NULL;
  TValue_t gc __tmp_or_var_0 = T_NULL;
  TValue_t gc __tmp_and_var_5 = T_NULL;
  TValue_t gc __tmp_and_var_4 = T_NULL;
  TValue_t gc __tmp_and_var_3 = T_NULL;
  TValue_t gc __tmp_and_var_2 = T_NULL;
  TValue_t gc __tmp_and_var_1 = T_NULL;
  TValue_t gc __tmp_and_var_0 = T_NULL;
  _set(&__tmp_and_var_0, _and(TNUM16(4), TNUM16(5)));

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, TNUM16(6));
  }
  _set(&a, __tmp_and_var_0);
  printh(a);
  _set(&__tmp_and_var_1, T_NULL);

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, TNUM16(13));
  }
  _set(&a, __tmp_and_var_1);
  printh(a);
  _set(&a, _and(T_FALSE, TNUM16(13)));
  printh(a);
  _set(&a, _or(TNUM16(4), TNUM16(5)));
  printh(a);
  _set(&a, _or(T_FALSE, TNUM16(5)));
  printh(a);
  printh(__str__dash__dash__dash__dash_);
  _set(&__tmp_and_var_2, TNUM16(5));

  if (_bool(__tmp_and_var_2)) {
    _set(&__tmp_and_var_2, printh(__str_visible));
  }
  _set(&a, __tmp_and_var_2);
  _set(&__tmp_and_var_3, TNUM16(5));

  if (_bool(__tmp_and_var_3)) {
    _set(&__tmp_and_var_3, printh(__str_visible2));
  }
  _set(&__tmp_and_var_0, __tmp_and_var_3);

  if (_bool(__tmp_and_var_0)) {
    _set(&__tmp_and_var_0, printh(__str_also_visible));
  }
  _set(&a, __tmp_and_var_0);
  _set(&__tmp_and_var_4, T_FALSE);

  if (_bool(__tmp_and_var_4)) {
    _set(&__tmp_and_var_4, printh(__str_NOT_visible));
  }
  _set(&__tmp_and_var_1, __tmp_and_var_4);

  if (_bool(__tmp_and_var_1)) {
    _set(&__tmp_and_var_1, printh(__str_also_NOT_visible));
  }
  _set(&a, __tmp_and_var_1);
  _set(&__tmp_and_var_5, T_FALSE);

  if (_bool(__tmp_and_var_5)) {
    _set(&__tmp_and_var_5, printh(__str_AND_dash_NOT_VISIBLE));
  }
  _set(&a, __tmp_and_var_5);
  _set(&__tmp_or_var_0, T_TRUE);

  if (_bool(_not(__tmp_or_var_0))) {
    _set(&__tmp_or_var_0, printh(__str_OR_dash_NOT_VISIBLE));
  }
  _set(&a, __tmp_or_var_0);
  _set(&__tmp_and_var_6, _and(TNUM16(4), TNUM16(5)));

  if (_bool(__tmp_and_var_6)) {
    _set(&__tmp_and_var_6, TNUM16(6));
  }

  if (_bool(__tmp_and_var_6)) {
    printh(__str_yep);
  }
}

TValue_t __preinit() {
  _grow_strings_to(9);
  _set(&__str__dash__dash__dash__dash_, TSTRi(_store_str_at_or_die(CONSTSTR("----"), 8)));
  _set(&__str_visible, TSTRi(_store_str_at_or_die(CONSTSTR("visible"), 7)));
  _set(&__str_visible2, TSTRi(_store_str_at_or_die(CONSTSTR("visible2"), 6)));
  _set(&__str_also_visible, TSTRi(_store_str_at_or_die(CONSTSTR("also visible"), 5)));
  _set(&__str_NOT_visible, TSTRi(_store_str_at_or_die(CONSTSTR("NOT visible"), 4)));
  _set(&__str_also_NOT_visible, TSTRi(_store_str_at_or_die(CONSTSTR("also NOT visible"), 3)));
  _set(&__str_AND_dash_NOT_VISIBLE, TSTRi(_store_str_at_or_die(CONSTSTR("AND-NOT VISIBLE"), 2)));
  _set(&__str_OR_dash_NOT_VISIBLE, TSTRi(_store_str_at_or_die(CONSTSTR("OR-NOT VISIBLE"), 1)));
  _set(&__str_yep, TSTRi(_store_str_at_or_die(CONSTSTR("yep"), 0)));
}
