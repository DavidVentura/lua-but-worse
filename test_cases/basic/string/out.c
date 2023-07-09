#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str_ = T_NULL;
TValue_t __str_22222222222222222222 = T_NULL;
TValue_t __str_11111111111111111111 = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __str_test_sub = T_NULL;
TValue_t __str_something = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t test_concat(TVSlice_t function_arguments);
TValue_t test_sub(TVSlice_t function_arguments);

TValue_t test_sub(TVSlice_t function_arguments) {
  TValue_t gc s = T_NULL;
  _set(&s, __str_something);
  printh(__str_test_sub);
  printh(CALL((sub), ((TVSlice_t){.elems = (TValue_t[2]){s, TNUM16(5)}, .num = 2})));
  printh(CALL((sub), ((TVSlice_t){.elems = (TValue_t[2]){s, _invert_sign(TNUM16(5))}, .num = 2})));
  printh(CALL((sub), ((TVSlice_t){.elems = (TValue_t[3]){s, _invert_sign(TNUM16(4)), _invert_sign(TNUM16(3))}, .num = 3})));
  printh(CALL((sub), ((TVSlice_t){.elems = (TValue_t[3]){s, _invert_sign(TNUM16(2)), T_NULL}, .num = 3})));
  printh(CALL((sub), ((TVSlice_t){.elems = (TValue_t[3]){s, TNUM16(8), T_NULL}, .num = 3})));
}

TValue_t test_concat(TVSlice_t function_arguments) {
  TValue_t gc twos = T_NULL;
  TValue_t gc ones = T_NULL;
  printh(__str_a);
  printh(_concat(__str_a, __str_b));
  _set(&ones, __str_11111111111111111111);
  _set(&twos, __str_22222222222222222222);
  printh(_concat(ones, twos));
  printh(_concat(__str_a, TNUM16(5)));
  printh(_concat(__str_, TNUM16(5)));
}

TValue_t __main() {
  CALL((test_sub), ((TVSlice_t){.elems = NULL, .num = 0}));
  CALL((test_concat), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {
  _grow_strings_to(7);
  _set(&__str_something, TSTRi(_store_str_at_or_die(CONSTSTR("something"), 6)));
  _set(&__str_test_sub, TSTRi(_store_str_at_or_die(CONSTSTR("test_sub"), 5)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 4)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 3)));
  _set(&__str_11111111111111111111, TSTRi(_store_str_at_or_die(CONSTSTR("11111111111111111111"), 2)));
  _set(&__str_22222222222222222222, TSTRi(_store_str_at_or_die(CONSTSTR("22222222222222222222"), 1)));
  _set(&__str_, TSTRi(_store_str_at_or_die(CONSTSTR(""), 0)));
}
