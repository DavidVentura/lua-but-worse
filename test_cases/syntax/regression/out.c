#include "lua.h"
#include "pico8.h"
TValue_t __preinit();
TValue_t __main();
TValue_t celeste(uint8_t argc, TValue_t *function_arguments);

TValue_t celeste(uint8_t argc, TValue_t *function_arguments) {

  if (_bool(T_FALSE)) {
    return T_NULL;
  }
}

TValue_t __main() { CALL((celeste), 0, NULL); }

TValue_t __preinit() {}
