#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_d = T_NULL;
TValue_t __str_c = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t adder = T_NULL;
TValue_t empty = T_NULL;
TValue_t member = T_NULL;
TValue_t squares = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&squares, TTAB(make_table(5)));
  set_tabvalue(squares, TNUM16(1), TNUM16(1));
  set_tabvalue(squares, TNUM16(2), TNUM16(4));
  set_tabvalue(squares, TNUM16(3), TNUM16(9));
  set_tabvalue(squares, TNUM16(4), TNUM16(16));
  set_tabvalue(squares, TNUM16(5), TNUM16(25));
  _set(&member, TTAB(make_table(4)));
  set_tabvalue(member, __str_x, _invert_sign(TNUM(fix32_from_float(1.5f))));
  set_tabvalue(member, __str_y, TNUM16(66));
  _set(&empty, TTAB(make_table(0)));
  printh(get_tabvalue(squares, TNUM16(2)));
  printh(get_tabvalue(member, __str_x));
  printh(get_tabvalue(member, __str_y));
  _set(&adder, TTAB(make_table(4)));
  set_tabvalue(adder, __str_a, TNUM16(1));
  printh(get_tabvalue(adder, __str_a));
  set_tabvalue(adder, __str_b, TNUM16(2));
  printh(get_tabvalue(adder, __str_a));
  printh(get_tabvalue(adder, __str_b));
  set_tabvalue(adder, __str_c, TNUM16(3));
  printh(get_tabvalue(adder, __str_a));
  printh(get_tabvalue(adder, __str_b));
  printh(get_tabvalue(adder, __str_c));
  set_tabvalue(adder, __str_d, TNUM16(4));
  printh(get_tabvalue(adder, __str_a));
  printh(get_tabvalue(adder, __str_b));
  printh(get_tabvalue(adder, __str_c));
  printh(get_tabvalue(adder, __str_d));
}

TValue_t __preinit() {
  _grow_strings_to(6);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 5)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 4)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 3)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 2)));
  _set(&__str_c, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 1)));
  _set(&__str_d, TSTRi(_store_str_at_or_die(CONSTSTR("d"), 0)));
}
