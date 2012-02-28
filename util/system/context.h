#pragma once

#include "align.h"
#include "defaults.h"

#include <util/generic/utility.h>
#include <util/generic/yexception.h>

#define STACK_ALIGN (8 * PLATFORM_DATA_ALIGN)

#if defined(_x86_64_) || defined(_i386_)
    #define STACK_GROW_DOWN
#else
    #error todo
#endif

/*
 * switch method
 */
#if defined(_win_)
    #define USE_FIBER_CONT
#elif defined(_unix_)
    #if (defined(_i386_) || defined(_x86_64_))
        #define USE_INTERNAL_ASM_JUMP_CONT
    #else
        #define USE_UCONTEXT_CONT
    #endif
#else
    #error todo
#endif

#if defined(USE_INTERNAL_ASM_JUMP_CONT)
    #undef  USE_UCONTEXT_CONT
    #define USE_JUMP_CONT
#endif

#if defined(USE_UCONTEXT_CONT)
    #include <ucontext.h>
#endif

#if defined(USE_JUMP_CONT)
    #if defined(USE_INTERNAL_ASM_JUMP_CONT)
        #include "context_x86.h"
    #else
        #include <setjmp.h>
    #endif
#endif

struct TMemRange {
    inline TMemRange() throw ()
        : Ptr(0)
        , Len(0)
    {
    }

    inline TMemRange(char* ptr, size_t len) throw ()
        : Ptr(ptr)
        , Len(len)
    {
    }

    inline char* End() throw () {
        return Ptr + Len;
    }

    char* Ptr;
    size_t Len;
};

struct ITrampoLine {
    virtual ~ITrampoLine() {
    }

    virtual void DoRun() = 0;
};

struct TContClosure {
    ITrampoLine* TrampoLine;
    TMemRange Stack;
};

#if defined(USE_UCONTEXT_CONT)
class TContMachineContext {
        typedef void (*ucontext_func_t)(void);
    public:
        inline TContMachineContext() {
            getcontext(&Ctx_);
        }

        inline TContMachineContext(const TContClosure& c) {
            Zero(Ctx_);

            Ctx_.uc_link = 0;
            Ctx_.uc_stack.ss_sp = c.Stack.Ptr;
            Ctx_.uc_stack.ss_size = c.Stack.Len;
            Ctx_.uc_stack.ss_flags = 0;

            extern void ContextTrampoLine(void* arg);
            makecontext(&Ctx_, (ucontext_func_t)ContextTrampoLine, 1, c.TrampoLine);
        }

        inline ~TContMachineContext() throw () {
        }

        inline void SwitchTo(TContMachineContext* next) throw () {
            swapcontext(&Ctx_, &next->Ctx_);
        }

    private:
        ucontext_t Ctx_;
};
#endif

#if defined(USE_FIBER_CONT)
class TContMachineContext {
    public:
        TContMachineContext();
        TContMachineContext(const TContClosure& c);
        ~TContMachineContext() throw ();

        void SwitchTo(TContMachineContext* next) throw ();

    private:
        void* Fiber_;
        bool MainFiber_;
};
#endif

#if defined(USE_JUMP_CONT)
/*
 * some nasty defs
 */
#if defined(USE_INTERNAL_ASM_JUMP_CONT)
    #define cont_setjmp __mysetjmp
    #define cont_longjmp __mylongjmp
    #define cont_jmp_buf __myjmp_buf

    #define PROGR_CNT MJB_PC
    #define STACK_CNT MJB_RSP
#else
    #if defined(_darwin_)
        #if defined(_i386_)
            #define PROGR_CNT 12
            #define STACK_CNT 9

            #define cont_setjmp _setjmp
            #define cont_longjmp _longjmp
        #endif
    #endif

    #if defined(_freebsd_)
        #define PROGR_CNT 0
        #define STACK_CNT 2

        #define cont_setjmp _setjmp
        #define cont_longjmp _longjmp
    #endif

    #if defined(_linux_)
        #if defined(_i386_)
            #define PROGR_CNT 5
            #define STACK_CNT 4
        #endif

        #if defined(_x86_64_)
            #define PROGR_CNT JB_PC
            #define STACK_CNT JB_RSP
        #endif
    #endif

    #if defined(_win_)
        #if defined(_i386_)
            #define PROGR_CNT 5
            #define STACK_CNT 4
        #endif
    #endif
#endif

#if !defined(cont_setjmp)
    #define cont_setjmp setjmp
#endif

#if !defined(cont_longjmp)
    #define cont_longjmp longjmp
#endif

#if !defined(cont_jmp_buf)
    #define cont_jmp_buf jmp_buf
#endif

static inline void*& JmpBufReg(cont_jmp_buf& buf, size_t n) throw () {
#define GETREG(X) (((void**)(void*)(X))[n])

#if defined(USE_INTERNAL_ASM_JUMP_CONT)
    return GETREG(buf);
#elif defined(_darwin_)
    return GETREG(buf);
#elif defined(_freebsd_)
    return GETREG(buf[0]._jb);
#elif defined(_linux_)
    return GETREG(buf[0].__jmpbuf);
#else
    #error todo
#endif

#undef GETREG
}

static inline void*& JmpBufStackReg(cont_jmp_buf& buf) throw () {
    return JmpBufReg(buf, STACK_CNT);
}

static inline void*& JmpBufProgrReg(cont_jmp_buf& buf) throw () {
    return JmpBufReg(buf, PROGR_CNT);
}

class TContMachineContext {
        class TStack {
            public:
                inline TStack(const TMemRange& range) throw ()
#if defined(STACK_GROW_DOWN)
                    : Data_(range.Ptr + range.Len)
#else
                    : Data_(range.Ptr + STACK_ALIGN)
#endif
                {
                    ReAlign();
                }

                inline ~TStack() throw () {
                }

                inline void ReAlign() throw () {
                    Data_ = AlignStackPtr(Data_);
                }

                template <class T>
                inline void Push(T t) throw () {
#if defined(STACK_GROW_DOWN)
                    Data_ -= sizeof(T);
                    *((T*)Data_) = t;
#else
                    *((T*)Data_) = t;
                    Data_ += sizeof(T);
#endif
                }

                inline char* StackPtr() throw () {
                    return Data_;
                }

            private:
                static inline char* AlignStackPtr(char* ptr) throw () {
#if defined(STACK_GROW_DOWN)
                    return AlignDown(ptr, STACK_ALIGN);
#else
                    return AlignUp(ptr, STACK_ALIGN);
#endif
                }

            private:
                char* Data_;
        };

    public:
        inline TContMachineContext() {
            Zero(Buf_);
        }

        inline TContMachineContext(const TContClosure& c) {
            TStack stack(c.Stack);

            /*
             * arg, and align data
             */
            stack.Push(c.TrampoLine);
            stack.Push(c.TrampoLine);
            stack.ReAlign();

            /*
             * fake return address
             */
            stack.Push((void*)0);

            cont_setjmp(Buf_);

            extern void ContextTrampoLine();
            JmpBufProgrReg(Buf_) = (void*)ContextTrampoLine;
            JmpBufStackReg(Buf_) = stack.StackPtr();
        }

        inline ~TContMachineContext() throw () {
        }

        void SwitchTo(TContMachineContext* next) throw ();

    private:
        cont_jmp_buf Buf_;
};

#endif

static inline size_t MachineContextSize() throw () {
    return sizeof(TContMachineContext);
}

#if !defined(FROM_CONTEXT_IMPL)
    /*
     * be polite
     */
    #undef USE_JUMP_CONT
    #undef USE_FIBER_CONT
    #undef USE_UCONTEXT_CONT
    #undef cont_setjmp
    #undef cont_longjmp
    #undef cont_jmp_buf
    #undef PROGR_CNT
    #undef STACK_CNT
#endif
