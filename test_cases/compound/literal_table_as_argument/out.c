#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t particles = T_NULL;
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __preinit();
TValue_t __anonymous_function_0(TVSlice_t function_arguments);
TValue_t __main();
TValue_t add_particle(TVSlice_t function_arguments);

TValue_t add_particle(TVSlice_t function_arguments) {
  TValue_t gc _anonymous_table_0 = T_NULL;
  TValue_t gc y = T_NULL;
  TValue_t gc x = T_NULL;
  _set(&x, __get_array_index_capped(function_arguments, 0));
  _set(&y, __get_array_index_capped(function_arguments, 1));
  _set(&_anonymous_table_0, TTAB(make_table(4)));
  set_tabvalue(_anonymous_table_0, __str_x, x);
  set_tabvalue(_anonymous_table_0, __str_y, y);
  add(particles, _anonymous_table_0);
}

TValue_t __main() {
  CALL((add_particle), ((TVSlice_t){.elems = (TValue_t[2]){TNUM16(1), TNUM16(5)}, .num = 2}));
  foreach (particles, TFUN(__anonymous_function_0))
    ;
}

TValue_t __anonymous_function_0(TVSlice_t function_arguments) {
  TValue_t gc particle = T_NULL;
  _set(&particle, __get_array_index_capped(function_arguments, 0));
  printh(get_tabvalue(particle, __str_x));
  printh(get_tabvalue(particle, __str_y));
}

TValue_t __preinit() {
  _grow_strings_to(2);
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
  _set(&particles, TTAB(make_table(0)));
}
