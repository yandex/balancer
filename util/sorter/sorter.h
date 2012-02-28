#pragma once

#include <util/system/defaults.h>
#include <util/stream/input.h>
#include <util/generic/vector.h>
#include <util/generic/stroka.h>
#include <util/generic/algorithm.h>
#include <util/generic/yexception.h>

namespace NSorter {
    class IPortionFileNameCallback;
}

namespace NSorterPrivate {

template <class T>
struct THeapElem {
    TInputStream* F;
    T Obj;

    THeapElem()
        : F(0)
    {
    }
};

template <class T>
inline bool operator < (const THeapElem<T>& a, const THeapElem<T>& b) {
    return b.Obj < a.Obj;
}

int CountPortions(NSorter::IPortionFileNameCallback* fname);
TInputStream* HeapIterInput(const Stroka& filename, size_t bufferSize, bool compressed);

static const size_t NO_BUF_SIZE = (size_t)-1;

}

namespace NSorter {

class IPortionFileNameCallback {
public:
    virtual ~IPortionFileNameCallback() {}
    virtual Stroka GetPortionName(int n) = 0;
};

class TPortionFileNameCallback2 : public IPortionFileNameCallback {
public:
    TPortionFileNameCallback2(const char* cookie = "")
        : Cookie(cookie)
    {}
    virtual Stroka GetPortionName(int n);
private:
    Stroka Cookie;
};

template <class T>
class TIterator {
private:
    typedef NSorterPrivate::THeapElem<T> TElem;
    yvector<TElem> Heap;
    size_t BufSize;
    int Rport;
public:
    TIterator(size_t bufSize = NSorterPrivate::NO_BUF_SIZE)
        : BufSize(bufSize)
    {
    }

    ~TIterator() {
        Close();
    };

    bool Finished() const {
        return Rport == 0;
    }

    const T& operator *() const {
        return Heap[Rport - 1].Obj;
    }

    const T* operator ->() const {
        return &Heap[Rport - 1].Obj;
    }

    void operator ++() {
        size_t loaded = Heap[Rport - 1].F->Load(&Heap[Rport - 1].Obj, sizeof(T));
        if (EXPECT_TRUE(sizeof(T) == loaded))
            PushHeap(Heap.begin(), Heap.begin() + Rport);
        else
            Rport--;
        if (Rport)
            PopHeap(Heap.begin(), Heap.begin() + Rport);
    }

    void Close() {
        for (size_t i = 0; i < Heap.size(); i++)
            delete Heap[i].F;
        Heap.clear();
    }

    void Init(int numportions, IPortionFileNameCallback* portionFilename, int firstportion, bool compressed);
};

template <class T>
void TIterator<T>::Init(int numportions, IPortionFileNameCallback* portionFilename, int firstportion, bool compressed) {
    if (numportions == 0) {
        Rport = 0;
        return;
    }
    if (numportions == -1)
        numportions = NSorterPrivate::CountPortions(portionFilename);
    Heap.reserve(numportions);
    size_t iBufSize = (NSorterPrivate::NO_BUF_SIZE == BufSize) ? 1 << 16 : BufSize/numportions;
    for (ui32 i = 0; i < (ui32)numportions; ++i) {
        Heap.push_back();
        Stroka fname = portionFilename->GetPortionName(i + firstportion);
        THolder<TInputStream> fIn(NSorterPrivate::HeapIterInput(fname, iBufSize, compressed));
        Heap.back().F = fIn.Get();
        size_t loaded = fIn->Load(&Heap.back().Obj, sizeof(T));
        if (loaded == 0)
            Heap.pop_back();
        else if (loaded == sizeof(T))
            fIn.Release();
        else
            ythrow yexception() << "bad file size";
    }
    Rport = (int)Heap.size();
    if (!Heap.empty()) {
        MakeHeap(Heap.begin(), Heap.end());
        PopHeap(Heap.begin(), Heap.end());
    }
    if (Rport < numportions)
        throw yexception() << "TIterator: " << numportions - Rport << " portions are empty"; // please do not alter this text
}

template <class T>
class TUniqIterator : public TIterator<T> {
private:
    T Last;

public:
    TUniqIterator(size_t bufSize = NSorterPrivate::NO_BUF_SIZE)
        : TIterator<T>(bufSize)
        , Last()
    {
    }

    void Init(int n, IPortionFileNameCallback* p, int f, bool c) {
        TIterator<T>::Init(n, p, f, c);
        if (!TIterator<T>::Finished())
            Last = TIterator<T>::operator*();
    }

    void operator ++() {
        TIterator<T>::operator++();
        while (!TIterator<T>::Finished() && Last == TIterator<T>::operator*())
            TIterator<T>::operator++();
        if (!TIterator<T>::Finished())
            Last = TIterator<T>::operator*();
    }
};

class TSorterBase {
protected:
    enum SORTER_FLAGS {
        IS_INTERNAL_ALLOCATION = 1,
        FREE_MEMORY_ON_CLOSE   = 2,
        KEEP_PORTIONS_FILES    = 8,
        DELETE_PORTION_FNAME_OBJ = 16,
    };
    int Flags;
    bool Compressed;
    int CurPortion;
    void* Store;
    const size_t ElementSize; // for internal use
    size_t StorePos;
    size_t StoreCaps;
    // Two following value may not belong to derived class,
    // because it will be used after derived class's destructor
    IPortionFileNameCallback* PortionFilename;
protected:
    TSorterBase(const size_t ElementSize, bool compressed);
    virtual ~TSorterBase();

    void Allocate(size_t capacity, void *extAlloc);
    void Deallocate();
    void WritePortion();

public:
    void SetFileNameCallback(IPortionFileNameCallback *f, bool dont_delete_object = true);
    int NumPortions() const {
        return CurPortion;
    }
    void Restart();
    DECLARE_NOCOPY(TSorterBase);
};

template<class T>
struct TSimpleSort {
    typedef NSorter::TIterator<T> TIterator;
    bool SortPortion(T *start, size_t len) const {
        QuickSort(start, start + len);
        return true;
    }
};

template<class T>
struct TSmartUniq {
    typedef TUniqIterator<T> TIterator;
    bool SortPortion(T *start, size_t &len) const {
        T *end = start + len;
        Sort(start, end);
        end = Unique(start, end);
        size_t oldlen = len;
        len = end - start;
        //warnx("TSmartUniq: %lu -> %lu (%s)", oldlen, len, len > oldlen * 2 / 5? "true" : "false");
        return len > oldlen * 2 / 5;
    }
};

template <class T, class TSortMode = TSimpleSort<T> >
class TSorter : public TSorterBase {
private:
    TSortMode SortMode;
    void SortAndFlush(bool doFlush);
public:
    TSorter(size_t size, T* extAlloc = 0, bool compressed = false);
    virtual ~TSorter();
    void PushBack(const T& x);
    void Close(typename TSortMode::TIterator& iter, bool freeMemory = true);
    void SortAndRelease(T*& storeStart, T*& storeEnd, bool dontevensort = false);
};

template <class T, class TSortMode>
inline TSorter<T, TSortMode>::TSorter(size_t size, T* extAlloc, bool compressed)
    : TSorterBase(sizeof(T), compressed)
{
    TSorterBase::Allocate(size, extAlloc);
}

template <class T, class TSortMode>
TSorter<T, TSortMode>::~TSorter() {
}

template <class T, class TSortMode>
inline void TSorter<T, TSortMode>::PushBack(const T& x) {
    if (EXPECT_FALSE(StorePos >= StoreCaps))
        SortAndFlush(false);
// *1*   new ((T*)Store + StorePos++) T(x);
// *2*   memcpy((T*)Store + StorePos++, &x, sizeof(T));
    ((T*)Store)[StorePos++] = x;
}

template <class T, class TSortMode>
inline void TSorter<T, TSortMode>::SortAndFlush(bool doFlush) {
    if (!StorePos)
        return;
    if (!SortMode.SortPortion((T*)Store, StorePos) && !doFlush)
        return; // SortPortion may free up some space and tell up to use it (return false)
    TSorterBase::WritePortion();
    StorePos = 0;
// *1*   for(T *t = (T*)Store; t != storeEnd; t++)
// *1*       t->~T();
    CurPortion++;
}

template <class T, class TSortMode>
inline void TSorter<T, TSortMode>::SortAndRelease(T*& storeStart, T*& storeEnd, bool dontevensort) {
    assert(!CurPortion);
    if (!StorePos) {
        storeStart = storeEnd = 0;
        return;
    }
    if (!dontevensort)
        SortMode.SortPortion((T*)Store, StorePos);
    storeStart = (T*)Store;
    storeEnd = (T*)Store + StorePos;
    StorePos = 0;
}

template <class T, class TSortMode>
void TSorter<T, TSortMode>::Close(typename TSortMode::TIterator& iter, bool freeMemory) {
    SortAndFlush(true);
    if (freeMemory) {
        TSorterBase::Deallocate();
    }
    iter.Init(CurPortion, PortionFilename, 0, Compressed);
}

}
