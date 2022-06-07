#include "header.h"
TValue empty;
TValue member;
TValue squares;
squares.t = new std::unordered_map<std::string, TValue>(
    {{fix32(1), fix32(1)}, {fix32(2), fix32(4)}, {fix32(3), fix32(9)}, {fix32(4), fix32(16)}, {fix32(5), fix32(25)}});
member.t = new std::unordered_map<std::string, TValue>({{"x", fix32(1)}});
empty.t = new std::unordered_map<std::string, TValue>();
