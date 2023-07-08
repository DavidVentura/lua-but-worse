#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
TValue_t __str_a = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_tables = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t change_state = T_NULL;
TValue_t next_state = T_NULL;
TValue_t state = T_NULL;
TValue_t c = T_NULL;
TValue_t b = T_NULL;
TValue_t a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  _set(&a, TTAB(make_table(2)));
  set_tabvalue(a, __str_x, TNUM16(5));
  set_tabvalue(a, __str_y, TNUM16(6));
  printh(get_tabvalue(a, __str_x));
  printh(get_tabvalue(a, __str_y));
  _set(&b, TNUM16(1));
  _set(&c, TNUM16(2));
  printh(b);
  printh(c);
  printh(__str_tables);
  _set(&state, TTAB(make_table(2)));
  _set(&next_state, TTAB(make_table(2)));
  _set(&change_state, T_FALSE);
  // Fields for table state
  set_tabvalue(next_state, __str_b, TNUM16(2));
  // Fields for table state
  set_tabvalue(state, __str_a, TNUM16(1));
  printh(get_tabvalue(state, __str_a));
  printh(get_tabvalue(next_state, __str_b));
  printh(change_state);
}

TValue_t __preinit() {
  _grow_strings_to(5);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 4)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 3)));
  _set(&__str_tables, TSTRi(_store_str_at_or_die(CONSTSTR("tables"), 2)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 1)));
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 0)));
}
