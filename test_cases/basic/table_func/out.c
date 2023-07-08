#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t vector = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_new = T_NULL;
TValue_t __preinit();
TValue_t __table_func_vector_new(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  TValue_t gc one = T_NULL;
  TValue_t gc zero = T_NULL;
  _set(&zero, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(0), TNUM16(0)}, .num = 2})));
  _set(&one, CALL((get_tabvalue(vector, __str_new)), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(1)}, .num = 2})));
  printh(get_tabvalue(zero, __str_x));
  printh(get_tabvalue(one, __str_x));
}

TValue_t __table_func_vector_new(TVSlice_t function_arguments) {
  TValue_t gc o = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&o, TTAB(make_table(4)));
  // Fields for table o
  set_tabvalue(o, __str_x, x);
  set_tabvalue(o, __str_y, y);
  setmetatable(o, vector);
  _incref(o);
  return o;
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_new, TSTRi(_store_str_at_or_die(CONSTSTR("new"), 2)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
  _set(&vector, TTAB(make_table(1)));
  set_tabvalue(vector, __str_new, TFUN(__table_func_vector_new));
}
