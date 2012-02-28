#pragma once

#include "deviation.h"

#include <util/generic/algorithm.h>
#include <util/generic/pair.h>
#include <util/datetime/base.h>

namespace NSrvKernel {
    template <size_t N>
    class TTimeLimitedAvgTracker {
        public:
            inline TTimeLimitedAvgTracker(TDuration ttl)
                : Cur_(0)
                , Total_(0)
                , Ttl_(ttl)
            {
            }

            inline TPair<double, size_t> Get() const {
                Check();
                return TPair<double, size_t>(Sum_, Total_);
            }

            inline void Add(double v) {
                TInstant now = Now();
                if (Total_ == N) {
                    Sum_ = Sum_ + v - V_[Cur_];
                    V_[Cur_] = v;
                    T_[Cur_] = now;
                    Cur_ = (Cur_ + 1) % N;
                } else {
                    size_t l = (Cur_ + Total_) % N;
                    V_[l] = v;
                    T_[l] = now;
                    Sum_ += v;
                    Total_ += 1;
                }
            }

        private:
            inline void Check() const {
                TInstant now = Now();
                while (Total_ != 0) {
                    if (now - T_[Cur_] > Ttl_) {
                        Sum_ -= V_[Cur_];
                        Total_ -= 1;
                        Cur_ = (Cur_ + 1) % N;
                    } else {
                        break;
                    }
                }
            }

        private:
            double V_[N];
            TInstant T_[N];
            mutable size_t Cur_;
            mutable size_t Total_;
            mutable double Sum_;

            TDuration Ttl_;
    };

    template <size_t N>
    class TAvgTracker {
        public:
            inline TAvgTracker(double v)
                : C_(0)
                , Sum_(v * N)
            {
                for (size_t i = 0; i < N; ++i) {
                    V_[i] = v;
                }
            }

            inline double Get() const {
                return Sum_ / N;
            }

            inline void Add(double v) {
                Sum_ = Sum_ - V_[C_] + v;
                V_[C_] = v;
                C_ = (C_ + 1) % N;
            }

        private:
            double V_[N];
            double Sum_;
            size_t C_;
    };

    template <size_t N>
    class TMedianTracker {
        public:
            inline TMedianTracker()
                : C_(0)
                , N_(0)
            {
            }

            inline double Get(size_t n) const throw () {
                const size_t filled = Min(N_, N);

                if (!filled) {
                    return 0;
                }

                memcpy(VC_, V_, sizeof(*VC_) * filled);
                QuickSort(VC_, VC_ + filled);

                const size_t pos = n * filled / 100;
                const size_t f = (size_t)Max(((int)pos - 2), 0);
                const size_t l = Min(pos + 3, filled);

                double ret = 0;

                for (size_t i = f; i < l; ++i) {
                    ret += VC_[i];
                }

                return (size_t)(ret / (l - f));
            }

            inline void Add(double v) throw () {
                V_[C_] = v;
                C_ = (C_ + 1) % N;
                ++N_;
            }

        private:
            mutable double VC_[N];
            double V_[N];
            size_t C_;
            size_t N_;
    };
}
