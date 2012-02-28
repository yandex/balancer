#pragma once

#include <cmath>

#include <util/generic/ymath.h>

template <class T>
class TMiddleValueTracker {
    public:
        inline TMiddleValueTracker(T initial, T coeff) throw ()
            : S_(initial)
            , Coeff_(coeff)
        {
        }

        inline void Add(T val) throw () {
            S_ = Coeff_ * S_ + (1.0 - Coeff_) * val;
        }

        inline T M() const throw () {
            return S_;
        }

        inline T C() const throw () {
            return Coeff_;
        }

    private:
        T S_;
        T Coeff_;
};

template <class T>
class TDeviationTracker: public TMiddleValueTracker<T> {
        typedef TMiddleValueTracker<T> TMVT;
    public:
        inline TDeviationTracker(T initial, T coeff)
            : TMVT(initial, coeff)
            , D_(0, coeff)
        {
        }

        inline void Add(T val) throw () {
            TMVT::Add(val);
            D_.Add(Sqr(this->M() - val));
        }

        inline T D() const throw () {
            return (T)sqrt(D_.M());
        }

        inline T GenMax() const throw () {
            return this->M() + T(3) * this->D();
        }

    private:
        TMVT D_;
};
