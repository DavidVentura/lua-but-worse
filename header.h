#include <initializer_list>
#include <variant>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "fix32.h"
#include <unordered_map>
#include <functional>
#include <memory>
using namespace z8;

#define assertm(exp, msg) assert(((void)msg, exp))

// must match with variant type order
const uint8_t TT_NUM  = 0;
const uint8_t TT_STR  = 1;
const uint8_t TT_TAB  = 2;
const uint8_t TT_NULL = 3;
const uint8_t TT_BOOL = 4;
const uint8_t TT_FN   = 5;

class TValue;
class SpecialTable;
void print(const TValue t);


class FWrapper {
    // this std::function is 24 bytes, which makes the variant 24 bytes
    // which makes every number / etc 24 bytes. due to the million unnecessary copies
    // that are currently triggered, it makes some actions significantly slower
    // Wrapping it in a pointer makes function accesses slower, but those are infrequent
    public:
        using ftype = std::function<TValue(std::vector<TValue> args)>;
        ftype func;

    FWrapper(ftype val) : func(val) {}
};

class TValue {
    public:
        using func = std::function<TValue(std::vector<TValue> args)>;

        using myvariant = std::variant<fix32,
                                       const char*,
                                       SpecialTable*,
                                       std::nullptr_t,
                                       bool,
                                       FWrapper*
                                       >;
        myvariant data;
    // size_t hash     = 0;
    operator int16_t() const {
        return std::get<fix32>(data);
    };

    operator uint8_t() const {
        //assertm(n<(int16_t)255, "Tried to cast >255 to int8_t");
        //assertm(n>(int16_t)0, "Tried to cast <0 to int8_t");
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
        if(data.index()==TT_BOOL) return std::get<bool>(data);
        return true;
    };

    inline bool operator >(fix32 o) {
        return std::get<fix32>(data) > o;
    }
    inline bool operator >(TValue o) {
        return std::get<fix32>(data) > std::get<fix32>(o.data);
    }

    inline bool operator <(TValue o) {
        return std::get<fix32>(data) < std::get<fix32>(o.data);
    }
    inline bool operator >=(fix32 o) {
        return std::get<fix32>(data) >= o;
    }
    inline bool operator >(int o) {
        return std::get<fix32>(data) > fix32(o);
    }
    inline bool operator >=(int o) {
        return std::get<fix32>(data) >= fix32(o);
    }
    inline bool operator <=(int o) {
        return std::get<fix32>(data) <= fix32(o);
    }
    inline bool operator <=(fix32 o) {
        return std::get<fix32>(data) <= o;
    }
    inline bool operator <=(TValue o) {
        return std::get<fix32>(data) <= std::get<fix32>(o.data);
    }
    inline bool operator <(fix32 o) {
        return std::get<fix32>(data) < o;
    }

    inline bool operator ==(int o) {
        return std::get<fix32>(data) == fix32(o);
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

    inline TValue operator +(TValue o) {
        return TValue(std::get<fix32>(data) + std::get<fix32>(o.data));
    }

    inline TValue operator +(int o) {
        return TValue(std::get<fix32>(data) + fix32(o));
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
        return std::get<TT_FN>(data)->func(args);
    }

    bool operator ==(TValue other) {
        if(data.index()!=other.data.index()) return false;
        switch(data.index()) {
            case TT_NUM:
                return std::get<fix32>(data) == std::get<fix32>(other.data);
            case TT_STR: // FIXME: by pointer identity
                return std::get<const char*>(data) == std::get<const char*>(other.data);
            case TT_BOOL:
                return std::get<bool>(data) == std::get<bool>(other.data);
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

    bool operator ==(const TValue other) const {
        if(data.index()!=other.data.index()) return false;
        switch(data.index()) {
            case TT_NUM:
                return std::get<fix32>(data) == std::get<fix32>(other.data);
            case TT_STR: // FIXME: by pointer identity
                return std::get<const char*>(data) == std::get<const char*>(other.data);
            case TT_BOOL:
                return std::get<bool>(data) == std::get<bool>(other.data);
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
        return *this;
    }
    TValue& operator=(int val) {
        data = fix32(val);
        return *this;
    }
    TValue& operator=(bool val) {
        data = val ? 1 : 0;
        return *this;
    }

    TValue& operator=(SpecialTable* val) {
        data = val;
        return *this;
    }

    TValue() {
        data = nullptr;
    }
    TValue(fix32 val) {
        data = val;
    }
    TValue(int val) {
        data = fix32(val);
    }
    TValue(bool val) {
        data = val ? 1 : 0;
    }
    TValue(const char* val) {
        data = val;
    }

    TValue(SpecialTable* val) {
        data = val;
    }

    TValue(std::function<TValue(std::vector<TValue>)> val) {
        data = new FWrapper(val);
    }
    TValue& operator= (std::function<TValue(std::vector<TValue>)> val) {
        data = new FWrapper(val);
        return *this;
    }

    /*
    TValue(TValue&& other) {
        data = other.data;
        other.data = nullptr;
    }
    TValue& operator=(TValue&& other) {
        if(this != &other) {
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }
    */

}; // TValue;


TValue operator* (fix32 x, const TValue& y)
{
    return std::get<fix32>(y.data) * x;
}

TValue operator< (fix32 x, const TValue& y)
{
    return std::get<fix32>(y.data) < x;
}
TValue operator< (TValue x, int o)
{
    return std::get<fix32>(x.data) < fix32(o);
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
            case TT_BOOL:
                return std::get<bool>(s.data);
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

inline TValue _and(TValue left, TValue right) {
    // The operator and returns its first argument if it is false; otherwise, it returns its second argument.
    if (!left) return left;
    return right;
}
inline TValue _or(TValue left, TValue right) {
    // The operator or returns its first argument if it is not false; otherwise, it returns its second argument:
    if (left) return left;
    return right;
}
