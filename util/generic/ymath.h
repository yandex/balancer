#pragma once

#include <util/system/defaults.h>
#include <cmath>
#include <cfloat>

static const double PI = 3.14159265358979323846;

/** Returns base 2 logarithm */
double Log2(double);
float Log2f(float);

template <class T>
static inline T Sqr(T t) throw () {
    return t * t;
}

static inline bool IsFinite(double f) {
#if defined(isfinite)
    return isfinite(f);
#elif defined(_win_)
    return _finite(f) != 0;
#elif defined(_darwin_)
    return finite(f);
#else
    return finitef(f);
#endif
}

static inline bool IsNan(double f) {
#if defined(_win_)
    return _isnan(f) != 0;
#else
    return isnan(f);
#endif
}

inline bool IsValidFloat(double f) {
    return IsFinite(f) && !IsNan(f);
}

#ifdef _win_
double Erf(double x);
#else
inline double Erf(double x) {
    return erf(x);
}
#endif
