#include "header.h"
const uint16_t FIELD___INDEX = 0;
const uint16_t FIELD_X = 1;
const uint16_t FIELD_Y = 2;
const TValue *idx_to_name[3] = {new TValue("__index"), new TValue("x"), new TValue("y")};

class SpecialTable : public Table {

public:
  TValue fast_fields[3];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 3; i++)
      fast_fields[i] = TValue::OPT_VAL();

    fields["__index"] = &fast_fields[FIELD___INDEX];
    fields["x"] = &fast_fields[FIELD_X];
    fields["y"] = &fast_fields[FIELD_Y];
  }

  // why o why does this not work when defined in Table
  inline TValue *operator[](TValue const &key) {
    if (fields.count(key)) {
      if (!fields[key]->is_opt) {
        // TT_OPT here means "optimized" -- unset
        return fields[key];
      }
      fields[key]->is_opt = false; // make NULL
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
    if (fast_fields[idx].is_opt) {
      fast_fields[idx] = get(idx);
    }
    fast_fields[idx] -= val;
  }
  void inc(uint16_t idx, TValue val) {
    if (fast_fields[idx].is_opt) {
      fast_fields[idx] = get(idx);
    }
    fast_fields[idx] += val;
  }
  TValue get(uint16_t idx) {
    TValue ret = fast_fields[idx];
    if (ret.is_opt) {
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
  TValue b;
  TValue a;
  void __preinit();
  TValue main();

  TValue main() {
    a = new SpecialTable({{"x", new TValue(5)}});
    std::get<SpecialTable *>(a.data)->set(FIELD___INDEX, a);
    b = new SpecialTable();
    setmetatable(b, a);
    print(std::get<SpecialTable *>(b.data)->get(FIELD_X));
    std::get<SpecialTable *>(b.data)->inc(FIELD_X, 5);
    print(std::get<SpecialTable *>(b.data)->get(FIELD_X));
    print(std::get<SpecialTable *>(b.data)->get(FIELD_Y));
    std::get<SpecialTable *>(a.data)->set(FIELD_Y, 7);
    print(std::get<SpecialTable *>(b.data)->get(FIELD_Y));
    return 0;
  }

  void __preinit() {}
} // namespace Game
