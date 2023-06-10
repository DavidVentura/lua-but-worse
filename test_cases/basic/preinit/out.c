#include "lua.c"
TValue_t a;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() { print(a); }

TValue_t __preinit() {
  a = TNUM16(5);
  _pluseq(&a, TNUM16(1));

  if (_bool(_lt(a, TNUM16(3)))) {
    a = TNUM16(7);
  }
}
