#include "ymath.h"
#include "singleton.h"

#include <cmath>

template <class T>
struct TLog2Holder {
    inline TLog2Holder()
        : Log2Inv((T)1.0 / log((T)2))
    {
    }

    T Log2Inv;
};

double Log2(double x) {
    return log(x) * Singleton<TLog2Holder<double> >()->Log2Inv;
}

float Log2f(float x) {
    return logf(x) * Singleton<TLog2Holder<float> >()->Log2Inv;
}

#ifdef _win_

double Erf(double x) {
    static const double _M_2_SQRTPI = 1.12837916709551257390;
    static const double eps = 1.0e-7;
    if (fabs(x) >= 3.75)
        return x > 0 ? 1.0 : -1.0;
    double r = _M_2_SQRTPI * x;
    double f = r;
    for (int i = 1;; ++i) {
        r *= -x * x / i;
        f += r / (2 * i + 1);
        if (fabs(r) < eps * (2 * i + 1))
            break;
    }
    return f;
}

#endif // _win_
