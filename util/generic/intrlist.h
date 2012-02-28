#pragma once

#include <util/system/yassert.h>

#include "utility.h"
#include "noncopyable.h"

class TSizeCount {
    public:
        inline TSizeCount() throw ()
            : Size_(0)
        {
        }

        template <class T>
        inline void operator() (const T&) throw () {
            ++Size_;
        }

        inline size_t Size() const throw () {
            return Size_;
        }

    private:
        size_t Size_;
};

/*
 * two-way linked list
 */
template <class T>
class TIntrusiveListItem: public TNonCopyable {
        typedef TIntrusiveListItem<T> TListItem;
    public:
        inline TIntrusiveListItem() throw ()
            : Next_(this)
            , Prev_(Next_)
        {
        }

        inline ~TIntrusiveListItem() throw () {
            Unlink();
        }

        inline bool Empty() const throw () {
            return (Prev_ == this) && (Next_ == this);
        }

        inline void Unlink() throw () {
            if (Empty()) {
                return;
            }

            Prev_->SetNext(Next_);
            Next_->SetPrev(Prev_);

            SetEnd();
        }

        inline void LinkBefore(TListItem* before) throw () {
            Unlink();
            LinkBeforeNoUnlink(before);
        }

        inline void LinkBeforeNoUnlink(TListItem* before) throw () {
            TListItem* after = before->Prev();

            after->SetNext(this);
            SetPrev(after);
            SetNext(before);
            before->SetPrev(this);
        }

        inline void LinkBefore(TListItem& before) throw () {
            LinkBefore(&before);
        }

        inline void LinkAfter(TListItem* after) throw () {
            Unlink();
            LinkBeforeNoUnlink(after->Next());
        }

        inline void LinkAfter(TListItem& after) throw () {
            LinkAfter(&after);
        }

        inline TListItem* Prev() throw () {
            return Prev_;
        }

        inline const TListItem* Prev() const throw () {
            return Prev_;
        }

        inline TListItem* Next() throw () {
            return Next_;
        }

        inline const TListItem* Next() const throw () {
            return Next_;
        }

        inline T* Node() throw () {
            return static_cast<T*>(this);
        }

        inline const T* Node() const throw () {
            return static_cast<const T*>(this);
        }

        template <class Functor>
        inline void ForEach(Functor& f) {
            TListItem* c = Next();

            while (c != this) {
                TListItem* n = c->Next();
                f(c->Node());
                c = n;
            }
        }

        template <class Functor>
        inline void ForEach(const Functor& f) {
            TListItem* c = Next();

            while (c != this) {
                TListItem* n = c->Next();
                f(c->Node());
                c = n;
            }
        }

        inline void SetEnd() throw () {
            Prev_ = this;
            Next_ = Prev_;
        }

        inline void SetNext(TListItem* t) throw () {
            Next_ = t;
        }

        inline void SetPrev(TListItem* t) throw () {
            Prev_ = t;
        }

    private:
        TListItem* Next_;
        TListItem* Prev_;
};

template <class T>
class TIntrusiveList: public TNonCopyable {
        typedef TIntrusiveListItem<T> TListItem;

        template <class TLI, class TT>
        class TBaseIterator {
        public:
            inline TBaseIterator() throw ()
                : Item_(0)
            {
            }

            inline TBaseIterator(TLI* item) throw ()
                : Item_(item)
            {
            }

            template <class X>
            inline TBaseIterator(const X& it) throw ()
                : Item_(it.Item())
            {
            }

            inline TLI* Item() const throw () {
                return Item_;
            }

            inline void Next() throw () {
                Item_ = Item_->Next();
            }

            inline void Prev() throw () {
                Item_ = Item_->Prev();
            }

            template <class X>
            inline bool operator== (const X& r) const throw () {
                return Item() == r.Item();
            }

            template <class X>
            inline bool operator!= (const X& r) const throw () {
                return Item() != r.Item();
            }

            inline TBaseIterator& operator++ () throw () {
                Next();

                return *this;
            }

            inline TBaseIterator operator++ (int) throw () {
                TBaseIterator ret(*this);

                Next();

                return ret;
            }

            inline TBaseIterator& operator-- () throw () {
                Prev();

                return *this;
            }

            inline TBaseIterator operator-- (int) throw () {
                TBaseIterator ret(*this);

                Prev();

                return ret;
            }

            inline TT& operator* () const throw () {
                return *Item_->Node();
            }

            inline TT* operator-> () const throw () {
                return Item_->Node();
            }

        private:
            TLI* Item_;
        };

        template <class It>
        class TReverseIteratorBase: public It {
        public:
            inline TReverseIteratorBase() throw () {
            }

            inline TReverseIteratorBase(const It& i) throw ()
                : It(i)
            {
            }

            inline TReverseIteratorBase& operator++ () throw () {
                Base()--;

                return *this;
            }

            inline TReverseIteratorBase operator++ (int) throw () {
                return --Base();
            }

            inline TReverseIteratorBase& operator-- () throw () {
                Base()++;

                return *this;
            }

            inline TReverseIteratorBase operator-- (int) throw () {
                return ++Base();
            }

            inline It& Base() throw () {
                return *this;
            }
        };

    public:
        typedef TBaseIterator<TListItem, T> TIterator;
        typedef TBaseIterator<const TListItem, const T> TConstIterator;

        typedef TReverseIteratorBase<TIterator> TReverseIterator;
        typedef TReverseIteratorBase<TConstIterator> TConstReverseIterator;

        //compat
        typedef TIterator iterator;
        typedef TConstIterator const_iterator;

        inline TIterator Begin() throw () {
            return ++End();
        }

        inline TIterator End() throw () {
            return TIterator(&End_);
        }

        inline TConstIterator Begin() const throw () {
            return ++End();
        }

        inline TConstIterator End() const throw () {
            return TConstIterator(&End_);
        }

        inline TReverseIterator RBegin() throw () {
            return --End();
        }

        inline TReverseIterator REnd() throw () {
            return End();
        }

        inline TConstReverseIterator RBegin() const throw () {
            return --End();
        }

        inline TConstReverseIterator REnd() const throw () {
            return End();
        }

        inline void PushBack(TListItem* item) throw () {
            item->LinkBefore(End_);
        }

        inline void PushFront(TListItem* item) throw () {
            item->LinkAfter(End_);
        }

        inline T* PopBack() throw () {
            TListItem* ret = End_.Prev();

            ret->Unlink();

            return ret->Node();
        }

        inline T* PopFront() throw () {
            TListItem* ret = End_.Next();

            ret->Unlink();

            return ret->Node();
        }

        template <class Functor>
        inline void ForEach(Functor& f) {
            TIterator it = Begin();

            while (it != End()) {
                f(&*(it++));
            }
        }

        template <class Functor>
        inline void ForEach(Functor& f) const {
            TConstIterator it = Begin();

            while (it != End()) {
                f(&*(it++));
            }
        }

        inline size_t Size() const throw () {
            TSizeCount f;

            ForEach(f);

            return f.Size();
        }

        inline bool Empty() const throw () {
            return End_.Empty();
        }

        inline void Swap(TIntrusiveList& lst) throw () {
            TIntrusiveList tmp;

            tmp.Append(lst); //tmp <- lst, lst - empty
            YASSERT(lst.Empty());
            lst.Append(*this); //lst <- this, this - empty
            YASSERT(this->Empty());
            this->Append(tmp); //this <- tmp, tmp - empty
            YASSERT(tmp.Empty());
        }

        inline void Append(TIntrusiveList& lst) throw () {
            Cut(lst.Begin(), lst.End(), End());
        }

        inline static void Cut(TIterator begin, TIterator end, TIterator pasteBefore) throw () {
            if (begin == end) {
                return;
            }

            TListItem* cutFront = begin.Item();
            TListItem* gapBack = end.Item();

            TListItem* gapFront = cutFront->Prev();
            TListItem* cutBack = gapBack->Prev();

            gapFront->SetNext(gapBack);
            gapBack->SetPrev(gapFront);

            TListItem* pasteBack = pasteBefore.Item();
            TListItem* pasteFront = pasteBack->Prev();

            pasteFront->SetNext(cutFront);
            cutFront->SetPrev(pasteFront);

            cutBack->SetNext(pasteBack);
            pasteBack->SetPrev(cutBack);
        }

        template <class F>
        inline void QuickSort(const F& f) {
            if (Begin() == End() || ++Begin() == End()) {
                return;
            }

            T* c = PopFront();
            TIntrusiveList bigger;
            TIterator it = Begin();

            while (it != End()) {
                if (f(*c, *it)) {
                    bigger.PushBack(&*it++);
                } else {
                    ++it;
                }
            }

            this->QuickSort(f);
            bigger.QuickSort(f);

            PushBack(c);
            Append(bigger);
        }

        inline void Clear() throw () {
            End_.Unlink();
        }

    private:
        TListItem End_;
};

template <class T, class D>
class TIntrusiveListWithAutoDelete: public TIntrusiveList<T> {
    public:
        typedef typename TIntrusiveList<T>::TIterator TIterator;

        inline ~TIntrusiveListWithAutoDelete() throw () {
            this->Clear();
        }

        inline void Clear() throw () {
            while (!this->Empty()) {
                D::Destroy(this->PopFront());
            }
        }

        inline static void Cut(TIterator begin, TIterator end) throw () {
            TIntrusiveListWithAutoDelete<T, D> temp;
            Cut(begin, end, temp.End());
        }

        inline static void Cut(TIterator begin, TIterator end, TIterator pasteBefore) throw () {
            TIntrusiveList<T>::Cut(begin, end, pasteBefore);
        }
};

/*
 * one-way linked list
 */
template <class T>
class TIntrusiveSListItem {
        typedef TIntrusiveSListItem<T> TListItem;

    public:
        inline TIntrusiveSListItem() throw ()
            : Next_(0)
        {
        }

        inline ~TIntrusiveSListItem() throw () {
        }

        inline bool IsEnd() const throw () {
            return Next_ == 0;
        }

        inline TListItem* Next() throw () {
            return Next_;
        }

        inline const TListItem* Next() const throw () {
            return Next_;
        }

        inline void SetNext(TListItem* item) throw () {
            Next_ = item;
        }

        inline T* Node() throw () {
            return static_cast<T*>(this);
        }

        inline const T* Node() const throw () {
            return static_cast<const T*>(this);
        }

    private:
        TListItem* Next_;
};

template <class T>
class TIntrusiveSList {
        typedef TIntrusiveSListItem<T> TListItem;

    public:
        template <class T1, class T2>
        class TIteratorBase {
            public:
                inline TIteratorBase(T2* item) throw ()
                    : Item_(item)
                {
                }

                inline void Next() throw () {
                    Item_ = Item_->Next();
                }

                inline bool operator== (const TIteratorBase& r) const throw () {
                    return Item_ == r.Item_;
                }

                inline bool operator!= (const TIteratorBase& r) const throw () {
                    return Item_ != r.Item_;
                }

                inline TIteratorBase& operator++ () throw () {
                    Next();

                    return *this;
                }

                inline TIteratorBase operator++ (int) throw () {
                    TIteratorBase ret(*this);

                    Next();

                    return ret;
                }

                inline T1& operator* () throw () {
                    return *Item_->Node();
                }

                inline T1* operator-> () throw () {
                    return Item_->Node();
                }

            private:
                T2* Item_;
        };

        typedef TIteratorBase<T, TListItem> TIterator;
        typedef TIteratorBase<const T, const TListItem> TConstIterator;

        inline TIntrusiveSList() throw ()
            : Begin_(0)
        {
        }

        inline void Reverse() throw () {
            TIntrusiveSList l;

            while (!Empty()) {
                l.PushFront(PopFront());
            }

            l.Swap(*this);
        }

        inline void PushFront(TListItem* item) throw () {
            item->SetNext(Begin_);
            Begin_ = item;
        }

        inline T* PopFront() throw () {
            TListItem* ret = Begin_;
            Begin_ = Begin_->Next();

            return ret->Node();
        }

        inline bool Empty() const throw () {
            return Begin_ == 0;
        }

        template <class Functor>
        inline void ForEach(Functor& f) const {
            TListItem* it = Begin_;

            while (it) {
                TListItem* next = it->Next();
                f(it->Node());
                it = next;
            }
        }

        inline TIterator Begin() throw () {
            return TIterator(Begin_);
        }

        inline TIterator End() throw () {
            return TIterator(0);
        }

        inline TConstIterator Begin() const throw () {
            return TConstIterator(Begin_);
        }

        inline TConstIterator End() const throw () {
            return TConstIterator(0);
        }

        inline void Swap(TIntrusiveSList& l) throw () {
            DoSwap(Begin_, l.Begin_);
        }

        inline size_t Size() const throw () {
            TSizeCount f;

            ForEach(f);

            return f.Size();
        }

    private:
        TListItem* Begin_;
};
