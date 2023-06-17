#include "lua.c"
TValue_t literals;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t __subtable_idk_good_name_prefix_right;
  TValue_t __subtable_idk_good_name_prefix_left;
  TValue_t __subtable_idk_good_name_prefix_bottom;
  TValue_t __subtable_idk_good_name_prefix_top;
  __subtable_idk_good_name_prefix_top = TTAB(make_table(2));
  // Fields for table __subtable_idk_good_name_prefix_top
  set_tabvalue(__subtable_idk_good_name_prefix_top, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_top, TSTR("y"), _invert_sign(TNUM16(68)));
  __subtable_idk_good_name_prefix_bottom = TTAB(make_table(2));
  // Fields for table __subtable_idk_good_name_prefix_bottom
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, TSTR("y"), TNUM16(68));
  __subtable_idk_good_name_prefix_left = TTAB(make_table(2));
  // Fields for table __subtable_idk_good_name_prefix_left
  set_tabvalue(__subtable_idk_good_name_prefix_left, TSTR("x"), _invert_sign(TNUM16(68)));
  set_tabvalue(__subtable_idk_good_name_prefix_left, TSTR("y"), TNUM16(0));
  __subtable_idk_good_name_prefix_right = TTAB(make_table(2));
  // Fields for table __subtable_idk_good_name_prefix_right
  set_tabvalue(__subtable_idk_good_name_prefix_right, TSTR("x"), TNUM16(68));
  set_tabvalue(__subtable_idk_good_name_prefix_right, TSTR("y"), TNUM16(0));
  literals = TTAB(make_table(4));
  // Fields for table literals
  set_tabvalue(literals, TSTR("top"), __subtable_idk_good_name_prefix_top);
  set_tabvalue(literals, TSTR("bottom"), __subtable_idk_good_name_prefix_bottom);
  set_tabvalue(literals, TSTR("left"), __subtable_idk_good_name_prefix_left);
  set_tabvalue(literals, TSTR("right"), __subtable_idk_good_name_prefix_right);
  print(get_tabvalue(get_tabvalue(literals, TSTR("top")), TSTR("y")));
  print(get_tabvalue(get_tabvalue(literals, TSTR("right")), TSTR("x")));
  free_tvalue(__subtable_idk_good_name_prefix_top);
  free_tvalue(__subtable_idk_good_name_prefix_bottom);
  free_tvalue(__subtable_idk_good_name_prefix_left);
  free_tvalue(__subtable_idk_good_name_prefix_right);
  return TNUM16(0);
}

TValue_t __preinit() {}
