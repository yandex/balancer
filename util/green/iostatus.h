#pragma once

class TIOStatus {
public:
    inline TIOStatus(int status) throw ()
        : Status_(status)
    {
    }

    static inline TIOStatus Error(int status) throw () {
        return TIOStatus(status);
    }

    static inline TIOStatus Error() throw () {
        return TIOStatus(LastSystemError());
    }

    static inline TIOStatus Success() throw () {
        return TIOStatus(0);
    }

    inline void Check() const {
        if (Status_) {
            ythrow TSystemError(Status_) << "io error";
        }
    }

    inline bool Failed() const throw () {
        return (bool)Status_;
    }

    inline bool Succeed() const throw () {
        return !Failed();
    }

    inline int Status() const throw () {
        return Status_;
    }

private:
    int Status_;
};

class TContIOStatus {
public:
    inline TContIOStatus(size_t processed, TIOStatus status) throw ()
        : Processed_(processed)
        , Status_(status)
    {
    }

    static inline TContIOStatus Error(TIOStatus status) throw () {
        return TContIOStatus(0, status);
    }

    static inline TContIOStatus Error() throw () {
        return TContIOStatus(0, TIOStatus::Error());
    }

    static inline TContIOStatus Success(size_t processed) throw () {
        return TContIOStatus(processed, TIOStatus::Success());
    }

    static inline TContIOStatus Eof() throw () {
        return Success(0);
    }

    inline ~TContIOStatus() throw () {
    }

    inline size_t Processed() const throw () {
        return Processed_;
    }

    inline int Status() const throw () {
        return Status_.Status();
    }

    inline size_t Checked() const {
        Status_.Check();

        return Processed_;
    }

private:
    size_t Processed_;
    TIOStatus Status_;
};
