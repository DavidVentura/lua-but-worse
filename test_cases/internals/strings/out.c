#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str_hi2 = T_NULL;
TValue_t __str_2 = T_NULL;
TValue_t __str_hi = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t _inner(TVSlice_t function_arguments);
TValue_t _inner_dyn(TVSlice_t function_arguments);

TValue_t _inner_dyn(TVSlice_t function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&a, __str_hi);
  _set(&b, tostring(TNUM16(3)));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(4));
  _set(&c, _concat(a, b));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(5));
}

TValue_t _inner(TVSlice_t function_arguments) {
  TValue_t gc c = T_NULL;
  TValue_t gc b = T_NULL;
  TValue_t gc a = T_NULL;
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&a, __str_hi);
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&b, __str_2);
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&c, _concat(__str_hi, __str_2));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  _set(&c, __str_hi2);
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __main() {
  CALL((_inner), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
  CALL((_inner_dyn), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(3));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_hi, TSTRi(_store_str_at_or_die(CONSTSTR("hi"), 2)));
  _set(&__str_2, TSTRi(_store_str_at_or_die(CONSTSTR("2"), 1)));
  _set(&__str_hi2, TSTRi(_store_str_at_or_die(CONSTSTR("hi2"), 0)));
}
