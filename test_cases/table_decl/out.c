#include "lua.c"
TValue_t literals;
TValue_t empty;
TValue_t member;
TValue_t squares;
void __preinit();
void __main();

void __main() {
  squares = TTAB(make_table(4));
  set_tabvalue(squares.table, TNUM16(1), TNUM16(1));
  set_tabvalue(squares.table, TNUM16(2), TNUM16(4));
  set_tabvalue(squares.table, TNUM16(3), TNUM16(9));
  set_tabvalue(squares.table, TNUM16(4), TNUM16(16));
  set_tabvalue(squares.table, TNUM16(5), TNUM16(25));
  ;
  member = TTAB(make_table(4));
  set_tabvalue(member.table, TSTR("x"), TNUM16(1));
  ;
  empty = TTAB(make_table(4));
  literals = TTAB(make_table(4));
  set_tabvalue(literals.table, TSTR("" top ""), TTAB(make_table(4)); set_tabvalue(T_IDK.table, TSTR("x"), TNUM16(0));
               set_tabvalue(T_IDK.table, TSTR("y"), _invert_sign(TNUM16(68))););
  set_tabvalue(literals.table, TSTR("" bottom ""), TTAB(make_table(4)); set_tabvalue(T_IDK.table, TSTR("x"), TNUM16(0));
               set_tabvalue(T_IDK.table, TSTR("y"), TNUM16(68)););
  set_tabvalue(literals.table, TSTR("" left ""), TTAB(make_table(4)); set_tabvalue(T_IDK.table, TSTR("x"), _invert_sign(TNUM16(68)));
               set_tabvalue(T_IDK.table, TSTR("y"), TNUM16(0)););
  set_tabvalue(literals.table, TSTR("" right ""), TTAB(make_table(4)); set_tabvalue(T_IDK.table, TSTR("x"), TNUM16(68));
               set_tabvalue(T_IDK.table, TSTR("y"), TNUM16(0)););
  ;
  print(get_tabvalue(get_tabvalue(literals.table, TSTR("top")).table, TSTR("y")));
  set_tabvalue(member.table, empty, TNUM16(5)) = TNUM16(5); // ?
  print(get_tabvalue(member.table, empty));
}

void __preinit() {}
