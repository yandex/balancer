extern "C" {
    #include "../coreApi.h"
}

#include <util/datetime/base.h>
#include <util/system/mutex.h>
#include <util/random/entropy.h>
#include <util/stream/input.h>

extern "C" {

int osdepTimeOpen(void) {
	return PS_SUCCESS;
}

void osdepTimeClose() {
}

int32 psGetTime(psTime_t* t) {
    TInstant now = TInstant::Now();

    if (t) {
        *t = now.MilliSeconds();
    }

    return now.Seconds();
}

int32 psDiffMsecs(psTime_t then, psTime_t now) {
    return (long)(then) - (long)(now);
}

int32 psCompareTime(psTime_t a, psTime_t b) {
    return !(a > b);
}

#ifdef USE_MULTITHREADING
int32 osdepMutexOpen(void) {
	return PS_SUCCESS;
}

int32 osdepMutexClose() {
    return PS_SUCCESS;
}

int32 psCreateMutex(psMutex_t* mutex) {
    try {
        *mutex = new TMutex();

        return PS_SUCCESS;
    } catch (...) {
    }

    return PS_PLATFORM_FAIL;
}

int32 psLockMutex(psMutex_t* mutex) {
    ((TMutex*)(*mutex))->Acquire();

	return PS_SUCCESS;
}

int32 psUnlockMutex(psMutex_t* mutex) {
    ((TMutex*)(*mutex))->Release();

    return PS_SUCCESS;
}

void psDestroyMutex(psMutex_t* mutex) {
    delete ((TMutex*)(*mutex));
}
#endif

int osdepEntropyOpen(void) {
	return PS_SUCCESS;
}

void osdepEntropyClose() {
}

int32 psGetEntropy(unsigned char* bytes, uint32 size) {
    try {
        return EntropyPool()->Read(bytes, size);
    } catch (...) {
    }

    return PS_PLATFORM_FAIL;
}

int osdepTraceOpen(void) {
	return PS_SUCCESS;
}

void osdepTraceClose(void) {
}

void _psTrace(char* msg) {
	printf("%s", msg);
}

void _psTraceStr(char *message, char *value) {
	if (value) {
		printf(message, value);
	} else {
		printf("%s", message);
	}
}

void _psTraceInt(char *message, int32 value) {
	printf(message, value);
}

void _psTracePtr(char *message, void *value) {
	printf(message, value);
}

#ifdef HALT_ON_PS_ERROR
void osdepBreak(void) {
	abort();
}
#endif
}
