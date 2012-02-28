#pragma once

#include <util/system/thread.h>
#include <util/system/defaults.h>
#include <util/generic/ptr.h>

//deprecated. use future.h instead
template <class T>
class TThreadable: public T {
    public:
        typedef TThread::TThreadProc TThreadProc;

        inline TThreadable()
            : Arg(0)
            , ThreadInit(0)
        {
        }

        inline int Run(void* arg = 0, int stackSize = 0, TThreadProc init = DefInit) {
            if (!!Thread_) {
                return 1;
            }

            Arg = arg;
            ThreadInit = init;

            try {
                TThread::TParams params = {
                    Dispatch,
                    this,
                    stackSize
                };

                {
                    THolder<TThread> thread(new TThread(params));

                    thread->Start();
                    Thread_.Swap(thread);
                }
            } catch (...) {
                return 1;
            }

            return 0;
        }

        inline int Join(void** result = 0) {
            if (!Thread_) {
                return 1;
            }

            try {
                void* ret = Thread_->Join();

                if (result) {
                    *result = ret;
                }

                Thread_.Destroy();

                return 0;
            } catch (...) {
            }

            return 1;
        }

    protected:
        static TThreadable<T>* This(void* ptr) {
            return (TThreadable<T>*)ptr;
        }

        static void* Dispatch(void* ptr) {
            void* result = This(ptr)->ThreadInit(This(ptr)->Arg);

            return result ? result : (void*)This(ptr)->T::Run(This(ptr)->Arg);
        }

        static void* DefInit(void*) {
            return 0;
        }

    public:
        void* Arg;
        TThreadProc ThreadInit;

    private:
        THolder<TThread> Thread_;
};
