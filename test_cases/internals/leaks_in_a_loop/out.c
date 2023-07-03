#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t _inner(TVSlice_t function_arguments);

TValue_t _inner(TVSlice_t function_arguments) {
  TValue_t gc s = T_NULL;
  TValue_t gc intermediate = T_NULL;
  _set(&intermediate, T_NULL);
  _set(&s, TSTR("$"));
  _set(&intermediate, TNUM16(5));
  for (TValue_t i = TNUM16(1); __bool(_leq(i, TNUM16(10))); i = _add(i, TNUM16(1))) {
    _set(&intermediate, tostring(i));
    _set(&s, _concat(TSTR("#"), intermediate));
  }
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __main() {
  CALL((_inner), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(1));
}

TValue_t __preinit() {}
