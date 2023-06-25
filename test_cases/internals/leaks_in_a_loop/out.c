#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t _inner(uint8_t argc, TValue_t *function_arguments);

TValue_t _inner(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc s = T_NULL;
  TValue_t gc intermediate = T_NULL;
  _set(&intermediate, T_NULL);
  _set(&s, TSTR("$"));
  _set(&intermediate, TNUM16(5));
  for (TValue_t i = TNUM16(1); _lt(i, TNUM16(10)); i = _add(i, TNUM16(1))) {
    _set(&intermediate, tostring(i));           // unknown type
    _set(&s, _concat(TSTR("#"), intermediate)); // unknown type
  }
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __main() {
  CALL((_inner), 0, NULL);
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(1));
}

TValue_t __preinit() {}
