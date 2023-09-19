#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_field = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t returned_tables(TVSlice_t function_arguments);
TValue_t returned_tables_1(TVSlice_t function_arguments);

TValue_t returned_tables_1(TVSlice_t function_arguments) {
  TValue_t gc var = T_NULL;
  _set(&var, TTAB(make_table(2)));
  set_tabvalue(var, __str_field, TNUM16(1));
  _mark_for_gc(var);
  return var;
}

TValue_t returned_tables(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  _set(&t, CALL((returned_tables_1), ((TVSlice_t){.elems = NULL, .num = 0})));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
}

TValue_t __main() {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((returned_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_field, TSTRi(_store_str_at_or_die(CONSTSTR("field"), 0)));
}
