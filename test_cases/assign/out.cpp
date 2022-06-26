#include "header.h"
const uint16_t FIELD_A = 0;
const TValue *idx_to_name[1] = {new TValue("a")};

class SpecialTable : public Table {

public:
  TValue fast_fields[1];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 1; i++)
      fast_fields[i] = TValue();

    fields["a"] = &fast_fields[FIELD_A];
  }

  // why o why does this not work when defined in Table
  inline TValue *operator[](TValue const &key) {
    if (fields.count(key)) {
      if (fields[key]->data.index() != TT_NULL) {
        // TT_OPT here means "optimized" -- unset
        return fields[key];
      }
      fields[key] = nullptr;
      return fields[key];
    }

    if (metatable != NULL && metatable->fields.count("__index")) {
      auto st = std::get<SpecialTable *>(metatable->fields["__index"]->data);
      return (*st)[key];
    }

    fields[key] = new TValue();
    return fields[key];
  }

  void set(uint16_t idx, TValue val) { fast_fields[idx] = val; }

  void sub(uint16_t idx, TValue val) {
    if (fast_fields[idx].data.index() == TT_NULL) {
      fast_fields[idx] = get(idx);
    }
    fast_fields[idx] -= val;
  }
  void inc(uint16_t idx, TValue val) {
    if (fast_fields[idx].data.index() == TT_NULL) {
      fast_fields[idx] = get(idx);
    }
    fast_fields[idx] += val;
  }
  TValue get(uint16_t idx) {
    TValue ret = fast_fields[idx];
    if (ret.data.index() == TT_NULL) {
      if (metatable != NULL && metatable->fields.count("__index")) {
        auto st = std::get<SpecialTable *>(metatable->fields["__index"]->data);
        return st->get(idx);
      }
      auto t = TValue();
      set(idx, t);
      return t;
    }
    return ret;
  }
};
#include "impl.cpp"
namespace Game {
  TValue t;
  TValue u;
  TValue o;
  TValue e;
  TValue d;
  TValue c;
  TValue b;
  TValue a;
  void __preinit();
  TValue main();

  TValue main() {
    TValue x;
    x = fix32(1);
    x += fix32(2);
    a = fix32(1);
    b = fix32(2);
    c = flr(fix32(1.5f)); // ?
    d = flr(fix32(2.5f)); // ?
    e = fix32(1);
    e = fix32(2);
    o = new SpecialTable();
    u = new SpecialTable({{"a", new TValue(fix32(1))}});
    std::get<SpecialTable *>(o.data)->set(FIELD_A, std::get<SpecialTable *>(u.data)->get(FIELD_A));
    t = fix32(7);
    t = *((*new SpecialTable({{fix32(7), new TValue(fix32(8))}, {fix32(8), new TValue(fix32(7))}}))[t]); // ?
    print(a * c);
    print(b * d);
    print(e);
    print(std::get<SpecialTable *>(o.data)->get(FIELD_A));
    print(t);
    return fix32(0);
  }

  void __preinit() {}
} // namespace Game
