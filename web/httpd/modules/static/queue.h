#pragma once

#include <web/httpd/kernel/chunks.h>

#include <util/system/file.h>
#include <util/system/pipe.h>
#include <util/memory/pool.h>
#include <util/generic/yexception.h>
#include <util/generic/singleton.h>
#include <util/ycoroutine.h>

namespace NQueue {
    using namespace NSrvKernel;

    class TThreadedQueue {
        public:
            class ICallBack {
                public:
                    virtual ~ICallBack() {
                    }

                    virtual void Run() throw () = 0;
                    virtual void Destroy() throw () = 0;

                private:
                    bool InGoodState_;
            };

            template <class T>
            class TCallBack: public ICallBack, public TObjectFromPool<T> {
                public:
                    virtual void Destroy() throw () {
                        delete static_cast<T*>(this);
                    }
            };

            inline TThreadedQueue(TContExecutor* e)
                : E_(e)
            {
                TPipe::Pipe(F_[0], F_[1]);
                TPipe::Pipe(T_[0], T_[1]);

                SetNonBlock(F_[1].GetHandle());
                SetNonBlock(T_[0].GetHandle());

                E_->Create<TThreadedQueue, &TThreadedQueue::Dispatcher>(this, "dispatcher");
            }

            template <class T>
            inline TAutoPtr<T> Run(TAutoPtr<T> cb, const TInstant& /*deadLine*/) {
                cb->Run();

                return cb;
            }

            inline void Dispatcher(TCont* /*c*/) {
            }

        private:
            TContExecutor* E_;
            TPipe F_[2];
            TPipe T_[2];
    };

    class TThreadedFile {
            struct THandle;

            class IFunc {
                public:
                    virtual void Run(THandle& h) const = 0;
            };

            struct THandle: public TThreadedQueue::TCallBack<THandle> {
                inline THandle(const Stroka& path)
                    : Path(path)
                    , Offset(0)
                    , Func(0)
                {
                }

                inline void Check() const {
                    if (!!Error) {
                        ythrow yexception() << Error;
                    }
                }

                virtual void Run() throw () {
                    try {
                        Func->Run(*this);
                    } catch (...) {
                        Error = CurrentExceptionMessage();
                    }
                }

                inline void Open() {
                    File = TFile(Path, OpenExisting | RdOnly);
                }

                inline void Read() {
                    const size_t readed = File.Pread(Chunk->Data(), Chunk->Length(), Offset);

                    Offset += (ui64)readed;
                    Chunk->Shrink(readed);
                }

                inline void Close() {
                    File = TFile();
                }

                Stroka Path;
                TFile File;
                ui64 Offset;
                TChunkPtr Chunk;
                Stroka Error;
                IFunc* Func;
            };

            class TOpen: public IFunc {
                public:
                    virtual void Run(THandle& h) const {
                        h.Open();
                    }

                    static inline IFunc& Instance() {
                        return *Singleton<TOpen>();
                    }
            };

            class TClose: public IFunc {
                public:
                    virtual void Run(THandle& h) const {
                        h.Close();
                    }

                    static inline IFunc& Instance() {
                        return *Singleton<TClose>();
                    }
            };

            class TRead: public IFunc {
                public:
                    virtual void Run(THandle& h) const {
                        h.Read();
                    }

                    static inline IFunc& Instance() {
                        return *Singleton<TRead>();
                    }
            };

        public:
            typedef THandle::TPool TPool;

            inline TThreadedFile(TThreadedQueue* q, TPool* pool, const Stroka& path)
                : Handle_(new (pool) THandle(path))
                , Queue_(q)
            {
                Step(TOpen::Instance());
            }

            inline TChunkPtr Pread(ui64 offset) {
                Handle_->Chunk = NewChunk();
                Handle_->Offset = offset;

                Step(TRead::Instance());

                return Handle_->Chunk;
            }

        private:
            inline void Step(IFunc& func) {
                Handle_->Func = &func;
                Handle_ = Queue_->Run(Handle_, TInstant::Max());
                Check();
            }

            inline void Check() const {
                if (!Handle_) {
                    ythrow TSystemError(ETIMEDOUT);
                }

                Handle_->Check();
            }

        private:
            TAutoPtr<THandle> Handle_;
            TThreadedQueue* Queue_;
    };
}
