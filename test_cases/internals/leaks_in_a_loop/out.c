#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str__hash_ = T_NULL;
TValue_t __str__dollar_ = T_NULL;
TValue_t s = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t _inner(TVSlice_t function_arguments);

TValue_t _inner(TVSlice_t function_arguments) {
  TValue_t gc intermediate = T_NULL;
  TValue_t gc s = T_NULL;
  _set(&s, __str__dollar_);

  for (TValue_t _hidden_i = TNUM16(1); __bool(_leq(_hidden_i, TNUM16(10))); _hidden_i = _add(_hidden_i, TNUM16(1))) {
    TValue_t gc i = T_NULL;
    _set(&i, _hidden_i);
    _set(&s, _concat(__str__hash_, i));
  }
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(4));
}

TValue_t __main() {
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(2));
  CALL((_inner), ((TVSlice_t){.elems = NULL, .num = 0}));
  __internal_debug_assert_eq(__internal_debug_str_used(), TNUM16(2));
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str__dollar_, TSTRi(_store_str_at_or_die(CONSTSTR("$"), 1)));
  _set(&__str__hash_, TSTRi(_store_str_at_or_die(CONSTSTR("#"), 0)));
}
