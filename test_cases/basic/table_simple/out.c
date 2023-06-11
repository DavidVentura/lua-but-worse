#include "lua.c"
TValue_t empty;
TValue_t member;
TValue_t squares;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  squares = TTAB(make_table(5));
  // Fields for table squares
  set_tabvalue(squares.table, TNUM16(1), TNUM16(1));
  set_tabvalue(squares.table, TNUM16(2), TNUM16(4));
  set_tabvalue(squares.table, TNUM16(3), TNUM16(9));
  set_tabvalue(squares.table, TNUM16(4), TNUM16(16));
  set_tabvalue(squares.table, TNUM16(5), TNUM16(25));
  member = TTAB(make_table(2));
  // Fields for table member
  set_tabvalue(member.table, TSTR("x"), _invert_sign(TNUM(fix32_from_float(1.5f))));
  set_tabvalue(member.table, TSTR("y"), TNUM16(66));
  empty = TTAB(make_table(0));
  print(get_tabvalue(squares.table, TNUM16(2)));
  print(get_tabvalue(member.table, TSTR("x")));
  print(get_tabvalue(member.table, TSTR("y")));
}

TValue_t __preinit() {}
