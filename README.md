# Lua, but worse

This is a terrible Lua-to-C++ compiler. This is only for Lua 5.2 base; and with extensions for Pico-8.

Tables are just `std::unordered_map`.

## Known bugs

* Memory leaks everywhere.
* Deleting a table (`del(tab, item)` if `item` is a table) will free its memory without considering that it might be used in more places
* Assigning NULL to a field in a table (`a.x = nil`) will _not_ free its memory

## Performance

For the Pico-8 game "Tennis" (very simple pong-like) the compiled performance is ~2.5x; spawning / updating / drawing 400 balls goes from 30ms to 12ms.

This is achieved with a single optimization:

For each literal table access (`tab["key1"]`, `tab.key2`), the `Table` class gets an array of TValues of that size (in this case, 2), and keys in the hashmap (`key1`, `key2`) that point to the corresponding array entries, making these two accesses equal:

```cpp
*member.t->fields["key1"];
member.t->fast_fields[0];
```

so, literal field accesses can be optimized for faster access and programatic access to keys still works.

For a purely calculation-based "micro benchmark" (no table accesses) coming from a real game (`spr_r` from game Rockets!) performance is ~30x (!) when comparing to the original version, which uses globals for all its variables, and ~21x (!) for the optimized version (using locals) [Time to run benchmark went from 93s to 3278ms].  
There are no (user-generated) table lookups in this benchmark, so the speedup comes purely from translating the code. This type of benchmark is the best case for this translation: memory accesses are _terribly_ slow (via PSRAM).

There's a not-yet-implemented possible optimization: downcasting (at compile time) `TValue` to `fix32` when possible -- in this benchmark the execution goes to 2264ms (~1.44x faster)

## Not implemented

* Methods (`tab:method`)
* Functions that return tuples (`a, b = func()`)
* Proper memory management
* Metamethods (other than `__index`)

## What it looks like

Input:

```lua
function main()
	squares = {1, 4, 9, 16, 25}
	member  = {x=1}
	empty   = {}

	-- using table as index
	member[empty] = 5
	print(member[empty])
end
```

Output

```c
#include "lua.c"
TValue_t empty;
TValue_t member;
TValue_t squares;
TValue_t __main();

TValue_t __main() {
  squares = TTAB(make_table(5));
  set_tabvalue(squares.table, TNUM16(1), TNUM16(1));
  set_tabvalue(squares.table, TNUM16(2), TNUM16(4));
  set_tabvalue(squares.table, TNUM16(3), TNUM16(9));
  set_tabvalue(squares.table, TNUM16(4), TNUM16(16));
  set_tabvalue(squares.table, TNUM16(5), TNUM16(25));

  member = TTAB(make_table(1));
  set_tabvalue(member.table, TSTR("x"), TNUM16(1));

  empty = TTAB(make_table(0));
  set_tabvalue(member.table, empty, TNUM16(5));

  print(get_tabvalue(member.table, empty));
}
```
