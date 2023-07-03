#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t test_del(TVSlice_t function_arguments);
TValue_t test_add(TVSlice_t function_arguments);

TValue_t test_add(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  _set(&t, TTAB(make_table(0)));
  add(t, TSTR("a value"));
  printh(get_tabvalue(t, TNUM16(1)));
  printh(get_tabvalue(t, TNUM16(2)));
  add(t, TSTR("another value"));
  printh(get_tabvalue(t, TNUM16(1)));
  printh(get_tabvalue(t, TNUM16(2)));
  printh(get_tabvalue(t, TNUM16(3)));
}

TValue_t test_del(TVSlice_t function_arguments) {
  TValue_t gc t = T_NULL;
  _set(&t, TTAB(make_table(0)));
  add(t, TSTR("a value"));
  add(t, TSTR("another value"));
  printh(get_tabvalue(t, TNUM16(1)));
  del(t, TSTR("a value"));
  printh(get_tabvalue(t, TNUM16(1)));
}

TValue_t __main() {
  CALL((test_add), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(TSTR("---"));
  CALL((test_del), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {}
