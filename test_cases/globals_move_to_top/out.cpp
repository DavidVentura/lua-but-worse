#include "header.h"
TValue c;
TValue b;
TValue a;
TValue *f();

TValue *f() {
  a = fix32(5);
  b = fix32(7);
  return NULL;
}
c = a * b; // unknown
