#pragma once

typedef volatile intptr_t TAtomic;

static inline intptr_t AtomicIncrement(TAtomic& p) {
    return ++p;
}

static inline intptr_t AtomicDecrement(TAtomic& p) {
    return --p;
}

static inline intptr_t AtomicAdd(TAtomic& p, intptr_t v) {
    return p += v;
}

static inline intptr_t AtomicSwap(TAtomic& p, intptr_t v) {
    intptr_t ret = p;
    p = v;
    return ret;
}

static inline bool AtomicCas(TAtomic* a, intptr_t exchange, intptr_t compare) {
    if (*a == compare) {
        *a = exchange;

        return true;
    }

    return false;
}

static inline void AtomicBarrier() {
}
