#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  for (TValue_t i = TNUM16(1); _lt(i, TNUM16(7)); i = _add(i, TNUM16(1))) {
    printh(i);
  }
  return TNUM16(0);
}

TValue_t __preinit() {}
