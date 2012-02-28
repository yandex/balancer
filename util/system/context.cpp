#include "defaults.h"

#include <cstdlib> //for abort()

#if defined(_win_)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

#include <util/generic/yexception.h>

#define FROM_CONTEXT_IMPL
#include "context.h"

static inline void Run(void* arg) {
    try {
        ((ITrampoLine*)arg)->DoRun();
    } catch (...) {
    }

    abort();
}

#if defined(USE_JUMP_CONT)
void ContextTrampoLine() {
    void** argPtr = (void**)((char*)AlignUp(&argPtr + 1, STACK_ALIGN) + STACK_ALIGN);
    YASSERT(*(argPtr - 1) == *(argPtr - 2));

    Run(*(argPtr - 1));
}
#else
void ContextTrampoLine(void* arg) {
    Run(arg);
}
#endif

#if defined(USE_FIBER_CONT)
TContMachineContext::TContMachineContext()
    : Fiber_(ConvertThreadToFiber(this))
    , MainFiber_(true)
{
    if (!Fiber_) {
        ythrow yexception() << "fiber error";
    }
}

TContMachineContext::TContMachineContext(const TContClosure& c)
    : Fiber_(CreateFiber(c.Stack.Len, (LPFIBER_START_ROUTINE)ContextTrampoLine, (LPVOID)c.TrampoLine))
    , MainFiber_(false)
{
    if (!Fiber_) {
        ythrow yexception() << "fiber error";
    }
}

TContMachineContext::~TContMachineContext() throw () {
    if (MainFiber_) {
        ConvertFiberToThread();
    } else {
        DeleteFiber(Fiber_);
    }
}

void TContMachineContext::SwitchTo(TContMachineContext* next) throw () {
    SwitchToFiber(next->Fiber_);
}
#endif

#if defined(USE_JUMP_CONT)
void TContMachineContext::SwitchTo(TContMachineContext* next) throw () {
    if (cont_setjmp(Buf_) == 0) {
        cont_longjmp(next->Buf_, 1);
    }
}
#endif
