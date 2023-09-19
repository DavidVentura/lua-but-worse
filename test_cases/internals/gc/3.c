#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();
TValue_t test_references_in_table_destroyed(TVSlice_t function_arguments);
TValue_t _references_in_table_destroyed(TVSlice_t function_arguments);

TValue_t _references_in_table_destroyed(TVSlice_t function_arguments) {
  TValue_t gc tab = T_NULL;
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  _set(&tab, TTAB(make_table(1)));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
  set_tabvalue(tab, TNUM16(0), TTAB(make_table(0)));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(2));
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(2));
}

TValue_t test_references_in_table_destroyed(TVSlice_t function_arguments) {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((_references_in_table_destroyed), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

TValue_t __main() { CALL((test_references_in_table_destroyed), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __preinit() { _grow_strings_to(0); }
