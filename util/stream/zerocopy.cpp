#include "output.h"
#include "zerocopy.h"

ui64 TransferData(IZeroCopyInput* in, TOutputStream* out) {
    ui64 ret = 0;

    size_t len;
    const void* ptr;

    while (in->Next(&ptr, &len)) {
        out->Write(ptr, len);
        ret += (ui64)len;
    }

    return ret;
}
