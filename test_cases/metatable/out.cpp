#include "header.h"
namespace Game {
  TValue b;
  TValue a;
  TValue main();

  TValue main() {
    a = new SpecialTable({{"x", new TValue(fix32(5))}});
    (*(*a.t)["__index"]) = a; // ?
    b = new SpecialTable();
    setmetatable(b, a);
    print((*(*b.t)["x"]));
    (*(*b.t)["x"]) += fix32(5);
    print((*(*b.t)["x"]));
    return fix32(0);
  }
} // namespace Game
