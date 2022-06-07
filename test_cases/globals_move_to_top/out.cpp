#include "header.h"
TValue c;
TValue b;
TValue a;
void f();

void f() {
  a = fix32(5);
  b = fix32(7);
}
c = a * b; // ?
