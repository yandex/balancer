#include "sem.h"

#ifdef _win_
    #include <malloc.h>
#elif defined(_sun)
    #include <alloca.h>
#endif

#include <cerrno>
#include <cstring>

#ifdef _win_
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#else
    #include <signal.h>
    #include <unistd.h>

    #define USE_SYSV_SEMAPHORES //unixoids declared the standard but not implemented it...

    #ifdef USE_SYSV_SEMAPHORES
        #include <errno.h>
        #include <sys/types.h>
        #include <sys/ipc.h>
        #include <sys/sem.h>
    #else
        #include <semaphore.h>
    #endif
#endif

#ifdef USE_SYSV_SEMAPHORES
    #if defined(_linux_) || defined(_sun_) || defined(_cygwin_)
        union semun {
            int val;
            struct semid_ds *buf;
            unsigned short  *array;
        } arg;
    #else
        union semun arg;
    #endif
#endif

#include <util/digest/crc.h>

class TSemaphore::TImpl {
    private:
#ifdef _win_
        typedef HANDLE SEMHANDLE;
#else
    #ifdef USE_SYSV_SEMAPHORES
        typedef int SEMHANDLE;
    #else
        typedef sem_t* SEMHANDLE;
    #endif
#endif

        SEMHANDLE Handle;
        int Errno;

    public:
        inline TImpl(const char* name, ui32 max_free_count) {
            Handle = 0;
            Errno = 0;
#ifdef _win_
            char* key = (char*)name;
            if (name) {
                size_t len = strlen(name);
                key = (char*)alloca(len+1);
                strcpy(key, name);
                if (len > MAX_PATH)
                    *(key + MAX_PATH) = 0;
                char* p = key;
                while (*p) {
                    if (*p == '\\')
                        *p = '/';
                    p++;
                }
            }
            // non-blocking on init
            Handle = ::CreateSemaphore(0, max_free_count, max_free_count, key);
#else
    #ifdef USE_SYSV_SEMAPHORES
            key_t key = Crc<ui32>(name, strlen(name));
            Handle = semget(key, 0, 0); // try to open exist semaphore
            if (Handle == -1) { // create new semaphore
                Handle = semget(key, 1, 0666 | IPC_CREAT);
                if (Handle != -1) {
                    union semun arg;
                    arg.val = max_free_count;
                    semctl(Handle, 0, SETVAL, arg);
                } else
                    Errno = errno;
            }
    #else
            Handle = sem_open(name, O_CREAT, 0666, max_free_count);
            if (Handle == SEM_FAILED)
                Errno = errno;
    #endif
#endif
        }

        inline ~TImpl() throw () {
#ifdef _win_
            ::CloseHandle(Handle);
#else
    #ifdef USE_SYSV_SEMAPHORES
            // we DO NOT want 'semctl(Handle, 0, IPC_RMID)' for multiprocess tasks;
            //struct sembuf ops[] = {{0, 0, IPC_NOWAIT}};
            //if (semop(Handle, ops, 1) != 0) // close only if semaphore's value is zero
            //    semctl(Handle, 0, IPC_RMID);
    #else
            sem_close(Handle); // we DO NOT want sem_unlink(...)
            if (Handle == SEM_FAILED)
                Errno = errno;
    #endif
#endif
        }

        inline bool Release() {
#ifdef _win_
            return ::ReleaseSemaphore(Handle, 1, 0) ? true : false;
#else
    #ifdef USE_SYSV_SEMAPHORES
            struct sembuf ops[] = {{0, 1, SEM_UNDO}};
            int ret = semop(Handle, ops, 1);
    #else
            int ret = sem_post(Handle);
    #endif
            if (ret != 0)
                Errno = errno;
            return ret == 0;
#endif
        }

        //The UNIX semaphore object does not support a timed "wait", and
        //hence to maintain consistancy, for win32 case we use INFINITE or 0 timeout.
        inline bool Acquire() {
#ifdef _win_
            return ::WaitForSingleObject(Handle, INFINITE) == WAIT_OBJECT_0;
#else
    #ifdef USE_SYSV_SEMAPHORES
            struct sembuf ops[] = {{0, -1, SEM_UNDO}};
            int ret = semop(Handle, ops, 1);
            //return true;
    #else
            int ret = sem_wait(Handle);
    #endif
            if (ret != 0)
                Errno = errno;
            return ret == 0;
#endif
        }

        inline bool TryAcquire() {
#ifdef _win_
            // zero-second time-out interval
            // WAIT_OBJECT_0: current free count > 0
            // WAIT_TIMEOUT:  current free count == 0
            return ::WaitForSingleObject(Handle, 0) == WAIT_OBJECT_0;
#else
    #ifdef USE_SYSV_SEMAPHORES
            struct sembuf ops[] = {{0, -1, SEM_UNDO|IPC_NOWAIT}};
            int ret = semop(Handle, ops, 1);
    #else
            int ret = sem_trywait(Handle);
    #endif
            if (ret != 0)
                Errno = errno;
            return ret == 0;
#endif
        }

        inline const char* get_strerror() {
#ifdef _win_
            return "";
#else
            return strerror(Errno);
#endif
        }
};

TSemaphore::TSemaphore(const char* name, ui32 maxFreeCount)
    : Impl_(new TImpl(name, maxFreeCount))
{
}

TSemaphore::~TSemaphore() throw () {
}

bool TSemaphore::Release() {
    return Impl_->Release();
}

bool TSemaphore::Acquire() {
    return Impl_->Acquire();
}

bool TSemaphore::TryAcquire() {
    return Impl_->TryAcquire();
}
