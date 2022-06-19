#include <initializer_list>
#include <variant>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "fix32.h"
#include <unordered_map>
#include <functional>
using namespace z8;

#define assertm(exp, msg) assert(((void)msg, exp))

// must match with variant type order
const uint8_t TT_NUM  = 0;
const uint8_t TT_STR  = 1;
const uint8_t TT_TAB  = 2;
const uint8_t TT_FN   = 3;
const uint8_t TT_NULL = 4;

class TValue;
class SpecialTable;
void print(const TValue t);

class TValue {
    public:
        using func = std::function<TValue(std::vector<TValue> args)>;

        using myvariant = std::variant<fix32,
                                       const char*,
                                       SpecialTable*,
                                       func,
                                       std::nullptr_t
                                       >;
        myvariant data;
        bool is_opt = false;
    // size_t hash     = 0;
    operator int16_t() const {
        return std::get<fix32>(data);
    };

    operator int8_t() const {
        //assertm(n<(int16_t)128, "Tried to cast >128 to int8_t");
        //assertm(n>(int16_t)-127, "Tried to cast <-127 to int8_t");
        return std::get<fix32>(data);
    };
    inline operator fix32() const {
        return std::get<fix32>(data);
    };

    operator const char*() const {
        return std::get<const char*>(data);
    };

    inline operator bool() const {
        if(data.index()==TT_NULL) return false;
        return std::get<fix32>(data) != fix32(0);
    };

    inline bool operator >(fix32 o) {
        return std::get<fix32>(data) > o;
    }
    /*
    inline bool operator >(TValue o) {
        return std::get<fix32>(data) > std::get<fix32>(o.data);
    }

    inline bool operator <(TValue o) {
        assertm(tag==TT_NUM, "Can't <t notnumber");
        return data < o.data;
        return std::get<fix32>(data) < std::get<fix32>(o.data);
    }
    */
    inline bool operator >=(fix32 o) {
        return std::get<fix32>(data) >= o;
    }
    inline bool operator <=(fix32 o) {
        return std::get<fix32>(data) <= o;
    }
    inline bool operator <(fix32 o) {
        return std::get<fix32>(data) < o;
    }

    inline bool operator ==(fix32 o) {
        return std::get<fix32>(data) == o;
    }

    inline bool operator !=(fix32 o) {
        return std::get<fix32>(data) != o;
    }

    inline TValue operator %(fix32 o) {
        return std::get<fix32>(data) % o;
    }

    inline TValue operator *(fix32 o) {
        return std::get<fix32>(data) * o;
    }

    inline TValue operator *(TValue o) {
        return std::get<fix32>(data) * std::get<fix32>(o.data);
    }

    inline TValue operator +(fix32 o) {
        return TValue(std::get<fix32>(data) + o);
    }

    inline TValue operator -(fix32 o) {
        return TValue(std::get<fix32>(data) - o);
    }

    inline TValue operator -() {
        return TValue(-std::get<fix32>(data));
    }

    inline TValue operator /(fix32 o) {
        return TValue(std::get<fix32>(data) / o);
    }

    inline TValue operator -=(fix32 o) {
        data = std::get<fix32>(data) - o;
        return *this;
    }

    inline TValue operator +=(fix32 o) {
        data = std::get<fix32>(data) + o;
        return *this;
    }

    inline TValue operator()(std::vector<TValue> args) {
        return std::get<func>(data)(args);
    }

    bool operator ==(TValue other) {
        if(data.index()!=other.data.index()) return false;
        switch(data.index()) {
            case TT_NUM:
                return std::get<fix32>(data) == std::get<fix32>(other.data);
            case TT_STR: // FIXME: by pointer identity
                return std::get<const char*>(data) == std::get<const char*>(other.data);
            case TT_NULL:
                return true;
            case TT_TAB: // by pointer identity
                return std::get<SpecialTable*>(data) == std::get<SpecialTable*>(other.data);
            case TT_FN:
                assertm(false, "Can't compare functions");
                return false;
        }
        assertm(false, "Nothing to compare");
        return false;
    }

    const bool operator ==(const TValue other) const {
        if(data.index()!=other.data.index()) return false;
        switch(data.index()) {
            case TT_NUM:
                return std::get<fix32>(data) == std::get<fix32>(other.data);
            case TT_STR: // FIXME: by pointer identity
                return std::get<const char*>(data) == std::get<const char*>(other.data);
            case TT_NULL:
                return true;
            case TT_TAB: // by pointer identity
                return std::get<SpecialTable*>(data) == std::get<SpecialTable*>(other.data);
            case TT_FN:
                assertm(false, "Can't compare functions");
                return false;
        }
        assertm(false, "Nothing to compare");
        return false;
    }

    TValue& operator=(fix32 val) {
        data = val;
        is_opt = false;
        return *this;
    }
    TValue& operator=(int val) {
        data = fix32(val);
        is_opt = false;
        return *this;
    }
    TValue& operator=(bool val) {
        data = val ? 1 : 0;
        is_opt = false;
        return *this;
    }

    TValue& operator=(SpecialTable* val) {
        data = val;
        is_opt = false;
        return *this;
    }

    TValue() {
        data = nullptr;
        is_opt = false;
    }
    TValue(fix32 val) {
        data = val;
        is_opt = false;
    }
    TValue(int val) {
        data = fix32(val);
        is_opt = false;
    }
    TValue(bool val) {
        data = val ? 1 : 0;
        is_opt = false;
    }
    TValue(const char* val) {
        data = val;
        is_opt = false;
    }

    TValue(SpecialTable* val) {
        data = val;
        is_opt = false;
    }

    TValue(std::function<TValue(std::vector<TValue>)> val) {
        data = val;
        is_opt = false;
    }

    static TValue OPT_VAL() {
        TValue t = TValue();
        t.is_opt = true;
        return t;
    }
    TValue& operator= (std::function<TValue(std::vector<TValue>)> val) {
        is_opt = false;
        data = val;
        return *this;
    }

}; // TValue;


TValue operator* (fix32 x, const TValue& y)
{
    return std::get<fix32>(y.data) * x;
}

TValue operator< (fix32 x, const TValue& y)
{
    return std::get<fix32>(y.data) < x;
}



template<>
struct std::hash<TValue> {
    inline std::size_t operator()(TValue const& s) const noexcept {
        switch(s.data.index()) {
            case TT_NUM:
                return std::get<fix32>(s.data).bits();
            case TT_TAB:
                return (size_t)std::get<SpecialTable*>(s.data);
            case TT_NULL:
                return 0x5a5a5a5a;
            case TT_STR:
                {
                auto c = std::get<const char*>(s.data);
                return (c[0] | c[1] << 1); // unless the strings are empty; they always have 2 bytes+
                }
            case TT_FN:
                assertm(false, "Did not match any tag on hash");
                return 0;
        }
        assertm(false, "Did not match any tag on hash");
        return 0;
    }
};

class Table
{
    public:
        using pair = std::pair<TValue, TValue*>;
        using cpair = std::pair<const TValue, TValue*>;
        std::unordered_map<TValue, TValue*> fields;

        Table* metatable = NULL;
        uint16_t last_auto_index = 0;

        void prepopulate(std::initializer_list<cpair> values) {
            for(auto [key, val]: values) {
                if(fields.count(key)) {
                    // pre-populated part of the map with a reference
                    // to a local class member
                    *fields[key] = *val;
                } else {
                    // dynamic key
                    fields[key] = val;
                }
            }

        }

};

TValue get_with_default(const std::vector<TValue>& v, uint8_t idx) {
    if(idx>= v.size())
        return TValue();
    return v[idx];
}
