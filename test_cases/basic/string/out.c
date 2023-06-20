#include "lua.c"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t twos;
  TValue_t ones;
  print(TSTR("a"));
  print(_concat(TSTR("a"), TSTR("b")));
  ones = TSTR("11111111111111111111");
  twos = TSTR("22222222222222222222");
  print(_concat(ones, twos));
}

TValue_t __preinit() {}
