#pragma once

#include <util/system/defaults.h>
#include <util/generic/utility.h>
#include <util/generic/yexception.h>

typedef bool TRbTreeColorType;

#define RBTreeRed false
#define RBTreeBlack true

struct TRbTreeNodeBase {
    typedef TRbTreeColorType TColorType;
    typedef TRbTreeNodeBase* TBasePtr;

    TColorType Color_;
    TBasePtr Parent_;
    TBasePtr Left_;
    TBasePtr Right_;
    size_t Children_;

    inline TRbTreeNodeBase() throw () {
        ReInitNode();
    }

    inline void ReInitNode() throw () {
        Color_ = RBTreeBlack;
        Parent_ = 0;
        Left_ = 0;
        Right_ = 0;
        Children_ = 1;
    }

    static TBasePtr MinimumNode(TBasePtr x) {
        while (x->Left_ != 0)
            x = x->Left_;

        return x;
    }

    static TBasePtr MaximumNode(TBasePtr x) {
        while (x->Right_ != 0)
            x = x->Right_;

        return x;
    }

    static TBasePtr ByIndex(TBasePtr x, size_t index) {
        if (x->Left_ != 0) {
            if (index < x->Left_->Children_)
                return ByIndex(x->Left_, index);
            index -= x->Left_->Children_;
        }
        if (0 == index)
            return x;
        if (!x->Right_)
            ythrow yexception() << "index not found";
        return ByIndex(x->Right_, index - 1);
    }
};

struct TRbTreeBaseIterator;

template <class TDummy>
class TRbGlobal {
    public:
        typedef TRbTreeNodeBase* TBasePtr;

        static void Rebalance(TBasePtr x, TBasePtr& root);
        static TBasePtr RebalanceForErase(TBasePtr z, TBasePtr& root, TBasePtr& leftmost, TBasePtr& rightmost);

        static TBasePtr IncrementNode(TBasePtr);
        static TBasePtr DecrementNode(TBasePtr);
        static void RotateLeft(TBasePtr x, TBasePtr& root);
        static void RotateRight(TBasePtr x, TBasePtr& root);
};

typedef TRbGlobal<bool> TRbGlobalInst;

struct TRbTreeBaseIterator {
    typedef TRbTreeNodeBase* TBasePtr;
    TBasePtr Node_;

    inline TRbTreeBaseIterator(TBasePtr x = 0) throw ()
        : Node_(x)
    {
    }
};

template <class TValue, class TTraits>
struct TRbTreeIterator: public TRbTreeBaseIterator {
    typedef typename TTraits::TReference TReference;
    typedef typename TTraits::TPointer TPointer;
    typedef TRbTreeIterator<TValue, TTraits> TSelf;
    typedef TRbTreeNodeBase* TBasePtr;

    inline TRbTreeIterator() throw () {
    }

    template <class T1>
    inline TRbTreeIterator(const T1& x) throw ()
        : TRbTreeBaseIterator(x)
    {
    }

    inline TReference operator*() const throw () {
        return *static_cast<TValue*>(Node_);
    }

    inline TPointer operator->() const throw () {
        return static_cast<TValue*>(Node_);
    }

    inline TSelf& operator++() throw () {
        Node_ = TRbGlobalInst::IncrementNode(Node_);
        return *this;
    }

    inline TSelf operator++(int) throw () {
        TSelf tmp = *this;
        ++(*this);
        return tmp;
    }

    inline TSelf& operator--() throw () {
        Node_ = TRbGlobalInst::DecrementNode(Node_);
        return *this;
    }

    inline TSelf operator--(int) throw () {
        TSelf tmp = *this;
        --(*this);
        return tmp;
    }

    template <class T1>
    inline bool operator== (const T1& rhs) const throw () {
        return Node_ == rhs.Node_;
    }

    template <class T1>
    inline bool operator!= (const T1& rhs) const throw () {
        return Node_ != rhs.Node_;
    }
};

template <class TValue, class TCmp>
class TRbTree {
        struct TCmpAdaptor: public TCmp {
            inline TCmpAdaptor() throw () {
            }

            inline TCmpAdaptor(const TCmp& cmp) throw ()
                : TCmp(cmp)
            {
            }

            template <class T1, class T2>
            inline bool operator() (const T1& l, const T2& r) const {
                return TCmp::Compare(l, r);
            }
        };

        struct TNonConstTraits {
            typedef TValue& TReference;
            typedef TValue* TPointer;
        };

        struct TConstTraits {
            typedef const TValue& TReference;
            typedef const TValue* TPointer;
        };

        typedef TRbTreeNodeBase TNodeBase;
        typedef TRbTreeNodeBase* TBasePtr;
        typedef TRbTreeColorType TColorType;

    public:
        class TRealNode: public TNodeBase {
            public:
                inline TRealNode()
                    : Tree_(0)
                {
                }

                inline ~TRealNode() throw () {
                    UnLink();
                }

                inline void UnLink() throw () {
                    if (Tree_) {
                        Tree_->EraseImpl(this);
                        ReInitNode();
                        Tree_ = 0;
                    }
                }

                inline void SetRbTreeParent(TRbTree* parent) throw () {
                    Tree_ = parent;
                }

            private:
                TRbTree* Tree_;
        };

        typedef TRbTreeIterator<TValue, TNonConstTraits> TIterator;
        typedef TRbTreeIterator<TValue, TConstTraits> TConstIterator;

        inline TRbTree() throw () {
            Init();
        }

        inline TRbTree(const TCmp& cmp) throw ()
            : KeyCompare_(cmp)
        {
            Init();
        }

        inline void Init() throw () {
            Data_.Color_ = RBTreeRed;
            Data_.Parent_ = 0;
            Data_.Left_ = &Data_;
            Data_.Right_ = &Data_;
            Data_.Children_ = 0;
        }

        struct TDestroy {
            inline void operator() (TValue& v) const throw () {
                v.SetRbTreeParent(0);
                v.ReInitNode();
            }
        };

        inline ~TRbTree() throw () {
            ForEachNoOrder(TDestroy());
        }

        inline void Clear() throw () {
            ForEachNoOrder(TDestroy());
            Init();
        }

        template <class F>
        inline void ForEachNoOrder(const F& f) {
            ForEachNoOrder(Root(), f);
        }

        template <class F>
        inline void ForEachNoOrder(TNodeBase* n, const F& f) {
            if (n && n != &Data_) {
                ForEachNoOrder(n->Left_, f);
                ForEachNoOrder(n->Right_, f);
                f(ValueNode(n));
            }
        }

        inline TIterator Begin() throw () {
            return LeftMost();
        }

        inline TConstIterator Begin() const throw () {
            return LeftMost();
        }

        inline TIterator End() throw () {
            return &this->Data_;
        }

        inline TConstIterator End() const throw () {
            return const_cast<TBasePtr>(&this->Data_);
        }

        inline bool Empty() const throw () {
            return this->Begin() == this->End();
        }

        inline TIterator Insert(TValue* val) {
            return Insert(*val);
        }

        inline TIterator Insert(TValue& val) {
            val.UnLink();

            TBasePtr y = &this->Data_;
            TBasePtr x = Root();

            while (x != 0) {
                ++(x->Children_);
                y = x;

                if (KeyCompare_(ValueNode(&val), ValueNode(x))) {
                    x = LeftNode(x);
                } else {
                    x = RightNode(x);
                }
            }

            return InsertImpl(y, &val, x);
        }

        template <class F>
        inline void ForEach(F& f) {
            TIterator it = Begin();

            while (it != End()) {
                f(*it++);
            }
        }

        inline void Erase(TValue& val) throw () {
            val.UnLink();
        }

        inline void Erase(TValue* val) throw () {
            Erase(*val);
        }

        inline void Erase(TIterator pos) throw () {
            Erase(*pos);
        }

        inline void EraseImpl(TNodeBase* val) throw () {
            TRbGlobalInst::RebalanceForErase(val, this->Data_.Parent_, this->Data_.Left_, this->Data_.Right_);
        }

        template <class T1>
        inline TValue* Find(const T1& k) const {
            TBasePtr y = 0;
            TBasePtr x = Root();      // Current node.

            while (x != 0)
                if (!KeyCompare_(ValueNode(x), k))
                    y = x, x = LeftNode(x);
                else
                    x = RightNode(x);

            if (y) {
                if (KeyCompare_(k, ValueNode(y))) {
                    y = 0;
                }
            }

            return static_cast<TValue*>(y);
        }

        template <class T1>
        inline TBasePtr LowerBound(const T1& k) const {
            TBasePtr y = const_cast<TBasePtr>(&this->Data_); /* Last node which is not less than k. */
            TBasePtr x = Root(); /* Current node. */

            while (x != 0)
                if (!KeyCompare_(ValueNode(x), k))
                    y = x, x = LeftNode(x);
                else
                    x = RightNode(x);

            return y;
        }

        template <class T1>
        inline TBasePtr UpperBound(const T1& k) const {
            TBasePtr y = const_cast<TBasePtr>(&this->Data_); /* Last node which is greater than k. */
            TBasePtr x = Root(); /* Current node. */

            while (x != 0)
                if (KeyCompare_(k, ValueNode(x)))
                    y = x, x = LeftNode(x);
                else
                    x = RightNode(x);

            return y;
        }

        TValue* ByIndex(size_t index) {
            return static_cast<TValue*>(TRbTreeNodeBase::ByIndex(Root(), index));
        }

    private:
        // CRP 7/10/00 inserted argument on_right, which is another hint (meant to
        // act like on_left and ignore a portion of the if conditions -- specify
        // on_right != 0 to bypass comparison as false or on_left != 0 to bypass
        // comparison as true)
        TIterator InsertImpl(TRbTreeNodeBase* parent, TRbTreeNodeBase* val, TRbTreeNodeBase* on_left = 0, TRbTreeNodeBase* on_right = 0) {
            ValueNode(val).SetRbTreeParent(this);
            TBasePtr new_node = val;

            if (parent == &this->Data_) {
                LeftNode(parent) = new_node;
                // also makes LeftMost() = new_node
                Root() = new_node;
                RightMost() = new_node;
            } else if (on_right == 0 &&
                // If on_right != 0, the remainder fails to false
                (on_left != 0 ||
                    // If on_left != 0, the remainder succeeds to true
                    KeyCompare_(ValueNode(val), ValueNode(parent)))) {
                LeftNode(parent) = new_node;
                if (parent == LeftMost())
                    // maintain LeftMost() pointing to min node
                    LeftMost() = new_node;
            } else {
                RightNode(parent) = new_node;
                if (parent == RightMost())
                    // maintain RightMost() pointing to max node
                    RightMost() = new_node;

            }
            ParentNode(new_node) = parent;
            TRbGlobalInst::Rebalance(new_node, this->Data_.Parent_);
            return new_node;
        }

        TBasePtr Root() const {
            return this->Data_.Parent_;
        }

        TBasePtr LeftMost() const {
            return this->Data_.Left_;
        }

        TBasePtr RightMost() const {
            return this->Data_.Right_;
        }

        TBasePtr& Root() {
            return this->Data_.Parent_;
        }

        TBasePtr& LeftMost() {
            return this->Data_.Left_;
        }

        TBasePtr& RightMost() {
            return this->Data_.Right_;
        }

        static TBasePtr& LeftNode(TBasePtr x) {
            return x->Left_;
        }

        static TBasePtr& RightNode(TBasePtr x) {
            return x->Right_;
        }

        static TBasePtr& ParentNode(TBasePtr x) {
            return x->Parent_;
        }

        static TValue& ValueNode(TBasePtr x) {
            return *static_cast<TValue*>(x);
        }

        static TBasePtr MinimumNode(TBasePtr x) {
            return TRbTreeNodeBase::MinimumNode(x);
        }

        static TBasePtr MaximumNode(TBasePtr x) {
            return TRbTreeNodeBase::MaximumNode(x);
        }

    private:
        TCmpAdaptor KeyCompare_;
        TNodeBase Data_;
};

template <class TValue, class TCmp>
class TRbTreeItem: public TRbTree<TValue, TCmp>::TRealNode {
};

template <class TDummy>
void TRbGlobal<TDummy>::RotateLeft(TRbTreeNodeBase* x, TRbTreeNodeBase*& root) {
    TRbTreeNodeBase* y = x->Right_;
    x->Right_ = y->Left_;
    if (y->Left_ != 0)
        y->Left_->Parent_ = x;
    y->Parent_ = x->Parent_;

    if (x == root)
        root = y;
    else if (x == x->Parent_->Left_)
        x->Parent_->Left_ = y;
    else
        x->Parent_->Right_ = y;
    y->Left_ = x;
    x->Parent_ = y;
    y->Children_ = x->Children_;
    x->Children_ = ((x->Left_) ? x->Left_->Children_ : 0) + ((x->Right_) ? x->Right_->Children_ : 0) + 1;
}

template <class TDummy>
void TRbGlobal<TDummy>::RotateRight(TRbTreeNodeBase* x, TRbTreeNodeBase*& root) {
    TRbTreeNodeBase* y = x->Left_;
    x->Left_ = y->Right_;
    if (y->Right_ != 0)
        y->Right_->Parent_ = x;
    y->Parent_ = x->Parent_;

    if (x == root)
        root = y;
    else if (x == x->Parent_->Right_)
        x->Parent_->Right_ = y;
    else
        x->Parent_->Left_ = y;
    y->Right_ = x;
    x->Parent_ = y;
    y->Children_ = x->Children_;
    x->Children_ = ((x->Left_) ? x->Left_->Children_ : 0) + ((x->Right_) ? x->Right_->Children_ : 0) + 1;
}

template <class TDummy>
void TRbGlobal<TDummy>::Rebalance(TRbTreeNodeBase* x, TRbTreeNodeBase*& root) {
    x->Color_ = RBTreeRed;
    while (x != root && x->Parent_->Color_ == RBTreeRed) {
        if (x->Parent_ == x->Parent_->Parent_->Left_) {
            TRbTreeNodeBase* y = x->Parent_->Parent_->Right_;
            if (y && y->Color_ == RBTreeRed) {
                x->Parent_->Color_ = RBTreeBlack;
                y->Color_ = RBTreeBlack;
                x->Parent_->Parent_->Color_ = RBTreeRed;
                x = x->Parent_->Parent_;
            } else {
                if (x == x->Parent_->Right_) {
                    x = x->Parent_;
                    RotateLeft(x, root);
                }
                x->Parent_->Color_ = RBTreeBlack;
                x->Parent_->Parent_->Color_ = RBTreeRed;
                RotateRight(x->Parent_->Parent_, root);
            }
        } else {
            TRbTreeNodeBase* y = x->Parent_->Parent_->Left_;
            if (y && y->Color_ == RBTreeRed) {
                x->Parent_->Color_ = RBTreeBlack;
                y->Color_ = RBTreeBlack;
                x->Parent_->Parent_->Color_ = RBTreeRed;
                x = x->Parent_->Parent_;
            } else {
                if (x == x->Parent_->Left_) {
                    x = x->Parent_;
                    RotateRight(x, root);
                }
                x->Parent_->Color_ = RBTreeBlack;
                x->Parent_->Parent_->Color_ = RBTreeRed;
                RotateLeft(x->Parent_->Parent_, root);
            }
        }
    }
    root->Color_ = RBTreeBlack;
}

template <class TDummy>
TRbTreeNodeBase* TRbGlobal<TDummy>::RebalanceForErase(TRbTreeNodeBase* z,
                                         TRbTreeNodeBase*& root,
                                         TRbTreeNodeBase*& leftmost,
                                         TRbTreeNodeBase*& rightmost) {
    TRbTreeNodeBase* y = z;
    TRbTreeNodeBase* x;
    TRbTreeNodeBase* x_parent;

    if (y->Left_ == 0)     // z has at most one non-null child. y == z.
        x = y->Right_;     // x might be null.
    else {
        if (y->Right_ == 0)  // z has exactly one non-null child. y == z.
            x = y->Left_;    // x is not null.
        else {                   // z has two non-null children.  Set y to
            y = TRbTreeNodeBase::MinimumNode(y->Right_);   //   z's successor.  x might be null.
            x = y->Right_;
        }
    }

    if (y != z) {
        // relink y in place of z.  y is z's successor
        z->Left_->Parent_ = y;
        y->Left_ = z->Left_;
        if (y != z->Right_) {
            x_parent = y->Parent_;
            if (x)
                x->Parent_ = y->Parent_;
            y->Parent_->Left_ = x;      // y must be a child of mLeft
            y->Right_ = z->Right_;
            z->Right_->Parent_ = y;
        } else
            x_parent = y;
        if (root == z)
            root = y;
        else if (z->Parent_->Left_ == z)
            z->Parent_->Left_ = y;
        else
            z->Parent_->Right_ = y;
        y->Parent_ = z->Parent_;
        DoSwap(y->Color_, z->Color_);
        y = z;
        // y now points to node to be actually deleted
    } else {
        // y == z
        x_parent = y->Parent_;
        if (x)
            x->Parent_ = y->Parent_;
        if (root == z)
            root = x;
        else {
            if (z->Parent_->Left_ == z)
                z->Parent_->Left_ = x;
            else
                z->Parent_->Right_ = x;
        }

        if (leftmost == z) {
            if (z->Right_ == 0)        // z->mLeft must be null also
                leftmost = z->Parent_;
            // makes leftmost == _M_header if z == root
            else
                leftmost = TRbTreeNodeBase::MinimumNode(x);
        }
        if (rightmost == z) {
            if (z->Left_ == 0)         // z->mRight must be null also
                rightmost = z->Parent_;
            // makes rightmost == _M_header if z == root
            else                      // x == z->mLeft
                rightmost = TRbTreeNodeBase::MaximumNode(x);
        }
    }

    if (y->Color_ != RBTreeRed) {
        while (x != root && (x == 0 || x->Color_ == RBTreeBlack))
            if (x == x_parent->Left_) {
                TRbTreeNodeBase* w = x_parent->Right_;
                if (w->Color_ == RBTreeRed) {
                    w->Color_ = RBTreeBlack;
                    x_parent->Color_ = RBTreeRed;
                    RotateLeft(x_parent, root);
                    w = x_parent->Right_;
                }
                if ((w->Left_ == 0 ||
                        w->Left_->Color_ == RBTreeBlack) && (w->Right_ == 0 ||
                            w->Right_->Color_ == RBTreeBlack)) {
                    w->Color_ = RBTreeRed;
                    x = x_parent;
                    x_parent = x_parent->Parent_;
                } else {
                    if (w->Right_ == 0 ||
                        w->Right_->Color_ == RBTreeBlack) {
                        if (w->Left_)
                            w->Left_->Color_ = RBTreeBlack;
                        w->Color_ = RBTreeRed;
                        RotateRight(w, root);
                        w = x_parent->Right_;
                    }
                    w->Color_ = x_parent->Color_;
                    x_parent->Color_ = RBTreeBlack;
                    if (w->Right_)
                        w->Right_->Color_ = RBTreeBlack;
                    RotateLeft(x_parent, root);
                    break;
                }
            } else {
                // same as above, with mRight <-> mLeft.
                TRbTreeNodeBase* w = x_parent->Left_;
                if (w->Color_ == RBTreeRed) {
                    w->Color_ = RBTreeBlack;
                    x_parent->Color_ = RBTreeRed;
                    RotateRight(x_parent, root);
                    w = x_parent->Left_;
                }
                if ((w->Right_ == 0 ||
                        w->Right_->Color_ == RBTreeBlack) && (w->Left_ == 0 ||
                            w->Left_->Color_ == RBTreeBlack)) {
                    w->Color_ = RBTreeRed;
                    x = x_parent;
                    x_parent = x_parent->Parent_;
                } else {
                    if (w->Left_ == 0 ||
                        w->Left_->Color_ == RBTreeBlack) {
                        if (w->Right_)
                            w->Right_->Color_ = RBTreeBlack;
                        w->Color_ = RBTreeRed;
                        RotateLeft(w, root);
                        w = x_parent->Left_;
                    }
                    w->Color_ = x_parent->Color_;
                    x_parent->Color_ = RBTreeBlack;
                    if (w->Left_)
                        w->Left_->Color_ = RBTreeBlack;
                    RotateRight(x_parent, root);
                    break;
                }
            }
        if (x)
            x->Color_ = RBTreeBlack;
    }
    return y;
}

template <class TDummy>
TRbTreeNodeBase* TRbGlobal<TDummy>::DecrementNode(TRbTreeNodeBase* Node_) {
    if (Node_->Color_ == RBTreeRed && Node_->Parent_->Parent_ == Node_)
        Node_ = Node_->Right_;
    else if (Node_->Left_ != 0) {
        Node_ = TRbTreeNodeBase::MaximumNode(Node_->Left_);
    } else {
        TBasePtr y = Node_->Parent_;
        while (Node_ == y->Left_) {
            Node_ = y;
            y = y->Parent_;
        }
        Node_ = y;
    }
    return Node_;
}

template <class TDummy>
TRbTreeNodeBase* TRbGlobal<TDummy>::IncrementNode(TRbTreeNodeBase* Node_) {
    if (Node_->Right_ != 0) {
        Node_ = TRbTreeNodeBase::MinimumNode(Node_->Right_);
    } else {
        TBasePtr y = Node_->Parent_;
        while (Node_ == y->Right_) {
            Node_ = y;
            y = y->Parent_;
        }
        // check special case: This is necessary if mNode is the
        // _M_head and the tree contains only a single node y. In
        // that case parent, left and right all point to y!
        if (Node_->Right_ != y)
            Node_ = y;
    }
    return Node_;
}

#undef RBTreeRed
#undef RBTreeBlack
