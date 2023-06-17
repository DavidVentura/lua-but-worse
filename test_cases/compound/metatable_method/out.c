#include "lua.c"
TValue_t b;
TValue_t a;
TValue_t __preinit();
TValue_t __a_method(TValue_t *function_arguments);
TValue_t __main();

TValue_t __main() {
  a = TTAB(make_table(2));
  set_tabvalue(a, TSTR("__index"), a);
  set_tabvalue(a, TSTR("method"), TFUN(__a_method));
  b = TTAB(make_table(0));
  setmetatable(b, a);
  CALL((get_tabvalue(b, TSTR("method"))), ((TValue_t[]){b}));
  print(get_tabvalue(a, TSTR("x")));
  print(get_tabvalue(b, TSTR("x")));
  CALL((get_tabvalue(a, TSTR("method"))), ((TValue_t[]){a}));
  print(get_tabvalue(a, TSTR("x")));
  print(get_tabvalue(b, TSTR("x")));
}

TValue_t __a_method(TValue_t *function_arguments) {
  TValue_t self;
  self = function_arguments[0]; // unknown type
  set_tabvalue(self, TSTR("x"), TNUM16(5));
  return T_NULL;
}

TValue_t __preinit() {}
