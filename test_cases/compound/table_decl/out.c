#include "lua.c"
TValue_t literals;
TValue_t __subtable_literals_right;
TValue_t __subtable_literals_left;
TValue_t __subtable_literals_bottom;
TValue_t __subtable_literals_top;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  __subtable_literals_top = TTAB(make_table(2));
  set_tabvalue(__subtable_literals_right.table, TSTR("x"), TNUM16(68));
  set_tabvalue(__subtable_literals_right.table, TSTR("y"), TNUM16(0));
  set_tabvalue(__subtable_literals_left.table, TSTR("x"), _invert_sign(TNUM16(68)));
  set_tabvalue(__subtable_literals_left.table, TSTR("y"), TNUM16(0));
  set_tabvalue(__subtable_literals_bottom.table, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_literals_bottom.table, TSTR("y"), TNUM16(68));
  set_tabvalue(__subtable_literals_top.table, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_literals_top.table, TSTR("y"), _invert_sign(TNUM16(68)));
  __subtable_literals_bottom = TTAB(make_table(2));
  __subtable_literals_left = TTAB(make_table(2));
  __subtable_literals_right = TTAB(make_table(2));
  literals = TTAB(make_table(4));
  set_tabvalue(literals.table, TSTR("top"), __subtable_literals_top);
  set_tabvalue(literals.table, TSTR("bottom"), __subtable_literals_bottom);
  set_tabvalue(literals.table, TSTR("left"), __subtable_literals_left);
  set_tabvalue(literals.table, TSTR("right"), __subtable_literals_right);
  print(get_tabvalue(get_tabvalue(literals.table, TSTR("top")).table, TSTR("y")));
  print(get_tabvalue(get_tabvalue(literals.table, TSTR("right")).table, TSTR("x")));
}

TValue_t __preinit() {}
