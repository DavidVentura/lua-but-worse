#include "header.h"
namespace Game {
  TValue e;
  TValue d;
  TValue c;
  TValue b;
  TValue a;
  TValue main();

  TValue main() {
    TValue x;
    x = fix32(1);
    x += fix32(2);
    a = fix32(1);
    b = fix32(2);
    c = flr(fix32(1.5f)); // ?
    d = flr(fix32(2.5f)); // ?
    e = fix32(1);
    e = fix32(2);
    print(a * c);
    print(b * d);
    print(e);
    return fix32(0);
  }
} // namespace Game
