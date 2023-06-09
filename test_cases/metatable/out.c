#include "lua.c"
TValue_t b;
TValue_t a;
void __preinit();
TValue_t __main();

TValue_t __main() {
  a = new SpecialTable({{"x", new TValue(TNUM(fix32_from_int16(5)))}});
  (*(*std::get<SpecialTable *>(a.data))["__index"]) = a; // ?
  b = new SpecialTable();
  setmetatable(b, a);
  print((*(*std::get<SpecialTable *>(b.data))["x"]));
  set_tabvalue(b.table, FIELD_X, TNUM(fix32_from_int16(5)));
  print((*(*std::get<SpecialTable *>(b.data))["x"]));
  print((*(*std::get<SpecialTable *>(b.data))["y"]));
  (*(*std::get<SpecialTable *>(a.data))["y"]) = TNUM(fix32_from_int16(7)); // ?
  print((*(*std::get<SpecialTable *>(b.data))["y"]));
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}
