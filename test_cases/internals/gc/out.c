#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_field = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t deleted_tables(TVSlice_t function_arguments);
TValue_t references_in_table_destroyed(TVSlice_t function_arguments);
TValue_t _references_in_table_destroyed(TVSlice_t function_arguments);
TValue_t nested_returned_tables(TVSlice_t function_arguments);
TValue_t _nested_returned_tables(TVSlice_t function_arguments);
TValue_t returned_tables(TVSlice_t function_arguments);
TValue_t returned_tables_1(TVSlice_t function_arguments);

TValue_t returned_tables_1(TVSlice_t function_arguments) {
  TValue_t gc var = T_NULL;
  _set(&var, TTAB(make_table(2)));
  // Fields for table var
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

TValue_t _nested_returned_tables(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  _set(&t, CALL((returned_tables_1), ((TVSlice_t){.elems = NULL, .num = 0})));
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
}

TValue_t nested_returned_tables(TVSlice_t function_arguments) {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((_nested_returned_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

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

TValue_t references_in_table_destroyed(TVSlice_t function_arguments) {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((deleted_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

TValue_t deleted_tables(TVSlice_t function_arguments) {
  TValue_t gc tab = T_NULL;
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  _set(&tab, TTAB(make_table(0)));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));

  for (TValue_t _hidden_i = TNUM16(1); __bool(_leq(_hidden_i, TNUM16(5))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc i = T_NULL;
    _set(&i, _hidden_i);
    set_tabvalue(tab, i, TTAB(make_table(0)));
  }
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(6));

  for (TValue_t _hidden_i = TNUM16(1); __bool(_leq(_hidden_i, TNUM16(5))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc i = T_NULL;
    _set(&i, _hidden_i);
    del(tab, TNUM16(1));
  }
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
}

TValue_t __main() {
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((returned_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(1));
  run_gc();
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((nested_returned_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((references_in_table_destroyed), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
  CALL((deleted_tables), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_tables_used(), TNUM16(0));
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_field, TSTRi(_store_str_at_or_die(CONSTSTR("field"), 0)));
}
