#include "lua.c"
TValue_t a;
TValue_t __preinit();
TValue_t __a_method(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  a = TTAB(make_table(0));
  set_tabvalue(a, TSTR("method"), TFUN(__a_method));
  print(get_tabvalue(a, TSTR("x")));
  CALL((get_tabvalue(a, TSTR("method"))), ((TValue_t[]){a, TNUM16(5)}));
  print(get_tabvalue(a, TSTR("x")));
}

TValue_t __a_method(TValue_t *function_arguments) {
  TValue_t value;
  TValue_t self;
  self = function_arguments[0];  // unknown type
  value = function_arguments[1]; // unknown type
  set_tabvalue(self, TSTR("x"), value);
  return T_NULL;
}

TValue_t __preinit() {}
