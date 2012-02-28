#pragma once

#include "input.h"

#include <util/memory/alloc.h>
#include <util/system/yassert.h>
#include <util/generic/ptr.h>

/// @addtogroup Streams
/// @{

/*
 * simple stream tokenizer
 */
/// Разбиение потока на части по символу-разделителю.
/// @details Простой доступ к частям потока - с помощью TIterator.
template <class TEndOfToken>
class TStreamTokenizer {
    public:/// Итератор по частям потока.
        class TIterator {
            public:
                inline TIterator(TStreamTokenizer* parent)
                    : Parent_(parent)
                    , AtEnd_(!Parent_->Next(Data_, Len_))
                {
                }

                inline TIterator() throw ()
                    : Parent_(0)
                    , Data_(0)
                    , Len_(0)
                    , AtEnd_(true)
                {
                }

                inline ~TIterator() throw () {
                }

                inline void operator++ () {
                    Next();
                }

                inline bool operator== (const TIterator& l) const throw () {
                    return AtEnd_ == l.AtEnd_;
                }

                inline bool operator!= (const TIterator& l) const throw () {
                    return !(*this == l);
                }

                inline char* Data() const throw () {
                    YASSERT(!AtEnd_);

                    return Data_;
                }

                inline size_t Length() const throw () {
                    YASSERT(!AtEnd_);

                    return Len_;
                }

                inline TIterator* operator-> () throw () {
                    return this;
                }

                inline TIterator& operator* () throw () {
                    return *this;
                }

            private:
                inline void Next() {
                    YASSERT(Parent_);

                    AtEnd_ = !Parent_->Next(Data_, Len_);
                }

            private:
                TStreamTokenizer* Parent_;
                char* Data_;
                size_t Len_;
                bool AtEnd_;
        };

        inline TStreamTokenizer(TInputStream* input, const TEndOfToken& eot = TEndOfToken(), size_t initial = 1024)
            : Input_(input)
            , Len_(initial)
            , Buf_((char*)malloc(Len_))
            , Cur_(BufBegin())
            , End_(BufBegin())
            , Eot_(eot)
        {
            CheckBuf();
        }

        inline bool Next(char*& buf, size_t& len) {
            char* it = Cur_;

            while (true) {
                do {
                    while (it != End_) {
                        if (Eot_(*it)) {
                            *it = 0;

                            buf = Cur_;
                            len = it - Cur_;
                            Cur_ = it + 1;

                            return true;
                        } else {
                            ++it;
                        }
                    }

                    if (Fill() == 0 && End_ != BufEnd()) {
                        *it = 0;

                        buf = Cur_;
                        len = it - Cur_;
                        Cur_ = End_;

                        return len;
                    }
                } while (it != BufEnd());

                YASSERT(it == BufEnd());
                YASSERT(End_ == BufEnd());

                const size_t blen = End_ - Cur_;

                if (Cur_ == BufBegin()) {
                    YASSERT(blen == Len_);

                    /*
                     * do reallocate
                     */

                    Len_ *= 4;
                    Buf_.Reset((char*)realloc(Buf_.Release(), Len_));

                    CheckBuf();
                } else {
                    /*
                     * do move
                     */

                    memmove(BufBegin(), Cur_, blen);
                }

                Cur_ = BufBegin();
                End_ = Cur_ + blen;
                it = End_;
            }
        }

        inline TIterator Begin() {
            return TIterator(this);
        }

        inline TIterator End() throw () {
            return TIterator();
        }

    private:
        inline size_t Fill() {
            const size_t avail = BufEnd() - End_;
            const size_t readed = Input_->Read(End_, avail);

            End_ += readed;

            return readed;
        }

        inline char* BufBegin() throw () {
            return Buf_.Get();
        }

        inline char* BufEnd() throw () {
            return BufBegin() + Len_;
        }

        inline void CheckBuf() const {
            if (!Buf_.Get()) {
                ThrowBadAlloc();
            }
        }

    private:
        TInputStream* Input_;
        size_t Len_;
        THolder<char, TFree> Buf_;
        char* Cur_;
        char* End_;
        TEndOfToken Eot_;
};

/*
 * eol delimiter for TStreamTokenizer
 */
/// Символ '\\n' для TStreamTokenizer
struct TEol {
    inline bool operator() (char ch) const throw () {
        return ch == '\n';
    }
};

/// @}
