#include "header.h"
const uint16_t FIELD_F = 0;
const TValue *idx_to_name[1] = {new TValue("f")};

class SpecialTable : public Table {

public:
  TValue fast_fields[1];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 1; i++)
      fast_fields[i] = TValue::OPT_VAL();

    fields["f"] = &fast_fields[FIELD_F];
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

  void set(uint16_t idx, TValue val) { fast_fields[idx] = val; }

  void sub(uint16_t idx, TValue val) {
    if (fast_fields[idx].tag == TT_OPT) {
      fast_fields[idx] = *(*this)[*idx_to_name[idx]];
    }
    fast_fields[idx] -= val;
  }
  void inc(uint16_t idx, TValue val) {
    if (fast_fields[idx].tag == TT_OPT) {
      fast_fields[idx] = *(*this)[*idx_to_name[idx]];
    }
    fast_fields[idx] += val;
  }
  TValue get(uint16_t idx) {
    TValue ret = fast_fields[idx];
    if (ret.tag == TT_OPT) {
      return *(*this)[*idx_to_name[idx]];
    }
    return ret;
  }
};
#include "impl.cpp"
namespace Game {
  TValue v;
  TValue c;
  TValue b;
  TValue a;
  TValue captured;
  TValue main();

  TValue main() {
    captured = 7;
    a = TValue([&](std::vector<TValue> args) -> TValue {
      TValue x = get_with_default(args, 0);
      return x * captured;
    });
    print(a({5}));
    b = TValue([&](std::vector<TValue> args) -> TValue {
      TValue x = get_with_default(args, 0);
      TValue y = get_with_default(args, 1);
      return x * y;
    });
    print(b({5, 6}));
    c = new SpecialTable();
    c.t->set(FIELD_F, TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table"; }));
    print(c.t->get(FIELD_F)({}));
    v = "index";
    (*(*c.t)[v]) = TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table, via hashmap"; }); // ?
    print((*(*c.t)[v])({}));
    return 0;
  }
} // namespace Game