#include "header.h"
const uint16_t FIELD_TOP = 0;
const uint16_t FIELD_Y = 1;
const TValue *idx_to_name[2] = {new TValue("top"), new TValue("y")};

class SpecialTable : public Table {

public:
  TValue fast_fields[2];

  SpecialTable(std::initializer_list<std::pair<const TValue, TValue *>> values) : SpecialTable() { prepopulate(values); }

  SpecialTable() {
    for (uint16_t i = 0; i < 2; i++)
      fast_fields[i] = TValue();

    fields["top"] = &fast_fields[FIELD_TOP];
    fields["y"] = &fast_fields[FIELD_Y];
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
  TValue literals;
  TValue empty;
  TValue member;
  TValue squares;
  void __preinit();
  void main();

  void main() {
    squares = new SpecialTable({{fix32(1), new TValue(fix32(1))},
                                {fix32(2), new TValue(fix32(4))},
                                {fix32(3), new TValue(fix32(9))},
                                {fix32(4), new TValue(fix32(16))},
                                {fix32(5), new TValue(fix32(25))}});
    member = new SpecialTable({{"x", new TValue(fix32(1))}});
    empty = new SpecialTable();
    literals = new SpecialTable({{"top", new TValue(new SpecialTable({{"x", new TValue(fix32(0))}, {"y", new TValue(-fix32(68))}}))},
                                 {"bottom", new TValue(new SpecialTable({{"x", new TValue(fix32(0))}, {"y", new TValue(fix32(68))}}))},
                                 {"left", new TValue(new SpecialTable({{"x", new TValue(-fix32(68))}, {"y", new TValue(fix32(0))}}))},
                                 {"right", new TValue(new SpecialTable({{"x", new TValue(fix32(68))}, {"y", new TValue(fix32(0))}}))}});
    print(std::get<SpecialTable *>(std::get<SpecialTable *>(literals.data)->get(FIELD_TOP).data)->get(FIELD_Y));
    (*(*std::get<SpecialTable *>(member.data))[empty]) = fix32(5); // ?
    print((*(*std::get<SpecialTable *>(member.data))[empty]));
  }

  void __preinit() {}
} // namespace Game
