#pragma once

class TContEvent {
public:
    inline TContEvent(TCont* current) throw ()
        : Cont_(current)
        , Status_(0)
    {
    }

    inline ~TContEvent() throw () {
    }

    inline int WaitD(TInstant deadline) {
        Status_ = 0;
        const int ret = Cont_->SleepD(deadline);

        return Status_ ? Status_ : ret;
    }

    inline int WaitT(TDuration timeout) {
        return WaitD(timeout.ToDeadLine());
    }

    inline int WaitI() {
        return WaitD(TInstant::Max());
    }

    inline void Wake() throw () {
        SetStatus(EWAKEDUP);
        Cont_->Executor()->ScheduleExecution(Cont_->Rep());
    }

    inline TCont* Cont() throw () {
        return Cont_;
    }

    inline int Status() const throw () {
        return Status_;
    }

    inline void SetStatus(int status) throw () {
        Status_ = status;
    }

private:
    TCont* Cont_;
    int Status_;
};

class TContWaitQueue {
    class TWaiter: public TContEvent, public TIntrusiveListItem<TWaiter> {
    public:
        inline TWaiter(TCont* current) throw ()
            : TContEvent(current)
        {
        }

        inline ~TWaiter() throw () {
        }
    };

public:
    inline TContWaitQueue() throw () {
    }

    inline ~TContWaitQueue() throw () {
        YASSERT(Waiters_.Empty());
    }

    inline int WaitD(TCont* current, TInstant deadline) {
        TWaiter waiter(current);

        Waiters_.PushBack(&waiter);

        return waiter.WaitD(deadline);
    }

    inline int WaitT(TCont* current, TDuration timeout) {
        return WaitD(current, timeout.ToDeadLine());
    }

    inline int WaitI(TCont* current) {
        return WaitD(current, TInstant::Max());
    }

    inline void Signal() throw () {
        if (!Waiters_.Empty()) {
            Waiters_.PopFront()->Wake();
        }
    }

    inline void BroadCast() throw () {
        while (!Waiters_.Empty()) {
            Waiters_.PopFront()->Wake();
        }
    }

private:
    TIntrusiveList<TWaiter> Waiters_;
};

class TContSimpleEvent {
public:
    inline TContSimpleEvent(TContExecutor* e)
        : E_(e)
    {
    }

    inline void Signal() throw () {
        Q_.Signal();
    }

    inline void BroadCast() throw () {
        Q_.BroadCast();
    }

    inline int WaitD(TInstant deadLine) throw () {
        return Q_.WaitD(E_->Running()->ContPtr(), deadLine);
    }

    inline int WaitT(TDuration timeout) throw () {
        return WaitD(timeout.ToDeadLine());
    }

    inline int WaitI() throw () {
        return WaitD(TInstant::Max());
    }

private:
    TContWaitQueue Q_;
    TContExecutor* E_;
};
