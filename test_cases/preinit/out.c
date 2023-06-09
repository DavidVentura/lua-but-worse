#include "lua.c"
TValue_t a;
void __preinit();
void __main();

void __main() { print(a); }

void __preinit() {
  a = TNUM(fix32_from_int16(5));
  _pluseq(&a, TNUM(fix32_from_int16(1)));

  if (_bool(_lt(a, TNUM(fix32_from_int16(3))))) {
    a = TNUM(fix32_from_int16(7));
  }
}
