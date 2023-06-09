#include "lua.c"
void __preinit();
TValue_t __main();

TValue_t __main() {
  for (TValue_t i = TNUM(fix32_from_int16(1)); _lt(i, TNUM(fix32_from_int16(7))); i = _add(i, TNUM(fix32_from_int16(1)))) {
    print(i);
  }
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}
