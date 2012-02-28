#pragma once

#include "alloc.h"

#include <util/memory/blob.h>
#include <util/system/yassert.h>
#include <util/memory/smallobj.h>
#include <util/generic/ptr.h>
#include <util/generic/strbuf.h>

namespace NSrvKernel {
    class TChunkData: public TBigObj<TChunkData>, public TBlob::TBase, public TRefCounted<TChunkData> {
        typedef TRefCounted<TChunkData> TRefBase;
    public:
        inline char* Data() const throw () {
            return (char*)AdditionalData();
        }

        inline size_t Length() const throw () {
            return AdditionalLength();
        }

    private:
        virtual void Ref() throw () {
            TRefBase::Ref();
        }

        virtual void UnRef() throw () {
            TRefBase::UnRef();
        }
    };

    template <class T>
    static inline TBlob MakeBlobFromRefCountedBase(T* t) {
        return TBlob(t->Data(), t->Length(), t);
    }

    static inline TBlob MakeBlobFromStringBuf(const TStringBuf& sb) {
        return TBlob::NoCopy(~sb, +sb);
    }

    class TChunk;
    typedef TAutoPtr<TChunk> TChunkPtr;

    class TChunk: public TSmallObj, public TIntrusiveListItem<TChunk> {
    public:
        inline TChunk(const TBlob& blob) throw ()
            : Blob_(blob)
        {
        }

        inline char* Data() const throw () {
            return (char*)Blob_.Data();
        }

        inline size_t Length() const throw () {
            return Blob_.Length();
        }

        inline void Shrink(size_t len) throw () {
            Blob_ = Blob_.SubBlob(len);
        }

        inline TChunkPtr SubChunk(size_t b, size_t e) const {
            return new TChunk(Blob_.SubBlob(b, e));
        }

        inline TChunkPtr SubChunk(const char* b, const char* e) const {
            YASSERT(b <= e);

            return SubChunk(Offset(b), Offset(e));
        }

        inline size_t Offset(const char* p) const {
            const size_t ret = p - Data();

            YASSERT(ret <= Length());

            return ret;
        }

        inline TChunkPtr SubChunk(size_t len) const {
            return SubChunk(0, len);
        }

        inline TChunkPtr Copy() const {
            return new TChunk(Blob_);
        }

        template <class T>
        inline void AddPartTo(const char* b, const char* e, T& lst) const {
            if (b < e) {
                lst.Push(SubChunk(b, e));
            }
        }

    private:
        TBlob Blob_;
    };

    inline TChunkPtr NewChunk(const TBlob& blob) {
        return new TChunk(blob);
    }

    inline TChunkPtr NewChunk(TChunkData* data) {
        return NewChunk(MakeBlobFromRefCountedBase(data));
    }

    inline TChunkPtr NewChunk(const TStringBuf& sb) {
        return NewChunk(MakeBlobFromStringBuf(sb));
    }

    inline TChunkPtr NewChunk(size_t len = 7000) {
        return NewChunk(new (len) TChunkData());
    }

    class TChunkList: public TIntrusiveListWithAutoDelete<TChunk, TDelete> {
    public:
        inline TChunkList() throw () {
        }

        template <class T>
        explicit inline TChunkList(const T& t) {
            Push(t);
        }

        inline size_t FullLength() const throw () {
            size_t ret = 0;

            for (TConstIterator it = Begin(); it != End(); ++it) {
                ret += it->Length();
            }

            return ret;
        }

        inline void Push(const TStringBuf& str) {
            Push(NewChunk(str));
        }

        inline void Push(const TChunkPtr& chunk) throw () {
            PushBack(chunk.Release());
        }

        inline size_t CopyData(void* buf, size_t len) const throw () {
            char* cur = (char*)buf;

            for (TConstIterator it = Begin(); len && it != End(); ++it) {
                const size_t read = TStringBuf(it->Data(), it->Length()).copy(cur, len);

                len -= read;
                cur += read;
            }

            return cur - (char*)buf;
        }

        friend inline bool operator== (const TChunkList& l, const TStringBuf& r) throw () {
            if (l.FullLength() == +r) {
                const char* b = ~r;

                for (TChunkList::TConstIterator it = l.Begin(); it != l.End(); ++it) {
                    const TChunk& c = *it;
                    const size_t len = c.Length();

                    if (TStringBuf(c.Data(), len) != TStringBuf(b, len)) {
                        return false;
                    }

                    b += len;
                }

                return true;
            }

            return false;
        }

        friend inline bool operator!= (const TChunkList& l, const TStringBuf& r) throw () {
            return !(l == r);
        }

        friend inline bool operator== (const TStringBuf& r, const TChunkList& l) throw () {
            return l == r;
        }

        friend inline bool operator!= (const TStringBuf& r, const TChunkList& l) throw () {
            return !(l == r);
        }
    };

    template <class T>
    static inline void MoveTo(T* f, T* t) throw () {
        t->Append(*f);
    }

    template <class T>
    static inline void CopyToImpl(const T* f, T* t) {
        for (typename T::TConstIterator it = f->Begin(); it != f->End(); ++it) {
            t->PushBack(it->Copy().Release());
        }
    }

    template <class T>
    static inline void CopyTo(const T* f, T* t) {
        T tmp;

        CopyToImpl(f, &tmp);
        MoveTo(&tmp, t);
    }

}
