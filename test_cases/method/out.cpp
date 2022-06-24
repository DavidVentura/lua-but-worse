#include "header.h"
const uint16_t FIELD___INDEX = 0;
const uint16_t FIELD_METHOD = 1;
const uint16_t FIELD_X = 2;
const TValue *idx_to_name[3] = {new TValue("__index"), new TValue("method"), new TValue("x")};

class SpecialTable : public Table {

public:
  TValue fast_fields[3];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 3; i++)
      fast_fields[i] = TValue();

    fields["__index"] = &fast_fields[FIELD___INDEX];
    fields["method"] = &fast_fields[FIELD_METHOD];
    fields["x"] = &fast_fields[FIELD_X];
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
  TValue b;
  TValue a;
  void __preinit();
  void main();

  void main() {
    a = new SpecialTable();
    std::get<SpecialTable *>(a.data)->set(FIELD___INDEX, a);
    std::get<SpecialTable *>(a.data)->set(FIELD_METHOD, TValue([&](std::vector<TValue> args) -> TValue {
                                            TValue self = get_with_default(args, 0);
                                            std::get<SpecialTable *>(self.data)->set(FIELD_X, fix32(5));
                                            return TValue();
                                          }));
    b = new SpecialTable();
    setmetatable(b, a);
    std::get<SpecialTable *>(b.data)->get(FIELD_METHOD)({b});
    print(std::get<SpecialTable *>(a.data)->get(FIELD_X));
    print(std::get<SpecialTable *>(b.data)->get(FIELD_X));
    std::get<SpecialTable *>(a.data)->get(FIELD_METHOD)({a});
    print(std::get<SpecialTable *>(a.data)->get(FIELD_X));
    print(std::get<SpecialTable *>(b.data)->get(FIELD_X));
  }

  void __preinit() {}
} // namespace Game
