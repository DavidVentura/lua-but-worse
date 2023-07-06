#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc t = T_NULL;
  TValue_t gc a = T_NULL;
  _set(&a, TNUM16(5));
  _set(&t, TTAB(make_table(4)));
  // Fields for table t
  set_tabvalue(t, TNUM16(1), TNUM16(1));
  set_tabvalue(t, TNUM16(2), TNUM16(2));
  set_tabvalue(t, TNUM16(3), TNUM16(3));
  set_tabvalue(t, TNUM16(4), TNUM16(4));

  {
    TValue_t *_super_secret_iterator = all(t);
    uint16_t __i = 0;
    while (_super_secret_iterator[__i].tag != NUL) {
      TValue_t item = _super_secret_iterator[__i];
      printh(item);
      __i++;
    }

    free(_super_secret_iterator);
  }
}

TValue_t __preinit() { _grow_strings_to(0); }
