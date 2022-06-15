const fix32 ZERO = fix32(0);

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
                if(_dec > ZERO) {
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
            printf("F<%x>\n", (size_t)t.t);
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

inline fix32 count(TValue val) {
    assertm(val.tag == TT_TAB, "Can't count a non-table");
    // p8 limits integers to uint16_t (SHRT_MAX) .. also there's not enough memory anyway
    // to store so many items
    return (uint16_t)val.t->fields.size();
}

inline fix32 rnd(float limit = 1.0f) {
    float x = (float)rand()/(float)(RAND_MAX/limit);
    return x;
}

inline fix32 flr(fix32 n) {
    fix32 ret = fix32::floor(n);
    return ret;
}

TValue add(TValue table, TValue val) {
    assertm(table.tag==TT_TAB, "Tried to add from a non-table");
    table.t->last_auto_index++;
    *(*table.t)[fix32(table.t->last_auto_index)] = val;
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

inline TValue sget(TValue x, TValue y) {
#if defined(SDL_BACKEND) || defined(ESP_BACKEND) || defined(PICO_BACKEND)
    return get_pixel(x, y);
#else
    return TValue(fix32(0));
#endif
}
inline void pset(TValue x, TValue y, TValue color) {
#if defined(SDL_BACKEND) || defined(ESP_BACKEND) || defined(PICO_BACKEND)
    _pset(x, y, color);
#endif
}

inline TValue _or(TValue a, TValue b) {
    if(a.tag!=TT_NULL) return a;
    return b;
}
