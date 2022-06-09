#include "header.h"
int main();

int main() {
  for (auto i = fix32(1); i < fix32(7); i += fix32(1)) {
    print("%d\n", i);
  }
  return fix32(0);
}
