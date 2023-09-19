#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();
TValue_t deleted_tables(TVSlice_t function_arguments);

TValue_t deleted_tables(TVSlice_t function_arguments) {
  TValue_t gc tab = T_NULL;
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  _set(&tab, TTAB(make_table(0)));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));

  for (TValue_t i = TNUM16(1); __bool(_leq(i, TNUM16(5))); i = _add(i, TNUM16(1))) {
    set_tabvalue(tab, i, TTAB(make_table(0)));
  }
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(6));

  for (TValue_t i = TNUM16(1); __bool(_leq(i, TNUM16(5))); i = _add(i, TNUM16(1))) {
    del(tab, TNUM16(1));
  }
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
}

TValue_t __main() {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((deleted_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

TValue_t __preinit() { _grow_strings_to(0); }
