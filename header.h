#include <initializer_list>
#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "fix32.h"
#include <unordered_map>
#include <functional>
using namespace z8;

#define assertm(exp, msg) assert(((void)msg, exp))

typedef enum {
    TT_NUM     = 0,
    TT_STR,
    TT_TAB,
    TT_FN,
    TT_NULL,
    TT_OPT, // is a local pointer for faster access; should always be
            // replaced by assigning a different type
} tt;

class TValue;
class SpecialTable;
void print(const TValue t);

class TValue {
    public:
//    union {
        fix32                          n;
        const char*                    s;
        SpecialTable*                  t;
//    };
    tt tag          = TT_NULL;
    std::function<TValue(std::vector<TValue> args)> f;  // TODO move into variant or numbers are gonna be heavy
    // size_t hash     = 0;

    operator int16_t() const {
        assertm(tag==TT_NUM, "Type was not number");
        return n;
    };

    operator int8_t() const {
        assertm(tag==TT_NUM, "Type was not number");
        assertm(n<(int16_t)128, "Tried to cast >128 to int8_t");
        assertm(n>(int16_t)-127, "Tried to cast <-127 to int8_t");
        return n;
    };
    inline operator fix32() const {
        assertm(tag==TT_NUM, "Type was not number");
        return n;
    };

    operator const char*() const {
        assertm(tag==TT_STR, "Type was not STR");
        return s;
    };

    inline operator bool() const {
        if(tag==TT_NULL) return false;
        assertm(tag==TT_NUM, "Type was not number");
        return n != fix32(0);
    };

    inline bool operator >(fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        bool res = n > o;
        return res;
    }
    inline bool operator >(TValue o) {
        return n > o.n;
    }

    inline bool operator <(TValue o) {
        assertm(tag==TT_NUM, "Can't <t notnumber");
        return n < o.n;
    }
    inline bool operator >=(fix32 o) {
        assertm(tag==TT_NUM, "Can't <= notnumber");
        return n >= o;
    }
    inline bool operator <=(fix32 o) {
        assertm(tag==TT_NUM, "Can't <= notnumber");
        return n <= o;
    }
    inline bool operator <(fix32 o) {
        assertm(tag==TT_NUM, "Can't <f32 notnumber");
        return n < o;
    }

    inline bool operator ==(fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n == o;
    }

    inline bool operator !=(fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n != o;
    }

    inline TValue operator %(fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return TValue(n % o);
    }

    inline TValue operator *(fix32 o) {
        assertm(tag==TT_NUM, "Can't mult notnumber");
        return TValue(n * o);
    }

    inline TValue operator *(TValue o) {
        assertm(tag==TT_NUM,   "Can't mult (self) notnumber");
        assertm(o.tag==TT_NUM, "Can't mult (other) notnumber");
        return TValue(n * o.n);
    }

    inline TValue operator +(fix32 o) {
        return TValue(n + o);
    }

    inline TValue operator -(fix32 o) {
        return TValue(n - o);
    }

    inline TValue operator -() {
        return TValue(-n);
    }

    inline TValue operator /(fix32 o) {
        return TValue(n / o);
    }

    inline TValue operator -=(fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        n -= o;
        return *this;
    }

    inline TValue operator +=(fix32 o) {
        assertm(tag==TT_NUM, "Can't inc notnumber");
        n += o;
        return *this;
    }

    inline TValue operator()(std::vector<TValue> args) {
        assertm(tag==TT_FN, "Tried to call a non-function");
        return f(args);
    }

    inline bool operator ==(TValue other) {
        if(tag!=other.tag) return false;
        switch(tag) {
            case TT_NUM:
                return n == other.n;
            case TT_STR: // FIXME: by pointer identity
                return s == other.s;
            case TT_NULL:
                return true;
            case TT_TAB: // by pointer identity
                return t == other.t;
            case TT_FN:
                assertm(false, "Can't compare functions");
                return false;
            case TT_OPT:
                assertm(false, "Can't compare OPT values");
                return false;
        }
        assertm(false, "Nothing to compare");
        return false;
    }

    inline bool operator ==(const TValue &other) const {
        if(tag!=other.tag) return false;
        switch(tag) {
            case TT_NUM:
                return n == other.n;
            case TT_STR: // FIXME: by pointer identity
                return s == other.s;
            case TT_NULL:
                return true;
            case TT_TAB: // by pointer identity
                return t == other.t;
            case TT_FN:
                assertm(false, "Can't compare functions");
            case TT_OPT:
                assertm(false, "Can't compare OPT values");
                return false;
        }
        assertm(false, "Nothing to compare");
        return false;
    }

    TValue& operator=(fix32 val) {
        tag = TT_NUM;
        n = val;
        return *this;
    }
    TValue& operator=(int val) {
        tag = TT_NUM;
        n = fix32(val);
        return *this;
    }
    TValue& operator=(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
        return *this;
    }

    TValue& operator=(SpecialTable* val) {
        tag = TT_TAB;
        t = val;
        return *this;
    }

    TValue() {
        tag = TT_NULL;
    }
    TValue(fix32 val) {
        tag = TT_NUM;
        n = val;
        //hash = n.bits();
    }
    TValue(int val) {
        tag = TT_NUM;
        n = fix32(val);
    }
    TValue(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
        //hash = n.bits();
    }
    TValue(const char* val) {
        tag = TT_STR;
        s = val;
    }

    TValue(SpecialTable* val) {
        tag = TT_TAB;
        t = val;
        //hash = (size_t)val;
    }

    static TValue OPT_VAL() {
        TValue t = TValue();
        t.tag = TT_OPT;
        return t;
    }
    TValue& operator= (std::function<TValue(std::vector<TValue>)> val) {
        tag = TT_FN;
        f = val;
        return *this;
    }

}; // TValue;


TValue operator* (fix32 x, const TValue& y)
{
    assertm(y.tag == TT_NUM, "Tried to right-multiply a non-number");
    return y.n * x;
}

TValue operator< (fix32 x, const TValue& y)
{
    assertm(y.tag == TT_NUM, "Tried to < a non-number");
    return y.n < x;
}




template<>
struct std::hash<TValue> {
    inline std::size_t operator()(TValue const& s) const noexcept {
        switch(s.tag) {
            case TT_NUM:
                return s.n.bits();
            case TT_TAB:
                return (size_t)s.t;
            case TT_NULL:
                return 0x5a5a5a5a;
            case TT_STR:
                return (s.s[0] | s.s[1] << 1); // unless the strings are empty; they always have 2 bytes+
            case TT_FN:
            case TT_OPT:
                assertm(false, "Did not match any tag on hash");
                return 0;
        }
        //return s.hash;
        assertm(false, "Did not match any tag on hash");
        return 0;
    }
};

class Table
{
    public:
        std::unordered_map<TValue, TValue*> fields;

        Table* metatable = NULL;
        uint16_t last_auto_index = 0;

        void prepopulate(std::initializer_list<std::pair<const TValue, TValue*>> values) {
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
