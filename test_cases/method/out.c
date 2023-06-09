#include "lua.c"
TValue_t b;
TValue_t a;
void __preinit();
void __main();

void __main() {
  a = new SpecialTable();
  (*(*std::get<SpecialTable *>(a.data))["__index"]) = a; // ?
  method = TValue([&](std::vector<TValue> args) -> TValue {
    TValue self = get_with_default(args, 0);
    (*(*std::get<SpecialTable *>(self.data))["x"]) = TNUM(fix32_from_int16(5)); // ?
    return NUL;
  });
  b = new SpecialTable();
  setmetatable(b, a);
  (*(*std::get<SpecialTable *>(b.data))["method"])({b});
  print((*(*std::get<SpecialTable *>(a.data))["x"]));
  print((*(*std::get<SpecialTable *>(b.data))["x"]));
  (*(*std::get<SpecialTable *>(a.data))["method"])({a});
  print((*(*std::get<SpecialTable *>(a.data))["x"]));
  print((*(*std::get<SpecialTable *>(b.data))["x"]));
}

void __preinit() {}
