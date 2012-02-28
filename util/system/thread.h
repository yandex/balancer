#pragma once

/// This code should not be used directly unless you really understand what you do.
/// If you need threads, use thread pool functionality in <util/thread/pool.h>
/// @see SystemThreadPool()

#include <util/generic/ptr.h>
#include "defaults.h"

class TThread {
    public:
        typedef void* (*TThreadProc)(void*);
        typedef size_t TId;

        struct TParams {
            TThreadProc Proc;
            void* Data;
            size_t StackSize;
        };

        TThread(const TParams& params);
        TThread(TThreadProc threadProc, void* param);

        ~TThread() throw ();

        void Start();

        void* Join();
        void Detach();
        bool Running() const throw ();
        TId Id() const throw ();

        static TId ImpossibleThreadId() throw ();
        static TId CurrentThreadId() throw ();

    private:
        class TImpl;
        THolder<TImpl> Impl_;
};
