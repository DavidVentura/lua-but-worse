#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __preinit();
TValue_t __main();
TValue_t something(TVSlice_t function_arguments);

TValue_t something(TVSlice_t function_arguments) {
  TValue_t gc arg = T_NULL;
  _set(&arg, __get_array_index_capped(function_arguments, 0));
  _incref(arg);
  return arg;
}

TValue_t __main() {
  CALL((something), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(5)}, .num = 1}));
  printh(CALL((something), ((TVSlice_t){.elems = (TValue_t[1]){TNUM16(6)}, .num = 1})));
}

TValue_t __preinit() { _grow_strings_to(0); }
