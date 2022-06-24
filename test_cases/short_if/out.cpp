#include "header.h"

const TValue *idx_to_name[0] = {};

class SpecialTable : public Table {

public:
  TValue fast_fields[0];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 0; i++)
      fast_fields[i] = TValue();
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
  TValue e;
  TValue d;
  TValue c;
  TValue b;
  TValue a;
  TValue test;
  void __preinit();
  TValue main();

  TValue main() {
    test = false;

    if (test) {
      return fix32(5);
    }
    print("asd");

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

  void __preinit() {}
} // namespace Game
