#include "lua.c"
TValue_t a;
void __preinit();
void __main();

void __main() { print(a); }

void __preinit() {
  a = TNUM16(5);
  _pluseq(&a, TNUM16(1));

  if (_bool(_lt(a, TNUM16(3)))) {
    a = TNUM16(7);
  }
}
