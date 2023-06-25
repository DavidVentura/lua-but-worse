#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  printh(_and(TNUM16(4), TNUM16(5)));
  printh(_and(T_NULL, TNUM16(13)));
  printh(_and(T_FALSE, TNUM16(13)));
  printh(_or(TNUM16(4), TNUM16(5)));
  printh(_or(T_FALSE, TNUM16(5)));
}

TValue_t __preinit() {}
