#include "lua.h"
#include "pico8.h"
TValue_t b = T_NULL;
TValue_t __preinit();
TValue_t __nested_func_b(TVSlice_t function_arguments);
TValue_t __main();

TValue_t __main() {
  _set(&b, TFUN(__nested_func_b));
  CALL((b), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __nested_func_b(TVSlice_t function_arguments) { printh(TNUM16(5)); }

TValue_t __preinit() {}
