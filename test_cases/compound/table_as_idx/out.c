#include "lua.h"
#include "pico8.h"
TValue_t __str_x = T_NULL;
TValue_t empty = T_NULL;
TValue_t member = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&member, TTAB(make_table(3)));
  // Fields for table member
  set_tabvalue(member, __str_x, TNUM16(1));
  _set(&empty, TTAB(make_table(0)));
  set_tabvalue(member, empty, TNUM16(5));
  printh(get_tabvalue(member, empty));
}

TValue_t __preinit() {
  _grow_strings_to(1);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 0)));
}
