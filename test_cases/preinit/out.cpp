#include "header.h"
const uint16_t FIELD_METHOD = 0;
const TValue *idx_to_name[1] = {new TValue("method")};

class SpecialTable : public Table {

public:
  TValue fast_fields[1];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 1; i++)
      fast_fields[i] = TValue();

    fields["method"] = &fast_fields[FIELD_METHOD];
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

  void main() { print(a); }

  void __preinit() {
    a = fix32(5);
    a += fix32(1);

    if ((a < fix32(3))) {
      a = fix32(7);
    }
    b = new SpecialTable();
    std::get<SpecialTable *>(b.data)->set(FIELD_METHOD, TValue([&](std::vector<TValue> args) -> TValue {
                                            TValue self = get_with_default(args, 0);
                                            print("method");
                                          }));
  }
} // namespace Game
