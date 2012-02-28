#pragma once

class TContStackAllocator {
    public:
        class TStack {
            public:
                inline TStack() throw () {
                }

                virtual ~TStack() throw () {
                }

                virtual void Release() throw () {
                    delete this;
                }

                virtual void* Data() throw () = 0;
                virtual size_t Length() const throw () = 0;

                inline void RegisterStackInValgrind() throw () {
#if defined(WITH_VALGRIND)
                    StackId_ = VALGRIND_STACK_REGISTER(Data(), (char*)Data() + Length());
#endif
                }

                inline void UnRegisterStackInValgrind() throw () {
#if defined(WITH_VALGRIND)
                    VALGRIND_STACK_DEREGISTER(StackId_);
#endif
                }

            private:
#if defined(WITH_VALGRIND)
                size_t StackId_;
#endif
        };

        struct TRelease {
            static inline void Destroy(TStack* s) throw () {
                s->UnRegisterStackInValgrind();
                s->Release();
            }
        };

        typedef TAutoPtr<TStack, TRelease> TStackPtr;

        inline TContStackAllocator() throw () {
        }

        virtual ~TContStackAllocator() throw () {
        }

        virtual TStackPtr Allocate() {
            TStackPtr ret(DoAllocate());

            ret->RegisterStackInValgrind();

            return ret;
        }

    private:
        virtual TStack* DoAllocate() = 0;
};

class TGenericContStackAllocatorBase: public TContStackAllocator {
        typedef IAllocator::TBlock TBlock;

        class TGenericStack: public TStack {
            public:
                inline TGenericStack(TGenericContStackAllocatorBase* parent, const TBlock& block) throw ()
                    : Parent_(parent)
                    , Block_(block)
                {
                }

                virtual ~TGenericStack() throw () {
                }

                virtual void Release() throw () {
                    TGenericContStackAllocatorBase* parent(Parent_);
                    const TBlock block(Block_);
                    this->~TGenericStack();
                    parent->Alloc_->Release(block);
                }

                virtual void* Data() throw () {
                    return this + 1;
                }

                virtual size_t Length() const throw () {
                    return Block_.Len - sizeof(*this);
                }

            private:
                TGenericContStackAllocatorBase* Parent_;
                const TBlock Block_;
        };

    public:
        inline TGenericContStackAllocatorBase(IAllocator* alloc, size_t len) throw ()
            : Alloc_(alloc)
            , Len_(len)
        {
        }

        virtual ~TGenericContStackAllocatorBase() throw () {
        }

        virtual TStack* DoAllocate() {
            TBlock block = Alloc_->Allocate(Len_ + sizeof(TGenericStack));

            return new (block.Data) TGenericStack(this, block);
        }

    private:
        IAllocator* Alloc_;
        const size_t Len_;
};

class TProtectedContStackAllocator: public TContStackAllocator {
        static inline size_t PageSize() throw () {
            return 4096;
        }

        static void Protect(void* ptr, size_t len) throw ();
        static void UnProtect(void* ptr, size_t len) throw ();

        class TProtectedStack: public TStack {
            public:
                inline TProtectedStack(TStack* slave) throw ()
                    : Slave_(slave)
                {
                    YASSERT(Length() % PageSize() == 0);

                    Protect((char*)AlignedData(), PageSize());
                    Protect((char*)Data() + Length(), PageSize());
                }

                virtual ~TProtectedStack() throw () {
                    UnProtect((char*)AlignedData(), PageSize());
                    UnProtect((char*)Data() + Length(), PageSize());

                    Slave_->Release();
                }

                virtual void Release() throw () {
                    delete this;
                }

                inline void* AlignedData() throw () {
                    return AlignUp(Slave_->Data(), PageSize());
                }

                virtual void* Data() throw () {
                    return (char*)AlignedData() + PageSize();
                }

                virtual size_t Length() const throw () {
                    return Slave_->Length() - 3 * PageSize();
                }

            private:
                TStack* Slave_;
        };

    public:
        inline TProtectedContStackAllocator(IAllocator* alloc, size_t len) throw ()
            : Alloc_(alloc, AlignUp(len, PageSize()) + 3 * PageSize())
        {
        }

        virtual ~TProtectedContStackAllocator() throw () {
        }

        virtual TStack* DoAllocate() {
            return new TProtectedStack(Alloc_.DoAllocate());
        }

    private:
        TGenericContStackAllocatorBase Alloc_;
};

#if defined(NDEBUG)
    typedef TGenericContStackAllocatorBase TGenericContStackAllocator;
#else
    typedef TProtectedContStackAllocator TGenericContStackAllocator;
#endif

class THeapStackAllocator: public TGenericContStackAllocator {
    public:
        inline THeapStackAllocator(size_t len)
            : TGenericContStackAllocator(TDefaultAllocator::Instance(), len)
        {
        }
};

typedef THeapStackAllocator TDefaultStackAllocator;
