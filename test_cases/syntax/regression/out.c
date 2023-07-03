#include "lua.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t celeste(TVSlice_t function_arguments);
TValue_t bunny(TVSlice_t function_arguments);
TValue_t bunny2(TVSlice_t function_arguments);
TValue_t bunny3(TVSlice_t function_arguments);

TValue_t bunny3(TVSlice_t function_arguments) {
  TValue_t gc arg = T_NULL;
  TValue_t gc t = T_NULL;
  _set(&t, __get_array_index_capped(function_arguments, 0));
  _set(&..., __get_array_index_capped(function_arguments, 1));
}

TValue_t bunny2(TVSlice_t function_arguments) {

  if (_bool(T_FALSE)) {
    return T_NULL;
  }
  _set(&a, TNUM16(1));
}

TValue_t bunny(TVSlice_t function_arguments) {}

TValue_t celeste(TVSlice_t function_arguments) {

  if (_bool(T_FALSE)) {
    return T_NULL;
  }
}

TValue_t __main() { CALL((celeste), ((TVSlice_t){.elems = NULL, .num = 0})); }

TValue_t __preinit() {}
