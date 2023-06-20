#include "lua.c"
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(0));
  a = TSTR("hi");
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(1));
  b = TSTR("2");
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(2));
  c = _concat(TSTR("hi"), TSTR("2")); // unknown type
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  c = TSTR("hi2");
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __preinit() {}
