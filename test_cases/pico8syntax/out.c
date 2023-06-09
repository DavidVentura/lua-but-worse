#include "lua.c"
TValue_t b;
TValue_t a;
void __preinit();
TValue_t __main();

TValue_t __main() {
  a = TNUM(((fix32_t){.i = 0x0, .f = 0x8000}));
  b = TNUM(((fix32_t){.i = 0x0, .f = 0x1000}));
  print(a);
  print(b);
  return TNUM16(0);
}

void __preinit() {}
