#include "lua.c"
TValue_t b;
TValue_t a;
void __preinit();
void __main();

void __main() {
  a = TTAB(make_table(4));
  set_tabvalue(a.table, TSTR("__index"), a) = a; // ?
  method = TValue([&](std::vector<TValue> args) -> TValue {
    TValue self = get_with_default(args, 0);
    set_tabvalue(self.table, TSTR("x"), TNUM16(5)) = TNUM16(5); // ?
    return NUL;
  });
  b = TTAB(make_table(4));
  setmetatable(b, a);
  get_tabvalue(b.table, TSTR("method"))({b});
  print(get_tabvalue(a.table, TSTR("x")));
  print(get_tabvalue(b.table, TSTR("x")));
  get_tabvalue(a.table, TSTR("method"))({a});
  print(get_tabvalue(a.table, TSTR("x")));
  print(get_tabvalue(b.table, TSTR("x")));
}

void __preinit() {}
