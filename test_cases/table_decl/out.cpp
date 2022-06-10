#include "header.h"
TValue empty;
TValue member;
TValue squares;
void main();

void main() {
  squares = new Table({{fix32(1), fix32(1)}, {fix32(2), fix32(4)}, {fix32(3), fix32(9)}, {fix32(4), fix32(16)}, {fix32(5), fix32(25)}});
  member = new Table({{"x", fix32(1)}});
  empty = new Table();
}
