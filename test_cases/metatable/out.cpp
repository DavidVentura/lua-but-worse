#include "header.h"
TValue b;
TValue a;
TValue main();

TValue main() {
  a.t = new Table({{"x", fix32(5)}});
  (*a.t)["__index"] = a; // ?
  b.t = new Table();
  setmetatable(b, a);
  print((*b.t)["x"]);
  (*b.t)["x"] += fix32(5);
  print((*b.t)["x"]);
  return fix32(0);
}