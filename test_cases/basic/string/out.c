#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str_ = T_NULL;
TValue_t __str_22222222222222222222 = T_NULL;
TValue_t __str_11111111111111111111 = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
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

TValue_t __preinit() {
  _grow_strings_to(5);
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 4)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 3)));
  _set(&__str_11111111111111111111, TSTRi(_store_str_at_or_die(CONSTSTR("11111111111111111111"), 2)));
  _set(&__str_22222222222222222222, TSTRi(_store_str_at_or_die(CONSTSTR("22222222222222222222"), 1)));
  _set(&__str_, TSTRi(_store_str_at_or_die(CONSTSTR(""), 0)));
}
