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
  TValue e;
  TValue d;
  TValue c;
  TValue b;
  TValue a;
  TValue test;
  TValue main();

  TValue main() {
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

    if (test) {
      print("this should not be visible");
    }
    return fix32(0);
  }
} // namespace Game
