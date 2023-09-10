#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_field = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t returned_tables_2(TVSlice_t function_arguments);
TValue_t returned_tables_1(TVSlice_t function_arguments);

TValue_t returned_tables_1(TVSlice_t function_arguments) {
  TValue_t gc var = T_NULL;
  _set(&var, TTAB(make_table(2)));
  // Fields for table var
  set_tabvalue(var, __str_field, TNUM16(1));
  _mark_for_gc(var);
  return var;
}

TValue_t returned_tables_2(TVSlice_t function_arguments) {
  TValue_t gc _anon_return_table = T_NULL;
  _set(&_anon_return_table, TTAB(make_table(2)));
  // Fields for returned table
  set_tabvalue(_anon_return_table, __str_field, TNUM16(1));
  _mark_for_gc(_anon_return_table);
  return _anon_return_table;
}

TValue_t __main() {
  TValue_t gc t2 = T_NULL;
  TValue_t gc t = T_NULL;
  _set(&t, CALL((returned_tables_1), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(get_tabvalue(t, __str_field));
  _set(&t2, CALL((returned_tables_2), ((TVSlice_t){.elems = NULL, .num = 0})));
  printh(get_tabvalue(t2, __str_field));
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_field, TSTRi(_store_str_at_or_die(CONSTSTR("field"), 0)));
}
