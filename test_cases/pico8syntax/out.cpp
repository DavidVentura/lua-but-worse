#include "header.h"
namespace Game {
  TValue b;
  TValue a;
  TValue main();

  TValue main() {
    a = fix32(0x0, 0x8000);
    b = fix32(0x0, 0x1000);
    print(a);
    print(b);
    return fix32(0);
  }
} // namespace Game
