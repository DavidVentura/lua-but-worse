#include "pico8.h"
TValue_t t1 = T_NULL;
TValue_t t = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&t, TTAB(make_table(0)));
  printh(getmetatable(t));
  _set(&t1, TTAB(make_table(0)));
  setmetatable(t, t1);
  printh(_equal(getmetatable(t), t1));
}

TValue_t __preinit() {}
