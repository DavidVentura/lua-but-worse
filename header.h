#include <assert.h>
#include <cstdio>
#include "fix32.h"

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
        z8::fix32 n;
        char*     s;
        Table*    t;
    };
    tt tag          = TT_NULL;
} TValue;

#define TVfromnumber(x) TValue{.n = x, .tag = TT_NUM}

extern uint8_t btn(uint8_t);
void print(char const* c) {
    printf("%s\n", c);
}

TValue _add(TValue t, z8::fix32 v) {
    assert(t.tag == TT_NUM);
    t.n += v;
    return t;
};

TValue _sub(TValue t, z8::fix32 v) {
    assert(t.tag == TT_NUM);
    t.n -= v;
    return t;
};
