#include "pico8.h"
TValue_t s = T_NULL;
TValue_t intermediate = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc intermediate = T_NULL;
  _set(&intermediate, T_NULL);
  for (TValue_t i = TNUM16(1); _lt(i, TNUM16(10)); i = _add(i, TNUM16(1))) {
    _set(&intermediate, tostring(i));           // unknown type
    _set(&s, _concat(TSTR("#"), intermediate)); // unknown type
  }
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __preinit() {}
