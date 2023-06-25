#include "pico8.h"
TValue_t adder = T_NULL;
TValue_t empty = T_NULL;
TValue_t member = T_NULL;
TValue_t squares = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&squares, TTAB(make_table(5)));
  // Fields for table squares
  set_tabvalue(squares, TNUM16(1), TNUM16(1));
  set_tabvalue(squares, TNUM16(2), TNUM16(4));
  set_tabvalue(squares, TNUM16(3), TNUM16(9));
  set_tabvalue(squares, TNUM16(4), TNUM16(16));
  set_tabvalue(squares, TNUM16(5), TNUM16(25));
  _set(&member, TTAB(make_table(4)));
  // Fields for table member
  set_tabvalue(member, TSTR("x"), _invert_sign(TNUM(fix32_from_float(1.5f))));
  set_tabvalue(member, TSTR("y"), TNUM16(66));
  _set(&empty, TTAB(make_table(0)));
  printh(get_tabvalue(squares, TNUM16(2)));
  printh(get_tabvalue(member, TSTR("x")));
  printh(get_tabvalue(member, TSTR("y")));
  _set(&adder, TTAB(make_table(4)));
  set_tabvalue(adder, TSTR("a"), TNUM16(1));
  printh(get_tabvalue(adder, TSTR("a")));
  set_tabvalue(adder, TSTR("b"), TNUM16(2));
  printh(get_tabvalue(adder, TSTR("a")));
  printh(get_tabvalue(adder, TSTR("b")));
  set_tabvalue(adder, TSTR("c"), TNUM16(3));
  printh(get_tabvalue(adder, TSTR("a")));
  printh(get_tabvalue(adder, TSTR("b")));
  printh(get_tabvalue(adder, TSTR("c")));
  set_tabvalue(adder, TSTR("d"), TNUM16(4));
  printh(get_tabvalue(adder, TSTR("a")));
  printh(get_tabvalue(adder, TSTR("b")));
  printh(get_tabvalue(adder, TSTR("c")));
  printh(get_tabvalue(adder, TSTR("d")));
}

TValue_t __preinit() {}
