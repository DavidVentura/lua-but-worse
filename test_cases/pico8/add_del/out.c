#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t test_del(uint8_t argc, TValue_t *function_arguments);
TValue_t test_add(uint8_t argc, TValue_t *function_arguments);

TValue_t test_add(uint8_t argc, TValue_t *function_arguments) {
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

TValue_t test_del(uint8_t argc, TValue_t *function_arguments) {
  TValue_t gc t = T_NULL;
  _set(&t, TTAB(make_table(0)));
  add(t, TSTR("a value"));
  add(t, TSTR("another value"));
  printh(get_tabvalue(t, TNUM16(1)));
  del(t, TSTR("a value"));
  printh(get_tabvalue(t, TNUM16(1)));
}

TValue_t __main() {
  CALL((test_add), 0, NULL);
  printh(TSTR("---"));
  CALL((test_del), 0, NULL);
}

TValue_t __preinit() {}
