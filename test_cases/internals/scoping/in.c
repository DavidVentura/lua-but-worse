#include "lua.h"
#include "lua_math.h"
#include "lua_table.h"
#include "pico8.h"
#include "stdlib.h"
TValue_t __str__dash__dash__dash_ = T_NULL;
TValue_t __str_nope2 = T_NULL;
TValue_t __str_nope = T_NULL;
TValue_t __preinit();
TValue_t __main();
TValue_t local_var(TVSlice_t function_arguments);
TValue_t else_arm(TVSlice_t function_arguments);
TValue_t elseif_else_arm(TVSlice_t function_arguments);

TValue_t elseif_else_arm(TVSlice_t function_arguments) {
  TValue_t gc a = T_NULL;

  if (_bool(T_FALSE)) {
    printh(__str_nope);
  } else if (_bool(T_FALSE)) {
    printh(__str_nope2);
  } else {
    TValue_t gc a = T_NULL;
    _set(&a, TNUM16(200));
    printh(a);
  }
  printh(a);
}

TValue_t else_arm(TVSlice_t function_arguments) {
  TValue_t gc a = T_NULL;

  if (_bool(T_FALSE)) {
    printh(__str_nope);
  } else {
    TValue_t gc a = T_NULL;
    _set(&a, TNUM16(100));
    printh(a);
  }
  printh(a);
}

TValue_t local_var(TVSlice_t function_arguments) {
  TValue_t gc a = T_NULL;

  if (_bool(TNUM16(5))) {
    TValue_t gc a = T_NULL;
    _set(&a, TNUM16(100));
  }
  printh(a);
}

TValue_t __main() {
  CALL((local_var), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(__str__dash__dash__dash_);
  CALL((else_arm), ((TVSlice_t){.elems = NULL, .num = 0}));
  printh(__str__dash__dash__dash_);
  CALL((elseif_else_arm), ((TVSlice_t){.elems = NULL, .num = 0}));
}

TValue_t __preinit() {
  _grow_strings_to(3);
  _set(&__str_nope, TSTRi(_store_str_at_or_die(CONSTSTR("nope"), 2)));
  _set(&__str_nope2, TSTRi(_store_str_at_or_die(CONSTSTR("nope2"), 1)));
  _set(&__str__dash__dash__dash_, TSTRi(_store_str_at_or_die(CONSTSTR("---"), 0)));
}
