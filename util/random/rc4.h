#pragma once

#include <util/system/defaults.h>

class TInputStream;

class TRc4Stream {
    public:
        inline TRc4Stream() throw ()
            : Arc4i_(0)
            , Arc4j_(0)
        {
            for (size_t n = 0; n < 256; n++) {
                Arc4s_[n] = (ui8)n;
            }
        }

        inline void AddRandom(unsigned char* dat, size_t datlen) throw () {
            Arc4i_--;

            for (size_t n = 0; n < 256; n++) {
                Arc4i_ = (Arc4i_ + 1);
                const ui8 si = Arc4s_[Arc4i_];
                Arc4j_ = (Arc4j_ + si + dat[n % datlen]);
                Arc4s_[Arc4i_] = Arc4s_[Arc4j_];
                Arc4s_[Arc4j_] = si;
            }
        }

        inline ui8 GetByte() throw () {
            ui8 si, sj;

            Arc4i_ = (Arc4i_ + 1);
            si = Arc4s_[Arc4i_];
            Arc4j_ = (Arc4j_ + si);
            sj = Arc4s_[Arc4j_];
            Arc4s_[Arc4i_] = sj;
            Arc4s_[Arc4j_] = si;

            return (Arc4s_[(si + sj) & 0xff]);
        }

        inline ui32 GetWord() throw () {
            ui32 val;

            val  = GetByte() << 24;
            val |= GetByte() << 16;
            val |= GetByte() << 8;
            val |= GetByte();

            return val;
        }

        void Stir(TInputStream* entropy);

    private:
        ui8 Arc4i_;
        ui8 Arc4j_;
        ui8 Arc4s_[256];
};
