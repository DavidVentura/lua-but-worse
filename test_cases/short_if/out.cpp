#include "header.h"
TValue e;
TValue d;
TValue c;
TValue b;
TValue a;
TValue test;
void main();

void main() {
  test = fix32(0);

  if (test) {
    return fix32(5);
  }

  if (test) {
    return fix32(5);
  } else {
    a = fix32(1);
  }

  if (test) {
    return fix32(5);
  } else {
    b = fix32(2);
    c = fix32(3);
  }

  if (!test) {
    d = -fix32(4); // ?
  } else {
    e = fix32(0);
  }
  print(a);
  print(b);
  print(c);
  print(d);
}
