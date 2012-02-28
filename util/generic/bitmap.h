#pragma once

#include "bitops.h"
#include "ptr.h"

#include <util/system/yassert.h>
#include <util/system/defaults.h>

#include <util/ysaveload.h>

namespace NBitMapPrivate {

template <typename TChunkType, size_t ExtraBits>
struct TSanitizeMask {
    static const TChunkType Value = ~((~TChunkType(0)) << ExtraBits);
};

template <typename TChunkType>
struct TSanitizeMask<TChunkType, 0> {
    static const TChunkType Value = ~TChunkType(0);
};

template <size_t BitCount, typename TChunkType>
struct TFixedStorage {
    typedef TChunkType TChunk;

    static const size_t Size = (BitCount + 8 * sizeof(TChunk) - 1) / (8 * sizeof(TChunk));

    TChunk Data[Size];

    TFixedStorage() {
        for (size_t i = 0; i < Size; ++i) {
            Data[i] = 0;
        }
    }

    TFixedStorage(const TFixedStorage<BitCount, TChunkType>& st) {
        for (size_t i = 0; i < Size; ++i) {
            Data[i] = st.Data[i];
        }
    }

    template <typename TOtherChunk>
    TFixedStorage(const TOtherChunk* data, size_t size) {
        VERIFY(Size * sizeof(TChunk) >= size * sizeof(TOtherChunk), "Exceeding bitmap storage capacity");
        if (Size * sizeof(TChunk) > size * sizeof(TOtherChunk)) {
            for (size_t i = (size * sizeof(TOtherChunk)) / sizeof(TChunk); i < Size; ++i) {
                Data[i] = 0;
            }
        }
        if (sizeof(TChunk) > sizeof(TOtherChunk)) {
            for (size_t i = 0; i < size; ++i) {
                static_cast<TOtherChunk*>(Data)[i] = data[i];
            }
        } else {
            for (size_t i = 0; i < (size * sizeof(TOtherChunk)) / sizeof(TChunk); ++i) {
                Data[i] = static_cast<const TChunk*>(data)[i];
            }
        }
    }

    FORCED_INLINE void Swap(TFixedStorage<BitCount, TChunkType>& st) {
        for (size_t i = 0; i < Size; ++i) {
            DoSwap(Data[i], st.Data[i]);
        }
    }

    FORCED_INLINE static size_t GetBitCapacity() {
        return BitCount;
    }

    FORCED_INLINE static size_t GetChunkCapacity() {
        return Size;
    }

    // Returns true if the resulting storage capacity is enough to fit the requested size
    FORCED_INLINE static bool ExpandBitSize(size_t bitSize) {
        return bitSize <= BitCount;
    }

    FORCED_INLINE void Sanitize() {
        Data[Size - 1] &= TSanitizeMask<TChunk, BitCount % (8 * sizeof(TChunk))>::Value;
    }
};

// Dynamically expanded storage.
// It uses "on stack" realization with no allocation for one chunk spaces
template <typename TChunkType>
struct TDynamicStorage {
    typedef TChunkType TChunk;

    size_t Size;
    TChunk StackData;
    TArrayHolder<TChunk> ArrayData;
    TChunk* Data;

    TDynamicStorage()
        : Size(1)
        , StackData(0)
        , Data(&StackData)
    {
    }

    TDynamicStorage(const TDynamicStorage<TChunk>& st)
        : Size(1)
        , StackData(0)
        , Data(&StackData)
    {
        ExpandSize(st.Size, false);
        ::memcpy(Data, st.Data, Size * sizeof(TChunk));
    }

    template <typename TOtherChunk>
    TDynamicStorage(const TOtherChunk* data, size_t size)
        : Size(1)
        , StackData(0)
        , Data(&StackData)
    {
        ExpandBitSize(size * sizeof(TOtherChunk), false);
        ::memset(Data, 0, Size * sizeof(TChunk));
        ::memcpy(Data, data, Min(Size * sizeof(TChunk), size * sizeof(TOtherChunk)));
    }

    FORCED_INLINE void Swap(TDynamicStorage<TChunkType>& st) {
        DoSwap(Size, st.Size);
        DoSwap(StackData, st.StackData);
        DoSwap(ArrayData, st.ArrayData);
        Data = 1 == Size ? &StackData : ArrayData.Get();
        st.Data = 1 == st.Size ? &st.StackData : st.ArrayData.Get();
    }

    FORCED_INLINE size_t GetBitCapacity() const {
        return Size * 8 * sizeof(TChunk);
    }

    FORCED_INLINE size_t GetChunkCapacity() const {
        return Size;
    }

    // Returns true if the resulting storage capacity is enough to fit the requested size
    FORCED_INLINE bool ExpandSize(size_t size, bool keepData = true) {
        if (size > Size) {
            TArrayHolder<TChunk> newData(new TChunk[size]);
            if (keepData) {
                ::memcpy(newData.Get(), Data, Size * sizeof(TChunk));
                ::memset(newData.Get() + Size, 0, (size - Size) * sizeof(TChunk));
            } else {
                ::memset(newData.Get(), 0, Size * sizeof(TChunk));
            }
            DoSwap(ArrayData, newData);
            Data = ArrayData.Get();
            Size = size;
        }
        return true;
    }

    FORCED_INLINE bool ExpandBitSize(size_t bitSize, bool keepData = true) {
        return ExpandSize((bitSize + 8 * sizeof(TChunk) - 1) / (8 * sizeof(TChunk)), keepData);
    }

    FORCED_INLINE void Sanitize() {
    }
};

template <size_t num>
struct TDivCount {
    const static size_t Value = 1 + TDivCount<(num >> 1)>::Value;
};

template <>
struct TDivCount<0> {
    const static size_t Value = 0;
};

} // NBitMapPrivate

template <size_t BitCount, typename TChunkType>
struct TFixedBitMapTraits {
    typedef TChunkType TChunk;
    typedef NBitMapPrivate::TFixedStorage<BitCount, TChunkType> TStorage;
};

template <typename TChunkType>
struct TDynamicBitMapTraits {
    typedef TChunkType TChunk;
    typedef NBitMapPrivate::TDynamicStorage<TChunkType> TStorage;
};

template <class TTraits>
class TBitMapOps {
public:
    typedef typename TTraits::TChunk TChunk;
    typedef TBitMapOps<TTraits> TThis;

private:
    static const size_t BitsPerChunk = 8 * sizeof(TChunk);
    static const TChunk ModMask = static_cast<TChunk>(BitsPerChunk - 1);
    static const size_t DivCount = NBitMapPrivate::TDivCount<BitsPerChunk>::Value - 1;

    template <class> friend class TBitMapOps;

    typedef typename TTraits::TStorage TStorage;

    TStorage Mask;

public:
    class TReference {
    private:
        friend class TBitMapOps<TTraits>;

        TChunk* Chunk;
        size_t Offset;

        TReference(TChunk* c, size_t offset)
            : Chunk(c)
            , Offset(offset)
        {
        }

    public:
        ~TReference() {}

        FORCED_INLINE TReference& operator =(bool val) {
            if (val)
                *Chunk |= static_cast<TChunk>(1) << Offset;
            else
                *Chunk &= ~static_cast<TChunk>(1) << Offset;

            return *this;
        }

        FORCED_INLINE TReference& operator =(const TReference& ref) {
            if (ref)
                *Chunk |= static_cast<TChunk>(1) << Offset;
            else
                *Chunk &= ~(static_cast<TChunk>(1) << Offset);

            return *this;
        }

        FORCED_INLINE bool operator ~() const {
            return 0 == (*Chunk & (static_cast<TChunk>(1) << Offset));
        }

        FORCED_INLINE operator bool() const {
            return 0 != (*Chunk & (static_cast<TChunk>(1) << Offset));
        }

        FORCED_INLINE TReference& Flip() {
            *Chunk ^= static_cast<TChunk>(1) << Offset;
            return *this;
        }
    };

public:
    TBitMapOps() {
    }

    TBitMapOps(TChunk val)
    {
        Mask.Data[0] = val;
        Mask.Sanitize();
    }

    TBitMapOps(const TThis& bitmap)
        : Mask(bitmap.Mask)
    {
    }

    template <class T>
    TBitMapOps(const TBitMapOps<T>& bitmap)
        : Mask(bitmap.Mask.Data, bitmap.Mask.GetChunkCapacity())
    {
        Mask.Sanitize();
    }

    template <class T>
    FORCED_INLINE bool operator ==(const TBitMapOps<T>& bitmap) const {
        return Equal(bitmap);
    }

    FORCED_INLINE TThis& operator =(const TThis& bitmap) {
        if (this != &bitmap) {
            TThis bm(bitmap);
            Swap(bm);
        }
        return *this;
    }

    template <class T>
    FORCED_INLINE TThis& operator =(const TBitMapOps<T>& bitmap) {
        TThis bm(bitmap);
        Swap(bm);
        return *this;
    }

    template <class T>
    FORCED_INLINE TThis& operator &=(const TBitMapOps<T>& bitmap) {
        return And(bitmap);
    }

    FORCED_INLINE TThis& operator &=(const TChunk& val) {
        return And(val);
    }

    template <class T>
    FORCED_INLINE TThis& operator |=(const TBitMapOps<T>& bitmap) {
        return Or(bitmap);
    }

    FORCED_INLINE TThis& operator |=(const TChunk& val) {
        return Or(val);
    }

    template <class T>
    FORCED_INLINE TThis& operator ^=(const TBitMapOps<T>& bitmap) {
        return Xor(bitmap);
    }

    FORCED_INLINE TThis& operator ^=(const TChunk& val) {
        return Xor(val);
    }

    FORCED_INLINE TThis& operator <<=(size_t pos) {
      return LShift(pos);
    }

    FORCED_INLINE TThis& operator >>=(size_t pos) {
      return RShift(pos);
    }

    FORCED_INLINE TThis operator <<(size_t pos) const {
        return TThis(*this).LShift(pos);
    }

    FORCED_INLINE TThis operator >>(size_t pos) const {
        return TThis(*this).RShift(pos);
    }

    FORCED_INLINE bool operator [](size_t pos) const {
        return Get(pos);
    }

    FORCED_INLINE TReference operator [](size_t pos) {
        const bool fitStorage = Mask.ExpandBitSize(pos + 1);
        YASSERT(fitStorage);
        return TReference(&Mask.Data[pos >> DivCount], ModMask & pos);
    }

    FORCED_INLINE void Swap(TThis& bitmap) {
        DoSwap(Mask, bitmap.Mask);
    }

    FORCED_INLINE TThis& Set(size_t pos) {
        const bool fitStorage = Mask.ExpandBitSize(pos + 1);
        YASSERT(fitStorage);
        Mask.Data[pos >> DivCount] |= static_cast<TChunk>(1) << (pos & ModMask);
        return *this;
    }

    FORCED_INLINE TThis& Reset(size_t pos) {
        if ((pos >> DivCount) < Mask.GetChunkCapacity()) {
            Mask.Data[pos >> DivCount] &= ~(static_cast<TChunk>(1) << (pos & ModMask));
        }
        return *this;
    }

    FORCED_INLINE TThis& Flip(size_t pos) {
        const bool fitStorage = Mask.ExpandBitSize(pos + 1);
        YASSERT(fitStorage);
        Mask.Data[pos >> DivCount] ^= static_cast<TChunk>(1) << (pos & ModMask);
        return *this;
    }

    FORCED_INLINE bool Get(size_t pos) const {
        if ((pos >> DivCount) < Mask.GetChunkCapacity()) {
            return Mask.Data[pos >> DivCount] & (static_cast<TChunk>(1) << (pos & ModMask));
        }
        return false;
    }

    FORCED_INLINE bool Test(size_t n) const {
        return Get(n);
    }

    FORCED_INLINE TThis& Push(bool val) {
        LShift(1);
        return val ? Set(0) : *this;
    }

    FORCED_INLINE bool Pop() {
        bool val = Get(0);
        return RShift(1), val;
    }

    FORCED_INLINE TThis& Clear() {
        for (size_t i = 0; i < Mask.GetChunkCapacity(); ++i) {
            Mask.Data[i] = 0;
        }
        return *this;
    }

    // Returns bits capacity
    FORCED_INLINE size_t Size() const {
        return Mask.GetBitCapacity();
    }

    void Reserve(size_t bitCount) {
        VERIFY(Mask.ExpandBitSize(bitCount), "Exceeding bitmap storage capacity");
    }

    FORCED_INLINE bool Empty() const {
        for (size_t i = 0; i < Mask.GetChunkCapacity(); ++i)
            if (Mask.Data[i])
                return false;
        return true;
    }

    TThis& And(const TThis& bitmap) {
        Reserve(bitmap.Size());
        for (size_t i = 0; i < bitmap.Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] &= bitmap.Mask.Data[i];
        for (size_t i = bitmap.Mask.GetChunkCapacity(); i < Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] = 0;
        return *this;
    }

    template <class T>
    FORCED_INLINE TThis& And(const TBitMapOps<T>& bitmap) {
        return And(TThis(bitmap));
    }

    FORCED_INLINE TThis& And(const TChunk& val) {
        Mask.Data[0] &= val;
        for (size_t i = 1; i < Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] = 0;
        return *this;
    }

    TThis& Or(const TThis& bitmap) {
        Reserve(bitmap.Size());
        for (size_t i = 0; i < bitmap.Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] |= bitmap.Mask.Data[i];
        return *this;
    }

    template <class T>
    FORCED_INLINE TThis& Or(const TBitMapOps<T>& bitmap) {
        return Or(TThis(bitmap));
    }

    FORCED_INLINE TThis& Or(const TChunk& val) {
        Mask.Data[0] |= val;
        Mask.Sanitize();
        return *this;
    }

    TThis& Xor(const TThis& bitmap) {
        Reserve(bitmap.Size());
        for (size_t i = 0; i < bitmap.Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] ^= bitmap.Mask.Data[i];
        return *this;
    }

    template <class T>
    FORCED_INLINE TThis& Xor(const TBitMapOps<T>& bitmap) {
        return Xor(TThis(bitmap));
    }

    FORCED_INLINE TThis& Xor(const TChunk& val) {
        Mask.Data[0] ^= val;
        Mask.Sanitize();
        return *this;
    }

    FORCED_INLINE TThis& Flip() {
        for (size_t i = 0; i < Mask.GetChunkCapacity(); ++i)
            Mask.Data[i] = ~Mask.Data[i];
        Mask.Sanitize();
        return *this;
    }

    TThis& LShift(size_t shift) {
        if (shift != 0) {
            const size_t eshift = shift / BitsPerChunk;
            const size_t offset = shift % BitsPerChunk;
            const size_t subOffset = BitsPerChunk - offset;

            // Calculate new bit capacity.
            // Ignore high-order zero bits and don't expand capacity for them
            size_t zeroBits = 0;
            size_t chunk = 0;
            for (; chunk < Mask.GetChunkCapacity() && !Mask.Data[Mask.GetChunkCapacity() - chunk - 1]; ++chunk) {
                zeroBits += BitsPerChunk;
            }
            if (chunk < Mask.GetChunkCapacity() && 0 == (Mask.Data[Mask.GetChunkCapacity() - chunk - 1] >> subOffset))
                zeroBits += offset;

            // Expand only if current bitmap have non-zero bits and they are located in the left shifted range
            // Don't verify expand result, so l-shift of fixed bitmap will work in the same way as for unsigned integer.
            if (shift > zeroBits && zeroBits < Size())
                Mask.ExpandBitSize(Size() + shift - zeroBits);

            if (offset == 0) {
                for (size_t i = Mask.GetChunkCapacity() - 1; i >= eshift; --i) {
                    Mask.Data[i] = Mask.Data[i - eshift];
                }
            } else {
                for (size_t i = Mask.GetChunkCapacity() - 1; i > eshift; --i)
                    Mask.Data[i] = (Mask.Data[i - eshift] << offset) | (Mask.Data[i - eshift - 1] >> subOffset);
                if (eshift < Mask.GetChunkCapacity())
                    Mask.Data[eshift] = Mask.Data[0] << offset;
            }
            for (size_t i = 0; i < Min(eshift, Mask.GetChunkCapacity()); ++i)
                Mask.Data[i] = 0;

            // Cleanup extra high bits in the storage
            Mask.Sanitize();
        }
        return *this;
    }

    TThis& RShift(size_t shift) {
        if (shift != 0) {
            const size_t eshift = shift / BitsPerChunk;
            const size_t offset = shift % BitsPerChunk;
            if (eshift >= Mask.GetChunkCapacity()) {
                Clear();

            } else {
                const size_t limit = Mask.GetChunkCapacity() - eshift - 1;

                if (offset == 0) {
                    for (size_t i = 0; i <= limit; ++i) {
                        Mask.Data[i] = Mask.Data[i + eshift];
                    }
                } else {
                    const size_t subOffset = BitsPerChunk - offset;
                    for (size_t i = 0; i < limit; ++i)
                        Mask.Data[i] = (Mask.Data[i + eshift] >> offset) | (Mask.Data[i + eshift + 1] << subOffset);
                    Mask.Data[limit] = Mask.Data[Mask.GetChunkCapacity() - 1] >> offset;
                }

                for (size_t i = limit + 1; i < Mask.GetChunkCapacity(); ++i)
                    Mask.Data[i] = 0;
            }
        }
        return *this;
    }

    bool Equal(const TThis& bitmap) const {
        if (Mask.GetChunkCapacity() > bitmap.Mask.GetChunkCapacity()) {
            for (size_t i = bitmap.Mask.GetChunkCapacity(); i < Mask.GetChunkCapacity(); ++i) {
                if (0 != Mask.Data[i])
                    return false;
            }
        } else  if (Mask.GetChunkCapacity() < bitmap.Mask.GetChunkCapacity()) {
            for (size_t i = Mask.GetChunkCapacity(); i < bitmap.Mask.GetChunkCapacity(); ++i) {
                if (0 != bitmap.Mask.Data[i])
                    return false;
            }
        }
        size_t size = Min(Mask.GetChunkCapacity(), bitmap.Mask.GetChunkCapacity());
        for (size_t i = 0; i < size; ++i) {
            if (Mask.Data[i] != bitmap.Mask.Data[i])
                return false;
        }
        return true;
    }

    template <class T>
    FORCED_INLINE bool Equal(const TBitMapOps<T>& bitmap) const {
        return Equal(TThis(bitmap));
    }

    int Compare(const TThis& bitmap) const {
        size_t size = Min(Mask.GetChunkCapacity(), bitmap.Mask.GetChunkCapacity());
        int res = ::memcmp(Mask.Data, bitmap.Mask.Data, size * sizeof(TChunk));
        if (0 != res || Mask.GetChunkCapacity() == bitmap.Mask.GetChunkCapacity())
            return res;

        if (Mask.GetChunkCapacity() > bitmap.Mask.GetChunkCapacity()) {
            for (size_t i = bitmap.Mask.GetChunkCapacity(); i < Mask.GetChunkCapacity(); ++i) {
                if (0 != Mask.Data[i])
                    return 1;
            }
        } else {
            for (size_t i = Mask.GetChunkCapacity(); i < bitmap.Mask.GetChunkCapacity(); ++i) {
                if (0 != bitmap.Mask.Data[i])
                    return -1;
            }
        }
        return 0;
    }

    template <class T>
    FORCED_INLINE int Compare(const TBitMapOps<T>& bitmap) const {
        return Compare(TThis(bitmap));
    }

    // For backward compatibility
    FORCED_INLINE static int Compare(const TThis& l, const TThis& r) {
        return l.Compare(r);
    }

    size_t FirstNonZeroBit() const {
        for (size_t i = 0; i < Mask.GetChunkCapacity(); ++i) {
            if (Mask.Data[i]) {
                TChunk c = Mask.Data[i];
                size_t k = 0;
                while (!(c & 1)) {
                    ++k;
                    c >>= 1;
                }
                return BitsPerChunk * i + k;
            }
        }
        return Size();
    }

    FORCED_INLINE size_t Count() const {
        size_t count = 0;
        for (size_t i = 0; i < Mask.GetChunkCapacity(); ++i)
            count += CountBits(Mask.Data[i]);
        return count;
    }

    void Save(TOutputStream* out) const {
        ::Save(out, ui8(sizeof(TChunk)));
        ::Save(out, ui64(Size()));
        ::SavePodArray(out, Mask.Data, Mask.GetChunkCapacity());
    }

    void Load(TInputStream* inp) {
        ui8 chunkSize = 0;
        ::Load(inp, chunkSize);
        VERIFY(size_t(chunkSize) == sizeof(TChunk), "Chunk size is not the same");

        ui64 bitCount64 = 0;
        ::Load(inp, bitCount64);
        size_t bitCount = size_t(bitCount64);
        Reserve(bitCount);

        size_t chunkCount = 0;
        if (bitCount > 0) {
            chunkCount = ((bitCount - 1) >> DivCount) + 1;
            ::LoadPodArray(inp, Mask.Data, chunkCount);
        }

        if (chunkCount < Mask.GetChunkCapacity()) {
            ::memset(Mask.Data + chunkCount, 0, (Mask.GetChunkCapacity() - chunkCount) * sizeof(TChunk));
        }
        Mask.Sanitize();
    }
};

template <class X, class Y>
inline TBitMapOps<X> operator &(const TBitMapOps<X>& x, const TBitMapOps<Y>& y) {
    return TBitMapOps<X>(x).And(y);
}

template <class X>
inline TBitMapOps<X> operator &(const TBitMapOps<X>& x, const typename TBitMapOps<X>::TChunk& y) {
    return TBitMapOps<X>(x).And(y);
}

template <class X>
inline TBitMapOps<X> operator &(const typename TBitMapOps<X>::TChunk& x, const TBitMapOps<X>& y) {
    return TBitMapOps<X>(x).And(y);
}

template <class X, class Y>
inline TBitMapOps<X> operator |(const TBitMapOps<X>& x, const TBitMapOps<Y>& y) {
    return TBitMapOps<X>(x).Or(y);
}

template <class X>
inline TBitMapOps<X> operator |(const TBitMapOps<X>& x, const typename TBitMapOps<X>::TChunk& y) {
    return TBitMapOps<X>(x).Or(y);
}

template <class X>
inline TBitMapOps<X> operator |(const typename TBitMapOps<X>::TChunk& x, const TBitMapOps<X>& y) {
    return TBitMapOps<X>(x).Or(y);
}

template <class X, class Y>
inline TBitMapOps<X> operator ^(const TBitMapOps<X>& x, const TBitMapOps<Y>& y) {
    return TBitMapOps<X>(x).Xor(y);
}

template <class X>
inline TBitMapOps<X> operator ^(const TBitMapOps<X>& x, const typename TBitMapOps<X>::TChunk& y) {
    return TBitMapOps<X>(x).Xor(y);
}

template <class X>
inline TBitMapOps<X> operator ^(const typename TBitMapOps<X>::TChunk& x, const TBitMapOps<X>& y) {
    return TBitMapOps<X>(x).Xor(y);
}

template <class X>
inline TBitMapOps<X> operator ~(const TBitMapOps<X>& x) {
    return TBitMapOps<X>(x).Flip();
}

/////////////////// Specialization ///////////////////////////

template <size_t BitCount, typename TChunkType = ui64>
class TBitMap : public TBitMapOps< TFixedBitMapTraits<BitCount, TChunkType> > {
private:
    typedef TBitMapOps< TFixedBitMapTraits<BitCount, TChunkType> > TBase;
public:
    TBitMap()
        : TBase()
    {
    }

    TBitMap(typename TBase::TChunk val)
        : TBase(val)
    {
    }

    TBitMap(const TBitMap<BitCount, TChunkType>& bitmap)
        : TBase(bitmap)
    {
    }

    template <class T>
    TBitMap(const TBitMapOps<T>& bitmap)
        : TBase(bitmap)
    {
    }
};

typedef TBitMapOps< TDynamicBitMapTraits<ui64> > TDynBitMap;
