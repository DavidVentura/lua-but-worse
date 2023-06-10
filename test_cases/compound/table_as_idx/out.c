#include "lua.c"
TValue_t empty;
TValue_t member;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  member = TTAB(make_table(4));
  set_tabvalue(member.table, TSTR("x"), TNUM16(1));
  empty = TTAB(make_table(4));
  set_tabvalue(member.table, empty, TNUM16(5));
  print(get_tabvalue(member.table, empty));
}

TValue_t __preinit() {}
