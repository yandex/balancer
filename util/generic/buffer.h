#pragma once

#include "utility.h"
#include "noncopyable.h"

#include <cstring>

#include <util/system/yassert.h>
#include <util/system/align.h>

class Stroka;

class TBuffer {
    public:
        typedef char* TIterator;
        typedef const char* TConstIterator;

        TBuffer(size_t len = 0);
        TBuffer(const Stroka& s);

        TBuffer(const TBuffer& b)
            : Data_(0)
            , Len_(0)
            , Pos_(0)
        {
            *this = b;
        }

        TBuffer& operator= (const TBuffer& b) {
            Assign(b.Data(), b.Size());
            return *this;
        }

        ~TBuffer() throw ();

        inline void Clear() throw () {
            Pos_ = 0;
        }

        inline void EraseBack(size_t n) throw () {
            YASSERT(n <= Pos_);
            Pos_ -= n;
        }

        inline void Reset() throw () {
            TBuffer().Swap(*this);
        }

        inline void Assign(const char* data, size_t len) {
            Clear();
            Append(data, len);
        }

        inline void Assign(const char* b, const char* e) {
            Assign(b, e - b);
        }

        inline char* Data() throw () {
            return Data_;
        }

        inline const char* Data() const throw () {
            return Data_;
        }

        inline char* Pos() throw () {
            return Data_ + Pos_;
        }

        inline const char* Pos() const throw () {
            return Data_ + Pos_;
        }

        inline size_t Size() const throw () {
            return Pos_;
        }

        inline bool Empty() const throw () {
            return !Size();
        }

        inline bool operator! () const throw () {
            return Empty();
        }

        inline size_t Avail() const throw () {
            return Len_ - Pos_;
        }

        inline void Append(const char* buf, size_t len) {
            if (len > Avail()) {
                Reserve(2 * (Pos_ + len));
            }

            YASSERT(len <= Avail());

            memcpy(Data() + Pos_, buf, len);
            Pos_ += len;

            YASSERT(Pos_ <= Len_);
        }

        inline void Append(const char* b, const char* e) {
            Append(b, e - b);
        }

        inline void Append(char ch) {
            if (Len_ == Pos_) {
                Reserve(2 * (Len_ + 1));
            }

            *(Data() + Pos_++) = ch;
        }

        inline void Proceed(size_t pos) {
            Reserve(pos);
            Pos_ = pos;
        }

        inline void Advance(size_t len) {
            Proceed(Pos_ + len);
        }

        inline void Reserve(size_t len) {
            if (len > Len_) {
                DoReserve(len);
            }
        }

        inline void Resize(size_t len) {
            if (len > Pos_) {
                Reserve(len);
            }
            Pos_ = len;
        }

        inline size_t Capacity() const throw () {
            return Len_;
        }

        inline void AlignUp(size_t align, char fillChar = '\0') {
            size_t diff = ::AlignUp(Pos_, align) - Pos_;
            while (diff-- > 0)
                Append(fillChar);
        }

        /*
         * some helpers...
         */
        inline char* operator~ () throw () {
            return Data();
        }

        inline const char* operator~ () const throw () {
            return Data();
        }

        inline size_t operator+ () const throw () {
            return Size();
        }

        inline void Swap(TBuffer& r) throw () {
            DoSwap(Data_, r.Data_);
            DoSwap(Len_, r.Len_);
            DoSwap(Pos_, r.Pos_);
        }

        /*
         * after this call buffer becomes empty
         */
        void AsString(Stroka& s);

        /*
         * iterator-like interface
         */
        inline TIterator Begin() throw () {
            return Data();
        }

        inline TIterator End() throw () {
            return Begin() + Size();
        }

        inline TConstIterator Begin() const throw () {
            return Data();
        }

        inline TConstIterator End() const throw () {
            return Begin() + Size();
        }

    private:
        void DoReserve(size_t len);

    private:
        char* Data_;
        size_t Len_;
        size_t Pos_;
};
