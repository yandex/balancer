#pragma once

#include <util/system/defaults.h>

class MD5 {
public:
    MD5() {
        Init();
    }

    void  Init();
    void  Update(const void*, unsigned int);
    void  Pad();
    void  Final(unsigned char [16]);
    char* End(char*);     // buf must be char[33];
    char* End_b64(char*); // buf must be char[25];

    static char* File(const char*, char*);
    static char* Data(const unsigned char*, unsigned int, char*);

private:
    ui32 state[4];    /* state (ABCD) */
    ui32 count[2];    /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64]; /* input buffer */
};
