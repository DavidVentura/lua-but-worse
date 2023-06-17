#include "lua.c"
TValue_t empty;
TValue_t member;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  member = TTAB(make_table(1));
  // Fields for table member
  set_tabvalue(member, TSTR("x"), TNUM16(1));
  empty = TTAB(make_table(0));
  set_tabvalue(member, empty, TNUM16(5));
  print(get_tabvalue(member, empty));
}

TValue_t __preinit() {}
