#include "lua.c"
TValue_t __preinit();
TValue_t __main();
TValue_t something(TValue_t *function_arguments);

TValue_t something(TValue_t *function_arguments) {
  TValue_t arg;
  arg = function_arguments[0]; // unknown type
  return arg;
}

TValue_t __main() {
  CALL((something), ((TValue_t[]){TNUM16(5)}));
  print(CALL((something), ((TValue_t[]){TNUM16(6)})));
}

TValue_t __preinit() {}
