#include "lua.h"
#include "pico8.h"
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t celeste(TVSlice_t function_arguments);
TValue_t bunny(TVSlice_t function_arguments);
TValue_t bunny2(TVSlice_t function_arguments);
TValue_t integer_div(TVSlice_t function_arguments);

TValue_t integer_div(TVSlice_t function_arguments) {
  printh(_floor_div(TNUM16(7), TNUM16(5)));
  printh(_floor_div(TNUM16(8), TNUM16(5)));
  printh(_floor_div(TNUM16(11), TNUM16(5)));
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

TValue_t __main() {
  CALL((celeste), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((integer_div), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((bunny), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((bunny), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {}
