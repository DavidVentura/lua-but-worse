#include "lua.c"
TValue_t t1;
TValue_t t;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  t = TTAB(make_table(0));
  print(getmetatable(t));
  t1 = TTAB(make_table(0));
  setmetatable(t, t1);
  print(_equal(getmetatable(t), t1));
}

TValue_t __preinit() {}
