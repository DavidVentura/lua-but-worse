#include "lua.c"
TValue_t t;
TValue_t u;
TValue_t o;
TValue_t e;
TValue_t d;
TValue_t c;
TValue_t b;
TValue_t a;
void __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t x;
  x = TNUM(fix32_from_int16(1));
  _pluseq(&x, TNUM(fix32_from_int16(2)));
  a = TNUM(fix32_from_int16(1));
  b = TNUM(fix32_from_int16(2));
  c = flr(FROMFLOATfix32(1.5f)); // ?
  d = flr(FROMFLOATfix32(2.5f)); // ?
  e = TNUM(fix32_from_int16(1));
  e = TNUM(fix32_from_int16(2));
  o = new SpecialTable();
  u = new SpecialTable({{"a", new TValue(TNUM(fix32_from_int16(1)))}});
  (*(*std::get<SpecialTable *>(o.data))["a"]) = (*(*std::get<SpecialTable *>(u.data))["a"]); // ?
  t = TNUM(fix32_from_int16(7));
  t = (*(*std::get<SpecialTable *>(new SpecialTable({{TNUM(fix32_from_int16(7)), new TValue(TNUM(fix32_from_int16(8)))},
                                                     {TNUM(fix32_from_int16(8)), new TValue(TNUM(fix32_from_int16(7)))}})
                                       .data))[t]); // ?
  print(_mult(a, c));
  print(_mult(b, d));
  print(e);
  print((*(*std::get<SpecialTable *>(o.data))["a"]));
  print(t);
  return TNUM(fix32_from_int16(0));
}

void __preinit() {}
