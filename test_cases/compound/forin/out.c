#include "lua.h"
#include "pico8.h"
TValue_t __str_y = T_NULL;
TValue_t __str_x = T_NULL;
TValue_t __str_n = T_NULL;
TValue_t __str_ipairs_over_num_table = T_NULL;
TValue_t __str_ipairs_over_fib_table = T_NULL;
TValue_t __str_f = T_NULL;
TValue_t __str_e = T_NULL;
TValue_t __str_d = T_NULL;
TValue_t __str_c = T_NULL;
TValue_t __str_b = T_NULL;
TValue_t __str_a = T_NULL;
TValue_t __preinit();
TValue_t __main();

TValue_t __main() {
  TValue_t gc numtab = T_NULL;
  TValue_t gc tab = T_NULL;
  _set(&tab, TTAB(make_table(12)));
  // Fields for table tab
  set_tabvalue(tab, __str_a, TNUM16(1));
  set_tabvalue(tab, __str_b, TNUM16(1));
  set_tabvalue(tab, __str_c, TNUM16(2));
  set_tabvalue(tab, __str_d, TNUM16(3));
  set_tabvalue(tab, __str_e, TNUM16(5));
  set_tabvalue(tab, __str_f, TNUM16(8));

  {
    KV_t *_pairs_iterator = pairs(tab);
    uint16_t __i = 0;
    while (_pairs_iterator[__i].key.tag != NUL) {
      TValue_t k = _pairs_iterator[__i].key;
      TValue_t v = _pairs_iterator[__i].value;
      printh(k);
      printh(v);
      __i++;
    }

    free(_pairs_iterator);
  }

  printh(__str_ipairs_over_fib_table);

  {
    KV_t *_pairs_iterator = ipairs(tab);
    uint16_t __i = 0;
    while (_pairs_iterator[__i].key.tag != NUL) {
      TValue_t k = _pairs_iterator[__i].key;
      TValue_t v = _pairs_iterator[__i].value;
      printh(k);
      printh(v);
      __i++;
    }

    free(_pairs_iterator);
  }

  printh(__str_ipairs_over_num_table);
  _set(&numtab, TTAB(make_table(9)));
  // Fields for table numtab
  set_tabvalue(numtab, TNUM16(1), TNUM16(123));
  set_tabvalue(numtab, TNUM16(2), TNUM16(456));
  set_tabvalue(numtab, __str_n, TNUM16(42));
  set_tabvalue(numtab, __str_x, TNUM16(100));
  set_tabvalue(numtab, __str_y, TNUM16(200));
  set_tabvalue(numtab, TNUM16(3), TNUM16(789));

  {
    KV_t *_pairs_iterator = ipairs(numtab);
    uint16_t __i = 0;
    while (_pairs_iterator[__i].key.tag != NUL) {
      TValue_t k = _pairs_iterator[__i].key;
      TValue_t v = _pairs_iterator[__i].value;
      printh(v);
      __i++;
    }

    free(_pairs_iterator);
  }
}

TValue_t __preinit() {
  _grow_strings_to(11);
  _set(&__str_a, TSTRi(_store_str_at_or_die(CONSTSTR("a"), 10)));
  _set(&__str_b, TSTRi(_store_str_at_or_die(CONSTSTR("b"), 9)));
  _set(&__str_c, TSTRi(_store_str_at_or_die(CONSTSTR("c"), 8)));
  _set(&__str_d, TSTRi(_store_str_at_or_die(CONSTSTR("d"), 7)));
  _set(&__str_e, TSTRi(_store_str_at_or_die(CONSTSTR("e"), 6)));
  _set(&__str_f, TSTRi(_store_str_at_or_die(CONSTSTR("f"), 5)));
  _set(&__str_ipairs_over_fib_table, TSTRi(_store_str_at_or_die(CONSTSTR("ipairs over fib table"), 4)));
  _set(&__str_ipairs_over_num_table, TSTRi(_store_str_at_or_die(CONSTSTR("ipairs over num table"), 3)));
  _set(&__str_n, TSTRi(_store_str_at_or_die(CONSTSTR("n"), 2)));
  _set(&__str_x, TSTRi(_store_str_at_or_die(CONSTSTR("x"), 1)));
  _set(&__str_y, TSTRi(_store_str_at_or_die(CONSTSTR("y"), 0)));
}
