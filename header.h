#include <cassert>
#include <cstdio>
#include <string>
#include "fix32.h"

#define assertm(exp, msg) assert(((void)msg, exp))

typedef enum {
    TT_NUM     = 0,
    TT_STR,
    TT_TAB,
    TT_FN,
    TT_NULL,
} tt;

typedef struct Table {
} Table;

typedef struct TValue {
    union {
        z8::fix32                              n;
        char*                                  s;
        std::unordered_map<std::string, TValue>*    t;
    };
    tt tag          = TT_NULL;

    operator z8::fix32() const {
        assertm(tag==TT_NUM, "Type was not number");
        return n;
    };

    operator char*() const {
        assertm(tag==TT_STR, "Type was not STR");
        return s;
    };

    operator bool() const {
        assertm(tag==TT_NUM, "Type was not number");
        return n != z8::fix32(0);
    };

    inline bool operator >(int o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        bool res = n > z8::fix32(o);
        return res;
    }
    inline bool operator >(TValue o) {
        return n > o.n;
    }
    inline bool operator <(TValue o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n < o.n;
    }
    inline bool operator <(int16_t o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n < z8::fix32(o);
    }

    inline bool operator ==(z8::fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n == o;
    }
    inline bool operator ==(int16_t o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        return n == z8::fix32(o);
    }

    inline TValue operator -=(z8::fix32 o) {
        assertm(tag==TT_NUM, "Can't dec notnumber");
        n -= o;
        return *this;
    }

    inline TValue operator +=(z8::fix32 o) {
        assertm(tag==TT_NUM, "Can't inc notnumber");
        n += o;
        return *this;
    }

    inline bool operator ==(const TValue other) const {
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
        }
        assertm(false, "Can't compare");
        return false;
    }

    TValue() = default;
    TValue(z8::fix32 val) {
        tag = TT_NUM;
        n = val;
    }
    TValue(int val) {
        tag = TT_NUM;
        n = z8::fix32(val);
    }
    TValue(bool val) {
        tag = TT_NUM;
        n = val ? 1 : 0;
    }
    TValue(char* val) {
        tag = TT_STR;
        s = val;
    }

} TValue;

extern uint8_t btn(uint8_t);
void print(const char* fmt, const TValue t) {
    if(t.tag == TT_STR) {
        printf(fmt, t.s);
    }
    if(t.tag == TT_NUM) {
        printf(fmt, (uint16_t)t.n, (int16_t)z8::fix32::decimals(t.n));
    }
}
void print(const std::initializer_list<TValue>& values) {
    for (auto& t : values) {

        if(t.tag == TT_STR) {
            printf("%s\n", t.s);
        }
        if(t.tag == TT_NUM) {
            printf("%d.%d\n", (uint16_t)t.n, (int16_t)z8::fix32::decimals(t.n));
        }
    }
}

TValue _div(TValue t, int16_t v) {
    TValue ret = TValue(0);
    assert(t.tag == TT_NUM);
    ret.n = t.n / (int16_t)v;
    return ret;
};

TValue _add(TValue t, int v) {
    assert(t.tag == TT_NUM);
    return TValue(t.n + z8::fix32(v));
};

TValue _add(TValue t, z8::fix32 v) {
    assert(t.tag == TT_NUM);
    return TValue(t.n + v);
};

TValue _add(TValue t, TValue v) {
    assert(t.tag == TT_NUM);
    assert(v.tag == TT_NUM);
    return TValue(t.n + v.n);
};

TValue _sub(TValue t, z8::fix32 v) {
    assert(t.tag == TT_NUM);
    return TValue(t.n - v);
};

TValue _sub(TValue t, TValue v) {
    assert(t.tag == TT_NUM);
    assert(v.tag == TT_NUM);
    return TValue(t.n - v.n);
};

void foreach(TValue val, std::function<TValue* (TValue)> f) {
    // assertm(t.tag == TT_TAB, "Can't foreach a non-table");
    // standard for instead of magic iterator as items can be deleted from the table
    // during iteration
    for (auto it = (*val.t).cbegin(); it != (*val.t).cend(); /* no increment */) {
        auto next = it;
        ++next;
        f(it->second);
        it = next;
    }
}

z8::fix32 count(TValue val) {
    // assertm(t.tag == TT_TAB, "Can't count a non-table");
    // p8 limits integers to uint16_t // SHRT_MAX .. also there's not enough memory anyway
    // to store so many items
    return (uint16_t)val.t->size();
}

z8::fix32 rnd(float limit = 1.0f) {
    float x = (float)rand()/(float)(RAND_MAX/limit);
    return x;
}

z8::fix32 flr(z8::fix32 n) {
    z8::fix32 ret = z8::fix32::floor(n);
    return ret;
}

void add(TValue table, TValue val) {
    char* key = (char*)malloc(8);
    sprintf(key, "%d", (uint16_t)((*table.t).size()));
    (*table.t)[key] = val;
    free(key);
}

void del(TValue table, TValue val) {
    for (auto it = (*table.t).cbegin(); it != (*table.t).cend(); ++it)
    {
        if (it->second == val) {
            (*table.t).erase(it);
            break;
        }
    }
}
