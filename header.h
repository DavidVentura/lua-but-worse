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
} tt;

class Table;

typedef struct TValue {
    union {
        fix32       n;
        const char* s;
        Table*      t;
    };
    tt tag          = TT_NULL;

    operator int8_t() const {
        assertm(tag==TT_NUM, "Type was not number");
        assertm(n<(int16_t)128, "Tried to cast >128 to int8_t");
        assertm(n>(int16_t)-127, "Tried to cast <-127 to int8_t");
        return n;
    };
    operator fix32() const {
        assertm(tag==TT_NUM, "Type was not number");
        return n;
    };

    operator const char*() const {
        assertm(tag==TT_STR, "Type was not STR");
        return s;
    };

    operator bool() const {
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
        return TValue(n * o);
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
                return false;
        }
        assertm(false, "Nothing to compare");
        return false;
    }

    TValue() = default;
    TValue(fix32 val) {
        tag = TT_NUM;
        n = val;
    }
    TValue(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
    }
    TValue(const char* val) {
        tag = TT_STR;
        s = val;
    }

    TValue(Table* val) {
        tag = TT_TAB;
        t = val;
    }

} TValue;

void print(const TValue t);


template<>
struct std::hash<TValue>
{
    inline std::size_t operator()(TValue const& s) const noexcept
    {
        switch(s.tag) {
            case TT_STR:
                // poor man's hash
                // most strings are "x1" or "x2" etc
                return s.s[0] ^ s.s[1];
            case TT_TAB:
                return (std::size_t)s.t; // unique by address
            case TT_NUM:
                return s.n.bits(); // unique by value, as bits
            case TT_FN:
                assertm(false, "Can't hash a function yet");
                return 0;
            case TT_NULL:
                return 0; // all TT_NULL have the same hash
        }
        assertm(false, "Unhandled case");
        return 0;
    }
};

TValue NULL_TV = TValue();

class Table
{
    public:
        std::unordered_map<TValue, TValue> fields;
        Table* metatable = NULL;
        uint16_t last_auto_index = 0;

        Table(std::initializer_list<std::pair<const TValue, TValue>> values): fields(values) {
            metatable = NULL;
        }

        Table(std::unordered_map<TValue, TValue> values) {
            fields = values;
            metatable = NULL;
        }

        Table() {
            metatable = NULL;
        }

        TValue& operator[](TValue const& key) {
            if(fields.count(key)) {
                return fields[key];
            }

            if(metatable!=NULL && metatable->fields.count("__index")) {
                fields[key] = (*(metatable->fields["__index"].t))[key];
                return fields[key];
            }

            fields[key] = NULL_TV;
            return fields[key];
        }

};

void setmetatable(TValue t, TValue meta) {
    assert(t.tag == TT_TAB);
    assert(meta.tag == TT_TAB);
    t.t->metatable = meta.t;
}

extern uint8_t btn(uint8_t);
void print(const TValue t) {
    switch(t.tag) {
        case TT_STR:
            printf("%s\n", t.s);
            break;
        case TT_NUM:
            {
                fix32 _dec = fix32::decimals(t.n);
                if(_dec > fix32(0)) {
                    char buf[17];
                    fix32::to_string(_dec, buf);
                    printf("%d.%s\n", int16_t(t.n), buf);
                } else {
                    printf("%d\n", int16_t(t.n));
                }
                break;
            }
        case TT_NULL:
            printf("NULL\n");
            break;
        case TT_TAB:
            printf("T<%d>\n", t.t);
            break;
    }
}
#if defined(SDL_BACKEND) || defined(ESP_BACKEND) || defined(PICO_BACKEND)
void print(TValue value, int16_t x, int16_t y, int16_t col) {
    static char numbuf[23];
    if(value.tag == TT_STR) {
        _print(value.s, strlen(value.s), x, y, col);
    }
    if(value.tag == TT_NUM) {
        int16_t decimals = (uint16_t)fix32::decimals(value.n);
        if (decimals) {
            char buf[17];
            fix32::to_string(decimals, buf);
            int len = sprintf(numbuf, "%d.%s", (uint16_t)value.n, buf);
            _print(numbuf, len, x, y, col);
        } else {
            int len = sprintf(numbuf, "%d", (uint16_t)value.n);
            _print(numbuf, len, x, y, col);
        }
    }
}
#endif

void foreach(TValue val, std::function<void (TValue)> f) {
    assertm(val.tag == TT_TAB, "Can't foreach a non-table");
    // standard for instead of magic iterator as items can be deleted from the table
    // during iteration
    for (auto it = val.t->fields.cbegin(); it != val.t->fields.cend(); /* no increment */) {
        auto next = it;
        ++next;
        f(it->second);
        it = next;
    }
}

fix32 count(TValue val) {
    assertm(val.tag == TT_TAB, "Can't count a non-table");
    // p8 limits integers to uint16_t (SHRT_MAX) .. also there's not enough memory anyway
    // to store so many items
    return (uint16_t)val.t->fields.size();
}

fix32 rnd(float limit = 1.0f) {
    float x = (float)rand()/(float)(RAND_MAX/limit);
    return x;
}

fix32 flr(fix32 n) {
    fix32 ret = fix32::floor(n);
    return ret;
}

TValue add(TValue table, TValue val) {
    table.t->last_auto_index++;
    (*table.t)[table.t->last_auto_index] = val;
    return val;
}

void del(TValue table, TValue val) {
    assertm(table.tag==TT_TAB, "Tried to delete from a non-table");
    for (auto it = table.t->fields.cbegin(); it != table.t->fields.cend(); ++it) {
        if (it->second == val) {
            table.t->fields.erase(it);
            break;
        }
    }
}
