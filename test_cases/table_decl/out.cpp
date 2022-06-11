#include "header.h"
TValue empty;
TValue member;
TValue squares;
void main();

void main() {
  squares = new Table({{fix32(1), new TValue(fix32(1))},
                       {fix32(2), new TValue(fix32(4))},
                       {fix32(3), new TValue(fix32(9))},
                       {fix32(4), new TValue(fix32(16))},
                       {fix32(5), new TValue(fix32(25))}});
  member = new Table({{"x", new TValue(fix32(1))}});
  empty = new Table();
  (*(*member.t)[empty]) = fix32(5); // ?
  print((*(*member.t)[empty]));
}
