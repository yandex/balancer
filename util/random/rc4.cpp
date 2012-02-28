#include "rc4.h"
#include "entropy.h"

#include <util/stream/input.h>
#include <util/generic/singleton.h>

void TRc4Stream::Stir(TInputStream* s) {
    char buf[128];

    s->Load(buf, sizeof(buf));

    AddRandom((unsigned char*)buf, sizeof(buf));
}

#if !defined(_freebsd_)
class TArc4Stream: public TRc4Stream {
    public:
        inline TArc4Stream() {
            Stir();
        }

        inline void Stir() {
            TRc4Stream::Stir(EntropyPool());
        }
};

#define rs (Singleton<TArc4Stream>())

extern "C" {
    void arc4random_stir() {
        rs->Stir();
    }

    void arc4random_addrandom(unsigned char* dat, int datlen) {
        rs->AddRandom(dat, datlen);
    }

    ui32 arc4random() {
        return rs->GetWord();
    }
}
#endif
