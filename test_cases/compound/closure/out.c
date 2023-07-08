#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t captured = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(TVSlice_t function_arguments);
TValue_t __anonymous_function_a(TVSlice_t function_arguments);
TValue_t __anonymous_function_0(TVSlice_t function_arguments);
TValue_t __main();
TValue_t celeste(TVSlice_t function_arguments);

TValue_t celeste(TVSlice_t function_arguments) {
  TValue_t gc tab = T_NULL;
  TValue_t gc var = T_NULL;
  _set(&var, TNUM16(7));
  _set(&tab, TTAB(make_table(3)));
  // Fields for table tab
  set_tabvalue(tab, TNUM16(1), TNUM16(1));
  set_tabvalue(tab, TNUM16(2), TNUM16(2));
  set_tabvalue(tab, TNUM16(3), TNUM16(3));
  foreach (tab, TFUN(__anonymous_function_0))
    ;
}

TValue_t __main() {
  CALL((celeste), ((TVSlice_t){.elems = NULL, .num = 0}));
  _set(&captured, TNUM16(7));
  _set(&a, TFUN(__anonymous_function_a));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  _set(&b, TFUN(__nested_func_b));
  CALL((b), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(CALL((a), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1})));
  return TNUM16(0);
}

TValue_t __anonymous_function_0(TVSlice_t function_arguments) {
  TValue_t gc item = T_NULL;
  _set(&item, __get_array_index_capped(function_arguments, 0));
  printh(_mult(item, var));
}

TValue_t __anonymous_function_a(TVSlice_t function_arguments) {
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  return _mult(x, captured);
}

TValue_t __nested_func_b(TVSlice_t function_arguments) { _set(&captured, TNUM16(1)); }

TValue_t __preinit() { _grow_strings_to(0); }
