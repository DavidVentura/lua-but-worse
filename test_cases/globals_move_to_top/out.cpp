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
} // namespace Game
