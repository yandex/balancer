#include "ptr.h"

#include <new>
#include <cstdlib>

#include <util/system/defaults.h>
#include <util/memory/alloc.h>

void TFree::DoDestroy(void* t) throw () {
    free(t);
}

void TDelete::Destroy(void* t) throw () {
    ::operator delete(t);
}
