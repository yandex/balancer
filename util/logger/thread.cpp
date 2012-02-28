#include "thread.h"
#include "record.h"

#include <util/thread/queue.h>
#include <util/system/event.h>
#include <util/memory/addstorage.h>
#include <util/generic/ptr.h>
#include <util/generic/yexception.h>

class TThreadedLogBackend::TImpl {
        class TRec: public IObjectInQueue
                  , public TAdditionalStorage<TRec>
                  , public TLogRecord {
            public:
                inline TRec(TImpl* parent, const TLogRecord& rec)
                    : TLogRecord(rec.Priority, (const char*)AdditionalData(), rec.Len)
                    , Parent_(parent)
                {
                    memcpy(AdditionalData(), rec.Data, rec.Len);
                }

                inline ~TRec() throw () {
                }

            private:
                virtual void Process(void* /*tsr*/) {
                    THolder<TRec> This(this);

                    Parent_->Slave_->WriteData(*this);
                }

            private:
                TImpl* Parent_;
        };

        class TReopener: public IObjectInQueue
                       , public Event {
            public:
                inline TReopener(TImpl* parent)
                    : Parent_(parent)
                {
                }

                inline ~TReopener() throw () {
                }

            private:
                virtual void Process(void* /*tsr*/) {
                    try {
                        Parent_->Slave_->ReopenLog();
                    } catch (...) {
                    }

                    Signal();
                }

            private:
                TImpl* Parent_;
        };

    public:
        inline TImpl(TLogBackend* slave, size_t queuelen)
            : Slave_(slave)
        {
            Queue_.Start(1, queuelen);
        }

        inline ~TImpl() throw () {
            Queue_.Stop();
        }

        inline void WriteData(const TLogRecord& rec) {
            THolder<TRec> obj(new (rec.Len) TRec(this, rec));

            if (!Queue_.Add(obj.Get())) {
                ythrow yexception() << "log queue exhausted";
            }

            obj.Release();
        }

        inline void ReopenLog() {
            TReopener reopener(this);

            if (!Queue_.Add(&reopener)) {
                ythrow yexception() << "log queue exhausted";
            }

            reopener.Wait();
        }

    private:
        TLogBackend* Slave_;
        TMtpQueue Queue_;
};

TThreadedLogBackend::TThreadedLogBackend(TLogBackend* slave)
    : Impl_(new TImpl(slave, 0))
{
}

TThreadedLogBackend::TThreadedLogBackend(TLogBackend* slave, size_t queuelen)
    : Impl_(new TImpl(slave, queuelen))
{
}

TThreadedLogBackend::~TThreadedLogBackend() throw () {
}

void TThreadedLogBackend::WriteData(const TLogRecord& rec) {
    Impl_->WriteData(rec);
}

void TThreadedLogBackend::ReopenLog() {
    Impl_->ReopenLog();
}
