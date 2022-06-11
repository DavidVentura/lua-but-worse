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

class Table;

typedef struct TValue {
    union {
        fix32       n;
        const char* s;
        Table*      t;
    };
    tt tag          = TT_NULL;
    size_t hash     = 0;

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
        hash = n.bits();
        return *this;
    }
    TValue& operator=(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
        hash = n.bits();
        return *this;
    }

    TValue& operator=(Table* val) {
        tag = TT_TAB;
        t = val;
        hash = (size_t)val;
        return *this;
    }

    TValue() {
        tag = TT_NULL;
        hash = 0x5a5a5a5a;
    }
    TValue(fix32 val) {
        tag = TT_NUM;
        n = val;
        hash = n.bits();
    }
    TValue(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
        hash = n.bits();
    }
    TValue(const char* val) {
        tag = TT_STR;
        s = val;
        hash = (val[0] | val[1] << 1); // unless the strings are empty; they always have 2 bytes+
    }

    TValue(Table* val) {
        printf("Creating value from Table*\n");
        tag = TT_TAB;
        t = val;
        hash = (size_t)val;
    }

    static TValue OPT_VAL() {
        TValue t = TValue();
        t.tag = TT_OPT;
        return t;
    }

} TValue;

void print(const TValue t);


template<>
struct std::hash<TValue> {
    inline std::size_t operator()(TValue const& s) const noexcept {
        return s.hash;
    }
};

class Table
{
    public:
        std::unordered_map<TValue, TValue*> fields;
        Table* metatable = NULL;
        uint16_t last_auto_index = 0;

        TValue x    = TValue::OPT_VAL();
        TValue y    = TValue::OPT_VAL();
        TValue dx   = TValue::OPT_VAL();
        TValue dy   = TValue::OPT_VAL();
        TValue spr  = TValue::OPT_VAL();
        TValue x1   = TValue::OPT_VAL();
        TValue y1   = TValue::OPT_VAL();
        TValue x2   = TValue::OPT_VAL();
        TValue y2   = TValue::OPT_VAL();

        Table(std::initializer_list<std::pair<const TValue, TValue*>> values) : Table() {
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

        /*
        Table(std::unordered_map<TValue, TValue> values) {
            fields = values;
            metatable = NULL;
        }
        */

        Table() {
            fields["x1"]     = &x1;
            fields["x2"]     = &x2;
            fields["y1"]     = &y1;
            fields["y2"]     = &y2;
            fields["x"]      = &x;
            fields["y"]      = &y;
            fields["dx"]     = &dx;
            fields["dy"]     = &dy;
            fields["spr"]    = &spr;
        }

        inline TValue* operator[](TValue const& key) {
            if(fields.count(key) && fields[key]->tag != TT_OPT) {
                // TT_OPT here means "optimized" -- unset
                return fields[key];
            }

            if(metatable!=NULL && metatable->fields.count("__index")) {
                fields[key] = (*(metatable->fields["__index"]->t))[key];
                return fields[key];
            }

            fields[key] = new TValue();
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
        case TT_FN:
            assertm(false, "Can't print a function yet");
            break;
        case TT_TAB:
            printf("T<%x>\n", (size_t)t.t);
            break;
        case TT_OPT:
            printf("OPT token at 0x%X\n", (size_t)&t);
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
        if(it->second->tag != TT_OPT) {
            f(*it->second);
        }
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
    assertm(table.tag==TT_TAB, "Tried to add from a non-table");
    table.t->last_auto_index++;
    *(*table.t)[fix32(table.t->last_auto_index)] = val;
    // printf("Table has %d\n", table.t->fields.size());
    return val;
}

void del(TValue table, TValue val) {
    assertm(table.tag==TT_TAB, "Tried to delete from a non-table");
    for (auto it = table.t->fields.cbegin(); it != table.t->fields.cend(); ++it) {
        if (*it->second == val) {
            if(it->second->tag == TT_TAB) {
                delete it->second->t;
                delete it->second;
            }
            table.t->fields.erase(it);
            break;
        }
    }
}
