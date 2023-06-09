#include "lua.c"
TValue_t literals;
TValue_t empty;
TValue_t member;
TValue_t squares;
void __preinit();
void __main();

void __main() {
  squares = new SpecialTable({{TNUM(fix32_from_int16(1)), new TValue(TNUM(fix32_from_int16(1)))},
                              {TNUM(fix32_from_int16(2)), new TValue(TNUM(fix32_from_int16(4)))},
                              {TNUM(fix32_from_int16(3)), new TValue(TNUM(fix32_from_int16(9)))},
                              {TNUM(fix32_from_int16(4)), new TValue(TNUM(fix32_from_int16(16)))},
                              {TNUM(fix32_from_int16(5)), new TValue(TNUM(fix32_from_int16(25)))}});
  member = new SpecialTable({{"x", new TValue(TNUM(fix32_from_int16(1)))}});
  empty = new SpecialTable();
  literals = new SpecialTable(
      {{"" top "", new TValue(new SpecialTable(
                       {{"x", new TValue(TNUM(fix32_from_int16(0)))}, {"y", new TValue(_invert_sign(TNUM(fix32_from_int16(68))))}}))},
       {"" bottom "",
        new TValue(new SpecialTable({{"x", new TValue(TNUM(fix32_from_int16(0)))}, {"y", new TValue(TNUM(fix32_from_int16(68)))}}))},
       {"" left "", new TValue(new SpecialTable(
                        {{"x", new TValue(_invert_sign(TNUM(fix32_from_int16(68))))}, {"y", new TValue(TNUM(fix32_from_int16(0)))}}))},
       {"" right "",
        new TValue(new SpecialTable({{"x", new TValue(TNUM(fix32_from_int16(68)))}, {"y", new TValue(TNUM(fix32_from_int16(0)))}}))}});
  print((*(*std::get<SpecialTable *>((*(*std::get<SpecialTable *>(literals.data))["top"]).data))["y"]));
  (*(*std::get<SpecialTable *>(member.data))[empty]) = TNUM(fix32_from_int16(5)); // ?
  print((*(*std::get<SpecialTable *>(member.data))[empty]));
}

void __preinit() {}
