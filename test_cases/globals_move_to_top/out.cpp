#include "header.h"
TValue c;
TValue main();

TValue main() {
  TValue b;
  TValue a;
  a = fix32(5);
  b = fix32(7);
  c = a * b; // ?
  print(c);
  return fix32(0);
}
