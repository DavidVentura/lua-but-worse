#include "pico8.h"
TValue_t literals = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc __subtable_idk_good_name_prefix_right = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_left = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_bottom = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_top = T_NULL;
  _set(&__subtable_idk_good_name_prefix_top, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_top
  set_tabvalue(__subtable_idk_good_name_prefix_top, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_top, TSTR("y"), _invert_sign(TNUM16(68)));
  _set(&__subtable_idk_good_name_prefix_bottom, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_bottom
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, TSTR("x"), TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, TSTR("y"), TNUM16(68));
  _set(&__subtable_idk_good_name_prefix_left, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_left
  set_tabvalue(__subtable_idk_good_name_prefix_left, TSTR("x"), _invert_sign(TNUM16(68)));
  set_tabvalue(__subtable_idk_good_name_prefix_left, TSTR("y"), TNUM16(0));
  _set(&__subtable_idk_good_name_prefix_right, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_right
  set_tabvalue(__subtable_idk_good_name_prefix_right, TSTR("x"), TNUM16(68));
  set_tabvalue(__subtable_idk_good_name_prefix_right, TSTR("y"), TNUM16(0));
  _set(&literals, TTAB(make_table(4)));
  // Fields for table literals
  set_tabvalue(literals, TSTR("top"), __subtable_idk_good_name_prefix_top);
  set_tabvalue(literals, TSTR("bottom"), __subtable_idk_good_name_prefix_bottom);
  set_tabvalue(literals, TSTR("left"), __subtable_idk_good_name_prefix_left);
  set_tabvalue(literals, TSTR("right"), __subtable_idk_good_name_prefix_right);
  printh(get_tabvalue(get_tabvalue(literals, TSTR("top")), TSTR("y")));
  printh(get_tabvalue(get_tabvalue(literals, TSTR("right")), TSTR("x")));
  return TNUM16(0);
}

TValue_t __preinit() {}
