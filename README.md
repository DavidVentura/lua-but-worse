# Lua, but worse

This is a terrible Lua-to-C++ compiler. This is only for Lua 5.2 base; and with extensions for Pico-8.

Tables are just `std::unordered_map`.

## Known bugs

* Memory leaks everywhere.
* Deleting a table (`del(tab, item)` if `item` is a table) will free its memory without considering that it might be used in more places

## Performance

For the Pico-8 game "Tennis" (very simple pong-like) the compiled performance is ~2.5x; spawning / updating / drawing 400 balls goes from 30ms to 12ms.

This is achieved with a single optimization:

For literal table accesses (`tab["constant"]`), the `Table` class gets a `constant` member and a `"constant"` entry in the hashmap, pointing to the class member, making these two accesses equal:

```cpp
*member.t->fields["constant"];
member.t->constant;
```

so, literal field accesses can be optimized for faster access and programatic access to keys still works.

## Not implemented

* Methods (`tab:method`)
* Functions that return tuples (`a, b = func()`)
* Proper memory management

## What it looks like

Input:

```lua
-- ...
function main()
	squares = {1, 4, 9, 16, 25}
	member  = {x=1}
	empty   = {}

	-- using table as index
	member[empty] = 5
	print(member[empty])
end
```

```
#include "header.h"
TValue empty;
TValue member;
TValue squares;
void main();

void main() {
  squares = new SpecialTable({{fix32(1), new TValue(fix32(1))},
                              {fix32(2), new TValue(fix32(4))},
                              {fix32(3), new TValue(fix32(9))},
                              {fix32(4), new TValue(fix32(16))},
                              {fix32(5), new TValue(fix32(25))}});
  member = new SpecialTable({{"x", new TValue(fix32(1))}});
  empty = new SpecialTable();
  (*(*member.t)[empty]) = fix32(5); // ?
  print((*(*member.t)[empty]));
}
```
