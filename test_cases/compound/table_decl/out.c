#include "lua.h"
#include "pico8.h"
TValue_t func_in_tab = T_NULL;
TValue_t this = T_NULL;
TValue_t literals = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_func_in_tab_func(TVSlice_t function_arguments);
TValue_t __main();
// Fields for table attr
// Fields for table func_in_tab

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
  CALL((get_tabvalue(func_in_tab, TSTR("func"))), ((TVSlice_t){.elems = (TValue_t[1]){func_in_tab}, .num = 1}));
  return TNUM16(0);
}

TValue_t __anonymous_function_func_in_tab_func(TVSlice_t function_arguments) {
  TValue_t gc this = T_NULL;
  _set(&this, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(this, TSTR("attr"), TTAB(make_table(1)));
  // Fields for table attr
  set_tabvalue(get_tabvalue(this, TSTR("attr")), TSTR("key"), TSTR("value"));
  printh(TSTR("func in tab"));
}

TValue_t __preinit() {
  _set(&this, TTAB(make_table(1)));
  set_tabvalue(this, TSTR("attr"), TTAB(make_table(1)));
  set_tabvalue(get_tabvalue(this, TSTR("attr")), TSTR("key"), TSTR("value"));
  _set(&func_in_tab, TTAB(make_table(2)));
  set_tabvalue(func_in_tab, TSTR("func"), TFUN(__anonymous_function_func_in_tab_func));
}
