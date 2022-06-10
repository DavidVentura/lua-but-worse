#include "header.h"
TValue d;
TValue c;
TValue b;
TValue a;
TValue main();

TValue main() {
  a = fix32(1);
  b = fix32(2);
  c = flr(fix32(1.5f)); // ?
  d = flr(fix32(2.5f)); // ?
  print(a * c);
  print(b * d);
  return fix32(0);
}
