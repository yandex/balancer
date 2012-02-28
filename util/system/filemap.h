#pragma once

#include "file.h"
#include "align.h"
#include "yassert.h"

#include <new>
#include <cstdio>

#include <util/generic/ptr.h>
#include <util/generic/utility.h>
#include <util/generic/yexception.h>

struct TMemoryMapCommon {
    struct TMapResult {
        inline size_t MappedSize() const throw () {
            return Size - Head;
        }

        inline void* MappedData() const throw () {
            return Ptr ? (void*)((char*)Ptr + Head) : 0;
        }

        inline bool IsMapped() const throw () {
            return Ptr != 0;
        }

        inline void Reset() throw () {
            Ptr = 0;
            Size = 0;
            Head = 0;
        }

        void* Ptr;
        size_t Size;
        i32 Head;

        TMapResult(void) throw () {
            Reset();
        }
    };

    enum OMode {
        oRdOnly = 1,
        oRdWr = 2,

        oAccessMask = 3,
        oNotGreedy = 8,
        oPrecharge = 16,
    };
};

class TMemoryMap: public TMemoryMapCommon {
    public:
        TMemoryMap(const char* name, OMode om = oRdOnly);
        TMemoryMap(const char* name, i64 length, OMode om = oRdOnly);
        TMemoryMap(FILE* f, OMode om = oRdOnly, const char *dbg_name = "unknown");
        TMemoryMap(const TFile& file, OMode om = oRdOnly, const char *dbg_name = "unknown");

        ~TMemoryMap() throw ();

        TMapResult Map(i64 offset, size_t size, const char *dbg_name = 0);
        bool Unmap(TMapResult region);

        i64 Length() const throw ();
        bool IsOpen() const throw ();
        TFile GetFile() const throw ();

        void SetSequential();

        /*
         * deprecated
         */
        bool Unmap(void* ptr, size_t size);

    private:
        class TImpl;
        TSimpleIntrusivePtr<TImpl> Impl_;
};

class TFileMap: public TMemoryMapCommon {
    public:
        TFileMap(const TMemoryMap& map) throw ();
        TFileMap(const char* name, OMode om = oRdOnly);
        TFileMap(const char* name, i64 length, OMode om = oRdOnly);
        TFileMap(FILE* f, OMode om = oRdOnly);
        TFileMap(const TFile& file, OMode om = oRdOnly);
        TFileMap(const TFileMap& fm) throw ();

        ~TFileMap() throw ();

        void Map(i64 offset, size_t size, const char* dbgName = 0);
        void Unmap();

        void Flush(void* ptr, size_t size) {
            Flush(ptr, size, true);
        }

        void Flush() {
            Flush(Ptr(), MappedSize());
        }

        void FlushAsync(void* ptr, size_t size) {
            Flush(ptr, size, false);
        }

        void FlushAsync() {
            FlushAsync(Ptr(), MappedSize());
        }

        inline i64 Length() const throw () {
            return Map_.Length();
        }

        inline bool IsOpen() const throw () {
            return Map_.IsOpen();
        }

        inline void* Ptr() const throw () {
            return Region_.MappedData();
        }

        inline size_t MappedSize() const throw () {
            return Region_.MappedSize();
        }

        TFile GetFile() const throw () {
            return Map_.GetFile();
        }

        int Precharge(size_t pos = 0, size_t size = (size_t)-1) const;

    private:
        void Flush(void* ptr, size_t size, bool sync);

        TMemoryMap Map_;
        TMapResult Region_;
};

//Use it if need one map on the whole object
class TMappedFile {
private:
    TFileMap* Map_;

private:
    TMappedFile(TFileMap* map, const char *dbg_name);

public:
    TMappedFile() {
        Map_ = NULL;
    }

    ~TMappedFile() {
        term();
    }

    explicit TMappedFile(const char* name) {
        Map_ = NULL;
        init(name);
    }

    TMappedFile(const TFile& file, TFileMap::OMode om = TFileMap::oRdOnly, const char* dbgName = "unknown");

    void init(const char *name, TFileMap::OMode om = TFileMap::oRdOnly);

    void init(const TFile&, TFileMap::OMode om = TFileMap::oRdOnly, const char* dbgName = "unknown");

    void init(const char *name, size_t length, TFileMap::OMode om = TFileMap::oRdOnly);

    void flush();

    void term() {
        if (Map_) {
            Map_->Unmap();
            delete Map_;
            Map_ = NULL;
        }
    }

    size_t getSize() const {
        return (Map_ ? Map_->MappedSize() : 0);
    }

    void* getData(size_t pos = 0) const {
        YASSERT(!Map_ || (pos <= getSize()));
        return (Map_ ? (void *)((unsigned char *)Map_->Ptr() + pos) : NULL);
    }

    int precharge(size_t pos = 0, size_t size = (size_t)-1) const;

    void swap(TMappedFile &file) throw () {
        DoSwap(Map_, file.Map_);
    }
};

template<class T>
class TFileMappedArray : private TMappedFile {
  private:
    const T*     Ptr_;
    const T*     End_;
    size_t Size_;
    char DummyData_[sizeof(T) + PLATFORM_DATA_ALIGN];
    mutable THolder<T, TDestructor> Dummy_;
  public:
    TFileMappedArray()
        : Ptr_(0)
        , End_(0)
        , Size_(0)
    {
    }
    ~TFileMappedArray() {
        /// don't call term() here as ~MappedFile also calls it's term()
        Ptr_ = 0;
        End_ = 0;
    }
    void init(const char *name) {
        TMappedFile::init(name);
        if (getSize() % sizeof(T)) {
            term();
            ythrow yexception() << "incorrect size of file " <<  name;
        }
        Ptr_ = (const T*)getData();
        Size_ = getSize() / sizeof(T);
        End_ = Ptr_ + Size_;
    }
    void term() {
        TMappedFile::term();
        Ptr_ = 0;
        Size_ = 0;
        End_ = 0;
    }
    void precharge() {
        TMappedFile::precharge();
    }
    const T &operator[](size_t pos) const {
        YASSERT(pos < size());
        return Ptr_[pos];
    }
    size_t size() const {
        return Size_;
    }
    const T& GetAt(size_t pos) const {
        if (pos < Size_)
            return Ptr_[pos];
        return Dummy();
    }
    void SetDummy(const T &n_Dummy) {
        Dummy_.Destroy();
        Dummy_.Reset(new (DummyData()) T(n_Dummy));
    }
    inline char* DummyData() const throw () {
        return AlignUp((char*)DummyData_);
    }
    inline const T& Dummy() const {
        if (!Dummy_) {
            Dummy_.Reset(new (DummyData()) T());
        }

        return *Dummy_;
    }
    bool empty() const {
        return 0 == Size_;
    }
    const T* begin() const throw () {
        return Ptr_;
    }
    const T* end() const throw () {
        return End_;
    }
};

class TMappedAllocation {
public:
    TMappedAllocation(size_t size = 0, bool shared = false);
    ~TMappedAllocation() {
        Dealloc();
    }
    void* Alloc(size_t size);
    void Dealloc();
    void* Ptr() const {
        return Ptr_;
    }
    char* Data(ui32 pos = 0) const {
        return (char*)(Ptr_ ? ((char*)Ptr_ + pos) : 0);
    }
    size_t MappedSize() const {
        return Size_;
    }
    void swap(TMappedAllocation &with);
private:
    void* Ptr_;
    size_t Size_;
    bool Shared_;
#ifdef _win_
    void* Mapping_;
#endif
    DECLARE_NOCOPY(TMappedAllocation);
};

template<class T>
class TMappedArray : private TMappedAllocation {
public:
    TMappedArray(size_t siz = 0)
      : TMappedAllocation(0) {
        if (siz)
            Create(siz);
    }
    ~TMappedArray() {
        Destroy();
    }
    T *Create(size_t siz) {
        assert(MappedSize() == 0 && Ptr() == 0);
        T* arr = (T*)Alloc((sizeof(T) * siz));
        if (!arr)
            return 0;
        assert(MappedSize() == sizeof(T) * siz);
        for (size_t n = 0; n < siz; n++)
            new(&arr[n]) T();
        return arr;
    }
    void Destroy() {
        T* arr = (T*)Ptr();
        if (arr) {
            for (size_t n = 0; n < size(); n++)
                arr[n].~T();
            Dealloc();
        }
    }
    T &operator[](size_t pos) {
        assert(pos < size());
        return ((T*)Ptr())[pos];
    }
    const T &operator[](size_t pos) const {
        assert(pos < size());
        return ((T*)Ptr())[pos];
    }
    T* begin(){
        return (T*)Ptr();
    }
    T* end(){
        return (T*)((char*)Ptr() + MappedSize());
    }
    size_t size() const {
        return MappedSize()/sizeof(T);
    }
    void swap(TMappedArray<T> &with) {
        TMappedAllocation::swap(with);
    }
    DECLARE_NOCOPY(TMappedArray);
};
