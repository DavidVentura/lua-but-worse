#include "lua.h"
#include "pico8.h"
TValue_t func_in_tab = T_NULL;
TValue_t this = T_NULL;
TValue_t __str_func_in_tab = T_NULL;
TValue_t __str_value = T_NULL;
TValue_t __str_key = T_NULL;
TValue_t __str_attr = T_NULL;
TValue_t __str_func = T_NULL;
TValue_t __str_right = T_NULL;
TValue_t __str_left = T_NULL;
TValue_t __str_bottom = T_NULL;
TValue_t __str_top = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t literals = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_func_in_tab_func(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc __subtable_idk_good_name_prefix_right = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_left = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_bottom = T_NULL;
  TValue_t gc __subtable_idk_good_name_prefix_top = T_NULL;
  _set(&__subtable_idk_good_name_prefix_top, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_top
  set_tabvalue(__subtable_idk_good_name_prefix_top, __str_x, TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_top, __str_y, _invert_sign(TNUM16(68)));
  _set(&__subtable_idk_good_name_prefix_bottom, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_bottom
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, __str_x, TNUM16(0));
  set_tabvalue(__subtable_idk_good_name_prefix_bottom, __str_y, TNUM16(68));
  _set(&__subtable_idk_good_name_prefix_left, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_left
  set_tabvalue(__subtable_idk_good_name_prefix_left, __str_x, _invert_sign(TNUM16(68)));
  set_tabvalue(__subtable_idk_good_name_prefix_left, __str_y, TNUM16(0));
  _set(&__subtable_idk_good_name_prefix_right, TTAB(make_table(4)));
  // Fields for table __subtable_idk_good_name_prefix_right
  set_tabvalue(__subtable_idk_good_name_prefix_right, __str_x, TNUM16(68));
  set_tabvalue(__subtable_idk_good_name_prefix_right, __str_y, TNUM16(0));
  _set(&literals, TTAB(make_table(4)));
  // Fields for table literals
  set_tabvalue(literals, __str_top, __subtable_idk_good_name_prefix_top);
  set_tabvalue(literals, __str_bottom, __subtable_idk_good_name_prefix_bottom);
  set_tabvalue(literals, __str_left, __subtable_idk_good_name_prefix_left);
  set_tabvalue(literals, __str_right, __subtable_idk_good_name_prefix_right);
  printh(get_tabvalue(get_tabvalue(literals, __str_top), __str_y));
  printh(get_tabvalue(get_tabvalue(literals, __str_right), __str_x));
  CALL((get_tabvalue(func_in_tab, __str_func)), ((TVSlice_t){.elems = (TValue_t[1]){func_in_tab}, .num = 1}));
  return TNUM16(0);
}

TValue_t __anonymous_function_func_in_tab_func(TVSlice_t function_arguments) {
  TValue_t gc this = T_NULL;
  _set(&this, __get_array_index_capped(function_arguments, 0));
  set_tabvalue(this, __str_attr, TTAB(make_table(1)));
  // Fields for table attr
  set_tabvalue(get_tabvalue(this, __str_attr), __str_key, __str_value);
  printh(__str_func_in_tab);
}

TValue_t __preinit() {
  _grow_strings_to(11);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 10)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 9)));
  _set(&__str_top, TSTRi(_store_str_at_or_die(CONSTSTR("top"), 8)));
  _set(&__str_bottom, TSTRi(_store_str_at_or_die(CONSTSTR("bottom"), 7)));
  _set(&__str_left, TSTRi(_store_str_at_or_die(CONSTSTR("left"), 6)));
  _set(&__str_right, TSTRi(_store_str_at_or_die(CONSTSTR("right"), 5)));
  _set(&__str_func, TSTRi(_store_str_at_or_die(CONSTSTR("func"), 4)));
  _set(&__str_attr, TSTRi(_store_str_at_or_die(CONSTSTR("attr"), 3)));
  _set(&__str_key, TSTRi(_store_str_at_or_die(CONSTSTR("key"), 2)));
  _set(&__str_value, TSTRi(_store_str_at_or_die(CONSTSTR("value"), 1)));
  _set(&__str_func_in_tab, TSTRi(_store_str_at_or_die(CONSTSTR("func in tab"), 0)));
  _set(&this, TTAB(make_table(1)));
  set_tabvalue(this, __str_attr, TTAB(make_table(1)));
  // Fields for table attr
  set_tabvalue(get_tabvalue(this, __str_attr), __str_key, __str_value);
  _set(&func_in_tab, TTAB(make_table(2)));
  // Fields for table func_in_tab
  set_tabvalue(func_in_tab, __str_func, TFUN(__anonymous_function_func_in_tab_func));
}
