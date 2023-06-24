#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t _inner(uint8_t argc, TValue_t *function_arguments);

TValue_t _inner(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(0));
  _set(&a, TSTR("hi"));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(1));
  _set(&b, TSTR("2"));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(2));
  _set(&c, _concat(TSTR("hi"), TSTR("2"))); // unknown type
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&c, TSTR("hi2"));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __main() {
  CALL((_inner), ((TValue_t[]){}));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(0));
}

TValue_t __preinit() {}
