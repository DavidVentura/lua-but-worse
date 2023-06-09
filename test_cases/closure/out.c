#include "lua.c"
TValue_t a;
TValue_t captured;
void __preinit();
TValue_t __main();

TValue_t __main() {
  captured = TNUM16(7);
  a = TValue([&](std::vector<TValue> args) -> TValue {
    TValue x = get_with_default(args, 0);
    return _mult(x, captured);
  });
  print(a({TNUM16(5)}));

  void b() { captured = TNUM16(1); }
  b();
  print(a({TNUM16(5)}));
  return TNUM16(0);
}

void __preinit() {}
