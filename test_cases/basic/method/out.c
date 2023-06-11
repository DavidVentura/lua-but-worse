#include "lua.c"
TValue_t value;
TValue_t a;
TValue_t __preinit();
TValue_t __a_method(TValue_t self, TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  a = TTAB(make_table(0));
  // Fields for table a
  set_tabvalue(a.table, TSTR("method"), TFUN(__a_method));
  print(get_tabvalue(a.table, TSTR("x")));
  get_tabvalue(a.table, TSTR("method")).fun(a, (TValue_t[]){TNUM16(5)});
  print(get_tabvalue(a.table, TSTR("x")));
}

TValue_t __a_method(TValue_t self, TValue_t *function_arguments) {
  TValue_t value;
  value = function_arguments[0]; // unknown type
  set_tabvalue(self.table, TSTR("x"), value);
  return T_NULL;
}

TValue_t __preinit() {}
