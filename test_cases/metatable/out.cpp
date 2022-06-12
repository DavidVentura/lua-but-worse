#include "header.h"
const uint16_t FIELD___INDEX = 0;
const uint16_t FIELD_X = 1;
const TValue *idx_to_name[2] = {new TValue("__index"), new TValue("x")};

class SpecialTable : public Table {

public:
  TValue *fast_fields[2];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 2; i++)
      fast_fields[i] = TValue::OPT_VAL();

    fields["__index"] = fast_fields[FIELD___INDEX];
    fields["x"] = fast_fields[FIELD_X];
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
  TValue b;
  TValue a;
  TValue main();

  TValue main() {
    a = new SpecialTable({{"x", new TValue(fix32(5))}});
    a.t->set(FIELD___INDEX, a);
    b = new SpecialTable();
    setmetatable(b, a);
    print(b.t->get(FIELD_X));
    b.t->inc(FIELD_X, fix32(5));
    print(b.t->get(FIELD_X));
    return fix32(0);
  }
} // namespace Game
