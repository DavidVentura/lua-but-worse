#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc twos = T_NULL;
  TValue_t gc ones = T_NULL;
  print(TSTR("a"));
  print(_concat(TSTR("a"), TSTR("b")));
  _set(&ones, TSTR("11111111111111111111"));
  _set(&twos, TSTR("22222222222222222222"));
  print(_concat(ones, twos));
  print(_concat(TSTR("a"), TNUM16(5)));
  print(_concat(TSTR(""), TNUM16(5)));
}

TValue_t __preinit() {}
