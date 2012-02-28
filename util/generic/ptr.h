#pragma once

#include "utility.h"
#include "intrlist.h"
#include "refcount.h"

#include <util/system/yassert.h>
#include <util/system/defaults.h>

template<class T>
inline void CheckedDelete(T* t)
{
    // from boost
    // intentionally complex - simplification causes regressions
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete t;
}

template<class T>
inline void CheckedArrayDelete(T* t)
{
    // from boost
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] t;
}

class TDelete {
    public:
        template <class T>
        static inline void Destroy(T* t) throw () {
            CheckedDelete(t);
        }

        /*
         * special handling for void* - call ::operator delete()
         */
        static void Destroy(void* t) throw ();
};

class TDeleteArray {
    public:
        template <class T>
        static inline void Destroy(T* t) throw () {
            CheckedArrayDelete(t);
        }
};

class TDestructor {
    public:
        template <class T>
        static inline void Destroy(T* t) throw () {
            (void)t;
            t->~T();
        }
};

class TFree {
    public:
        template <class T>
        static inline void Destroy(T* t) throw () {
            DoDestroy((void*)t);
        }

    private:
        /*
         * we do not want dependancy on cstdlib here...
         */
        static void DoDestroy(void* t) throw ();
};

template <class D>
struct TDestroyFunctor {
    template <class T>
    inline void operator() (T* t) const throw () {
        D::Destroy(t);
    }
};

template <class Base, class T>
class TPointerCommon {
    public:
        inline T* operator-> () const throw () {
            return AsT();
        }

        template <class C>
        inline bool operator== (const C& p) const throw () {
            return (p == AsT());
        }

        template <class C>
        inline bool operator!= (const C& p) const throw () {
            return (p != AsT());
        }

        inline bool operator! () const throw () {
            return 0 == AsT();
        }

    protected:
        inline T* AsT() const throw () {
            return (static_cast<const Base*>(this))->Get();
        }

        static inline T* DoRelease(T*& t) throw () {
            T* ret = t; t = 0; return ret;
        }
};

template <class Base, class T>
class TPointerBase: public TPointerCommon<Base, T> {
    public:
        inline T& operator* () const throw () {
            YASSERT(this->AsT());

            return *(this->AsT());
        }

        inline T& operator[] (size_t n) const throw () {
            YASSERT(this->AsT());

            return (this->AsT())[n];
        }
};

/*
 * void*-like pointers does not have operator*
 */
template <class Base>
class TPointerBase<Base, void>: public TPointerCommon<Base, void> {
};

template <class T, class D = TDelete>
class TAutoPtr: public TPointerBase<TAutoPtr<T, D>, T> {
    public:
        inline TAutoPtr(T* t = 0) throw ()
            : T_(t)
        {
        }

        inline TAutoPtr(const TAutoPtr& t) throw ()
            : T_(t.Release())
        {
        }

        inline ~TAutoPtr() throw () {
            DoDestroy();
        }

        inline TAutoPtr& operator= (const TAutoPtr& t) throw () {
            if (this != &t) {
                Reset(t.Release());
            }

            return *this;
        }

        inline T* Release() const throw () {
            return this->DoRelease(T_);
        }

        inline void Reset(T* t) throw () {
            if (T_ != t) {
                DoDestroy(); T_ = t;
            }
        }

        inline void Destroy() throw () {
            Reset(0);
        }

        inline void Swap(TAutoPtr& r) throw () {
            DoSwap(T_, r.T_);
        }

        inline T* Get() const throw () {
            return T_;
        }

    private:
        inline void DoDestroy() throw () {
            if (T_) {
                D::Destroy(T_);
            }
        }

    private:
        mutable T* T_;
};

template <class T, class D = TDelete>
class THolder: public TPointerBase<THolder<T, D>, T> {
    public:
        inline THolder(T* t = 0) throw ()
            : T_(t)
        {
        }

        inline THolder(TAutoPtr<T, D> t) throw ()
            : T_(t.Release())
        {
        }

        inline ~THolder() throw () {
            DoDestroy();
        }

        inline void Destroy() throw () {
            Reset(0);
        }

        inline T* Release() throw () {
            return this->DoRelease(T_);
        }

        inline void Reset(T* t) throw () {
            if (T_ != t) {
                DoDestroy(); T_ = t;
            }
        }

        inline void Swap(THolder& r) throw () {
            DoSwap(T_, r.T_);
        }

        inline T* Get() const throw () {
            return T_;
        }

        inline operator TAutoPtr<T, D> () throw () {
            return Release();
        }

    private:
        inline void DoDestroy() throw () {
            if (T_) {
                D::Destroy(T_);
            }
        }

    private:
        THolder(const THolder&);
        THolder& operator= (const THolder&);

    private:
        T* T_;
};

/*
 * usage:
 * class T: public TRefCounted<T>
 * and we get methods Ref() && UnRef() with
 * proper destruction of last UnRef()
 */
template <class T, class C = TSimpleCounter, class D = TDelete>
class TRefCounted {
    public:
        inline TRefCounted(long initval = 0) throw ()
            : Counter_(initval)
        {
        }

        inline ~TRefCounted() throw () {
        }

        inline void Ref() throw () {
            Counter_.Inc();
        }

        inline void UnRef() throw () {
            if (!Counter_.Dec()) {
                D::Destroy(static_cast<T*>(this));
            }
        }

        inline long RefCount() const throw () {
            return Counter_.Val();
        }

        inline void DecRef() throw () {
            Counter_.Dec();
        }

        TRefCounted(const TRefCounted&)
            : Counter_(0)
        {
        }

        void operator =(const TRefCounted&) {
        }

    private:
        C Counter_;
};

template <class T>
class TDefaultIntrusivePtrOps {
    public:
        static inline void Ref(T* t) throw () {
            YASSERT(t);

            t->Ref();
        }

        static inline void UnRef(T* t) throw () {
            YASSERT(t);

            t->UnRef();
        }

        static inline void DecRef(T* t) throw () {
            YASSERT(t);

            t->DecRef();
        }
};

template <
            class T,
            class Ops = TDefaultIntrusivePtrOps<T>
         >
class TIntrusivePtr: public TPointerBase<TIntrusivePtr<T, Ops>, T> {
    public:
        inline TIntrusivePtr(T* t = 0) throw ()
            : T_(t)
        {
            Ops();
            Ref();
        }

        inline ~TIntrusivePtr() throw () {
            UnRef();
        }

        inline TIntrusivePtr(const TIntrusivePtr& p) throw ()
            : T_(p.T_)
        {
            Ref();
        }

        inline TIntrusivePtr& operator= (TIntrusivePtr p) throw () {
            if (&p != this) {
                p.Swap(*this);
            }

            return *this;
        }

        inline T* Get() const throw () {
            return T_;
        }

        inline void Swap(TIntrusivePtr& r) throw () {
            DoSwap(T_, r.T_);
        }

        inline void Drop() throw () {
            TIntrusivePtr(0).Swap(*this);
        }

        inline T* Release() const throw () {
            T* res = T_;
            if (T_) {
                Ops::DecRef(T_);
                T_ = 0;
            }
            return res;
        }

    private:
        inline void Ref() throw () {
            if (T_) {
                Ops::Ref(T_);
            }
        }

        inline void UnRef() throw () {
            if (T_) {
                Ops::UnRef(T_);
            }
        }

    private:
        mutable T* T_;
};

// Behaves like TIntrusivePtr but returns const T* to prevent user from accidentally modifying the referenced object.
template <
            class T,
            class Ops = TDefaultIntrusivePtrOps<T>
         >
class TIntrusiveConstPtr {
    public:
        inline TIntrusiveConstPtr(T* t = NULL) throw ()  // we need a non-const pointer to Ref(), UnRef() and eventually delete it.
            : T_(t)
        {
            Ops();
            Ref();
        }

        inline ~TIntrusiveConstPtr() throw () {
            UnRef();
        }

        inline TIntrusiveConstPtr(const TIntrusiveConstPtr& p) throw ()
            : T_(p.T_)
        {
            Ref();
        }

        inline TIntrusiveConstPtr(const TIntrusivePtr<T, Ops>& p) throw ()
            : T_(p.Get())
        {
            Ref();
        }

        inline TIntrusiveConstPtr& operator= (TIntrusiveConstPtr p) throw () {
            if (&p != this) {
                p.Swap(*this);
            }

            return *this;
        }

        inline const T* Get() const throw () {
            return T_;
        }

        inline void Swap(TIntrusiveConstPtr& r) throw () {
            DoSwap(T_, r.T_);
        }

        inline void Drop() throw () {
            TIntrusiveConstPtr(0).Swap(*this);
        }

        inline const T* operator-> () const throw () {
            return Get();
        }

        template <class C>
        inline bool operator== (const C& p) const throw () {
            return Get() == p;
        }

        template <class C>
        inline bool operator!= (const C& p) const throw () {
            return Get() != p;
        }

        inline bool operator! () const throw () {
            return Get() == NULL;
        }

        inline const T& operator* () const throw () {
            YASSERT(Get() != NULL);
            return *Get();
        }

    private:
        inline void Ref() throw () {
            if (T_ != NULL) {
                Ops::Ref(T_);
            }
        }

        inline void UnRef() throw () {
            if (T_ != NULL) {
                Ops::UnRef(T_);
            }
        }

    private:
        T* T_;
};

template <class T, class Ops>
class TSimpleIntrusiveOps {
        typedef void (*TFunc)(T*);

        static void DoRef(T* t) throw () {
            Ops::Ref(t);
        }

        static void DoUnRef(T* t) throw () {
            Ops::UnRef(t);
        }

    public:
        inline TSimpleIntrusiveOps() throw () {
            InitStaticOps();
        }

        inline ~TSimpleIntrusiveOps() throw () {
        }

        static inline void Ref(T* t) throw () {
            Ref_(t);
        }

        static inline void UnRef(T* t) throw () {
            UnRef_(t);
        }

    private:
        static inline void InitStaticOps() throw () {
            if (!Ref_) {
                Ref_ = DoRef;
                UnRef_ = DoUnRef;
            }
        }

    private:
        static TFunc Ref_;
        static TFunc UnRef_;
};

template <class T, class Ops>
typename TSimpleIntrusiveOps<T, Ops>::TFunc TSimpleIntrusiveOps<T, Ops>::Ref_ = 0;

template <class T, class Ops>
typename TSimpleIntrusiveOps<T, Ops>::TFunc TSimpleIntrusiveOps<T, Ops>::UnRef_ = 0;

/*
 * this is really template typedef
 */
template <
            class T,
            class Ops = TDefaultIntrusivePtrOps<T>
         >
class TSimpleIntrusivePtr: public TIntrusivePtr< T, TSimpleIntrusiveOps<T, Ops> > {
        typedef TIntrusivePtr< T, TSimpleIntrusiveOps<T, Ops> > Base;
    public:
        inline TSimpleIntrusivePtr(T* t = 0) throw ()
            : Base(t)
        {
        }

        inline TSimpleIntrusivePtr(const TSimpleIntrusivePtr& p) throw ()
            : Base(p)
        {
        }
};

template <class T, class C = TSimpleCounter, class D = TDelete>
class TSharedPtr: public TPointerBase<TSharedPtr<T, C, D>, T> {
        template <class TT, class CC, class DD> friend class TSharedPtr;

    public:
        inline TSharedPtr(T* t = 0) {
            THolder<T, D> h(t);

            C_ = new C(1);
            T_ = h.Release();
        }

        inline TSharedPtr(TAutoPtr<T, D> t)
            : T_(t.Get())
            , C_(new C(1))
        {
            t.Release();
        }

        inline ~TSharedPtr() {
            UnRef();
        }

        inline TSharedPtr(const TSharedPtr& t) throw ()
            : T_(t.T_)
            , C_(t.C_)
        {
            Ref();
        }

        template <class TT>
        inline TSharedPtr(const TSharedPtr<TT, C, D>& t) throw ()
            : T_(t.T_)
            , C_(t.C_)
        {
            Ref();
        }

        inline TSharedPtr& operator= (const TSharedPtr& t) throw () {
            if (this != &t) {
                TSharedPtr(t).Swap(*this);
            }

            return *this;
        }

        inline void Reset(const TSharedPtr& t) throw () {
            *this = t;
        }

        inline T* Get() const throw () {
            return T_;
        }

        inline void Swap(TSharedPtr& r) throw () {
            DoSwap(T_, r.T_);
            DoSwap(C_, r.C_);
        }

        inline long RefCount() const throw () {
            return C_->Val();
        }

    private:
        inline void Ref() throw () {
            C_->Inc();
        }

        inline void UnRef() throw () {
            if (!C_->Dec()) {
                DoDestroy();
            }
        }

        inline void DoDestroy() throw () {
            if (T_) {
                D::Destroy(T_);
            }

            delete C_;
        }

    private:
        T* T_;
        C* C_;
};

template <class T, class D = TDelete>
class TLinkedPtr: public TPointerBase<TLinkedPtr<T, D>, T>
                , public TIntrusiveListItem<TLinkedPtr<T, D> > {
        typedef TIntrusiveListItem<TLinkedPtr> TListBase;
    public:
        inline TLinkedPtr(T* t) throw ()
            : TListBase()
            , T_(t)
        {
            YASSERT(Last());
        }

        inline TLinkedPtr(const TLinkedPtr& r) throw ()
            : TListBase()
            , T_(r.T_)
        {
            this->LinkBefore((TLinkedPtr&)r);
            YASSERT(!Last());
        }

        inline ~TLinkedPtr() throw () {
            DoDestroy();
        }

        inline TLinkedPtr& operator= (const TLinkedPtr& t) throw () {
            if (this != &t) {
                DoDestroy();
                T_ = t.T_;
                this->LinkBefore((TLinkedPtr&)t);
                YASSERT(!Last());
            }

            return *this;
        }

        inline T* Get() const throw () {
            return T_;
        }

        inline void Swap(TLinkedPtr& r) throw () {
            DoSwap(*this, r);
        }

    private:
        inline bool Last() const throw () {
            return this == this->Next();
        }

        inline void DoDestroy() throw () {
            if (T_ && Last()) {
                D::Destroy(T_);
            }
        }

    private:
        T* T_;
};

class TCopyClone {
    public:
        template <class T>
        static inline T* Copy(T* t) {
            if (t)
                return t->Clone();
            return 0;
        }
};

class TCopyNew {
    public:
        template <class T>
        static inline T* Copy(T* t) {
            if (t)
                return new T(*t);
            return 0;
        }
};

template <class T, class C = TCopyNew, class D = TDelete>
class TCopyPtr: public TPointerBase<TCopyPtr<T, C, D>, T> {
    public:
        inline TCopyPtr(T* t = 0) throw ()
            : T_(t)
        {
        }

        inline TCopyPtr(const TCopyPtr& t)
            : T_(C::Copy(t.Get()))
        {
        }

        inline ~TCopyPtr() throw () {
            DoDestroy();
        }

        inline TCopyPtr& operator= (TCopyPtr t) {
            t.Swap(*this);

            return *this;
        }

        inline T* Release() throw () {
            return DoRelease(T_);
        }

        inline void Reset(T* t) throw () {
            if (T_ != t) {
                DoDestroy();
                T_ = t;
            }
        }

        inline void Destroy() throw () {
            Reset(0);
        }

        inline void Swap(TCopyPtr& r) throw () {
            DoSwap(T_, r.T_);
        }

        inline T* Get() const throw () {
            return T_;
        }

    private:
        inline void DoDestroy() throw () {
            if (T_)
                D::Destroy(T_);
        }

    private:
        T* T_;
};

/*
 * now we define TArrayHolder/TArrayPtr && TMallocHolder/TMallocPtr
 * as template typedefs, but c++ does not have template typedef,
 * so this shit with macros'es
 */
#define DEFINE_CUSTOM_HOLDER(a, b)\
    template <class T>\
    class a: public THolder<T, b> {\
        public:\
            inline a(T* t = 0) throw ()\
                : THolder<T, b>(t)\
            {\
            }\
            \
            inline a(TAutoPtr<T, b> t) throw ()\
                : THolder<T, b>(t)\
            {\
            }\
    };

#define DEFINE_CUSTOM_AUTOPTR(a, b)\
    template <class T>\
    class a: public TAutoPtr<T, b> {\
        public:\
            inline a(T* t = 0) throw ()\
                : TAutoPtr<T, b>(t)\
            {\
            }\
    };

DEFINE_CUSTOM_HOLDER(TArrayHolder, TDeleteArray)
DEFINE_CUSTOM_HOLDER(TMallocHolder, TFree)

DEFINE_CUSTOM_AUTOPTR(TArrayPtr, TDeleteArray)
DEFINE_CUSTOM_AUTOPTR(TMallocPtr, TFree)

#undef DEFINE_CUSTOM_HOLDER
#undef DEFINE_CUSTOM_AUTOPTR
