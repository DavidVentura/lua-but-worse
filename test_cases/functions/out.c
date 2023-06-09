#include "lua.c"
TValue_t v;
TValue_t c;
TValue_t b;
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
  b = TValue([&](std::vector<TValue> args) -> TValue {
    TValue x = get_with_default(args, 0);
    TValue y = get_with_default(args, 1);
    return _mult(x, y);
  });
  print(b({TNUM16(5), TNUM16(6)}));
  c = TTAB(make_table(4));
  set_tabvalue(c.table, TSTR("f"), TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table"; })) =
      TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table"; }); // ?
  print(get_tabvalue(c.table, TSTR("f"))({}));
  v = "index";
  set_tabvalue(c.table, v, TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table, via hashmap"; })) =
      TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table, via hashmap"; }); // ?
  print(get_tabvalue(c.table, v)({}));
  return TNUM16(0);
}

void __preinit() {}
