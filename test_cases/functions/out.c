#include "lua.c"
TValue_t v;
TValue_t c;
TValue_t b;
TValue_t a;
TValue_t captured;
void __preinit();
TValue_t __main();

TValue_t __main() {
  captured = TNUM(fix32_from_int16(7));
  a = TValue([&](std::vector<TValue> args) -> TValue {
    TValue x = get_with_default(args, 0);
    return _mult(x, captured);
  });
  print(a({TNUM(fix32_from_int16(5))}));
  b = TValue([&](std::vector<TValue> args) -> TValue {
    TValue x = get_with_default(args, 0);
    TValue y = get_with_default(args, 1);
    return _mult(x, y);
  });
  print(b({TNUM(fix32_from_int16(5)), TNUM(fix32_from_int16(6))}));
  c = new SpecialTable();
  (*(*std::get<SpecialTable *>(c.data))["" f ""]) = TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table"; }); // ?
  print((*(*std::get<SpecialTable *>(c.data))["f"])({}));
  v = "index";
  (*(*std::get<SpecialTable *>(c.data))[v]) =
      TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table, via hashmap"; }); // ?
  print((*(*std::get<SpecialTable *>(c.data))[v])({}));
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}
