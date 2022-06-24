#include "header.h"
const uint16_t FIELD_F = 0;
const TValue *idx_to_name[1] = {new TValue("f")};

class SpecialTable : public Table {

public:
  TValue fast_fields[1];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 1; i++)
      fast_fields[i] = TValue();

    fields["f"] = &fast_fields[FIELD_F];
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
  TValue v;
  TValue c;
  TValue b;
  TValue a;
  TValue captured;
  void __preinit();
  TValue main();

  TValue main() {
    captured = fix32(7);
    a = TValue([&](std::vector<TValue> args) -> TValue {
      TValue x = get_with_default(args, 0);
      return x * captured;
    });
    print(a({fix32(5)}));
    b = TValue([&](std::vector<TValue> args) -> TValue {
      TValue x = get_with_default(args, 0);
      TValue y = get_with_default(args, 1);
      return x * y;
    });
    print(b({fix32(5), fix32(6)}));
    c = new SpecialTable();
    std::get<SpecialTable *>(c.data)->set(FIELD_F, TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table"; }));
    print(std::get<SpecialTable *>(c.data)->get(FIELD_F)({}));
    v = "index";
    (*(*std::get<SpecialTable *>(c.data))[v]) =
        TValue([&](std::vector<TValue> args) -> TValue { return "works inside a table, via hashmap"; }); // ?
    print((*(*std::get<SpecialTable *>(c.data))[v])({}));
    return fix32(0);
  }

  void __preinit() {}
} // namespace Game
