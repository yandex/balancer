#include <new>

#include <cstdlib>
#include <cstring>

#include <util/system/platform.h>

#if defined(_linux_)
    #include <malloc.h>
#endif

/*
 * TODO
 */
#if defined(USE_INTELCC) || defined(_darwin_) || defined(_freebsd_) || defined(_STLPORT_VERSION)
  #define OP_THROWNOTHING throw ()
  #define OP_THROWBADALLOC throw (std::bad_alloc)
#else
  #define OP_THROWNOTHING
  #define OP_THROWBADALLOC
#endif

void* operator new(size_t size) OP_THROWBADALLOC {
    return malloc(size);
}

void* operator new(size_t size, const std::nothrow_t&) OP_THROWNOTHING {
    return malloc(size);
}

void operator delete(void* p) OP_THROWNOTHING {
    free(p);
}

void operator delete(void* p, const std::nothrow_t&) OP_THROWNOTHING {
    free(p);
}

void* operator new[](size_t size) OP_THROWBADALLOC {
    return malloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) OP_THROWNOTHING {
    return malloc(size);
}

void operator delete[](void* p) OP_THROWNOTHING {
    free(p);
}

void operator delete[](void* p, const std::nothrow_t&) OP_THROWNOTHING {
    free(p);
}

extern "C" void* memalign(size_t align, size_t size) {
    void* result;

    if (posix_memalign(&result, align, size) == 0) {
        return result;
    }

    return 0;
}

static const size_t pagesize = 8192;

extern "C" void* valloc(size_t size) {
    return memalign(pagesize, size);
}

extern "C" void* pvalloc(size_t size) {
  size = (size + pagesize - 1) & ~(pagesize - 1);

  return memalign(pagesize, size);
}

extern "C" void malloc_stats(void) {
}

extern "C" int mallopt(int /*cmd*/, int /*value*/) {
    return 1;
}

#if defined(_linux_)
extern "C" struct mallinfo mallinfo(void) {
    struct mallinfo info;
    memset(&info, 0, sizeof(info));

    return info;
}
#endif

#if defined(_darwin_)
extern "C" struct mstats mstats() {
	struct mstats info;
	memset(&info, 0, sizeof(info));

	return info;
}
#endif

extern "C" {
  void* __libc_malloc(size_t size)              { return malloc(size);       }
  void  __libc_free(void* ptr)                  { free(ptr);                 }
  void* __libc_realloc(void* ptr, size_t size)  { return realloc(ptr, size); }
  void* __libc_calloc(size_t n, size_t size)    { return calloc(n, size);    }
  void  __libc_cfree(void* ptr)                 { cfree(ptr);                }
  void* __libc_memalign(size_t align, size_t s) { return memalign(align, s); }
  void* __libc_valloc(size_t size)              { return valloc(size);       }
  void* __libc_pvalloc(size_t size)             { return pvalloc(size);      }
  int __posix_memalign(void** r, size_t a, size_t s) {
    return posix_memalign(r, a, s);
  }
}

// Override __libc_memalign in libc on linux boxes specially.
// They have a bug in libc that causes them to (very rarely) allocate
// with __libc_memalign() yet deallocate with free() and the
// definitions above don't catch it.
// This function is an exception to the rule of calling MallocHook method
// from the stack frame of the allocation function;
// heap-checker handles this special case explicitly.
static void *MemalignOverride(size_t align, size_t size, const void* /*caller*/) {
  return memalign(align, size);
}

void *(*__memalign_hook)(size_t, size_t, const void *) = MemalignOverride;
