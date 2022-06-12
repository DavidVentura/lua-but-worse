#include "header.h"
namespace Game {
  TValue main();

  TValue main() {
    for (auto i = fix32(1); i < fix32(7); i += fix32(1)) {
      print(i);
    }
    return fix32(0);
  }
} // namespace Game
