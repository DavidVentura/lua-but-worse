#include "header.h"

const TValue *idx_to_name[0] = {};

class SpecialTable : public Table {

public:
  TValue *fast_fields[0];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 0; i++)
      fast_fields[i] = TValue::OPT_VAL();
  }

  // why o why does this not work when defined in Table
  inline TValue *operator[](TValue const &key) {
    if (fields.count(key) && fields[key]->tag != TT_OPT) {
      // TT_OPT here means "optimized" -- unset
      return fields[key];
    }

    if (metatable != NULL && metatable->fields.count("__index")) {
      fields[key] = (*(metatable->fields["__index"]->t))[key];
      return fields[key];
    }

    fields[key] = new TValue();
    return fields[key];
  }

  void set(uint16_t idx, TValue val) { *fast_fields[idx] = val; }

  void inc(uint16_t idx, TValue val) {
    TValue *target = fast_fields[idx];
    if (target->tag == TT_OPT) {
      *target = *(*this)[*idx_to_name[idx]];
    }
    *target += val;
  }
  TValue get(uint16_t idx) {
    TValue ret = *fast_fields[idx];
    if (ret.tag == TT_OPT) {
      return *(*this)[*idx_to_name[idx]];
    }
    return ret;
  }
};
#include "impl.cpp"
namespace Game {
  TValue empty;
  TValue member;
  TValue squares;
  void main();

  void main() {
    squares = new SpecialTable({{fix32(1), new TValue(fix32(1))},
                                {fix32(2), new TValue(fix32(4))},
                                {fix32(3), new TValue(fix32(9))},
                                {fix32(4), new TValue(fix32(16))},
                                {fix32(5), new TValue(fix32(25))}});
    member = new SpecialTable({{"x", new TValue(fix32(1))}});
    empty = new SpecialTable();
    (*(*member.t)[empty]) = fix32(5); // ?
    print((*(*member.t)[empty]));
  }
} // namespace Game
