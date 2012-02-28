LIBRARY()

ENABLE(NOUTIL)

IF (NOT WIN32)
    SET(WERROR yes)
ENDIF (NOT WIN32)

SRCS(
    align.h
    atexit.cpp
    atomic.h
    atomic_fake.h
    atomic_gcc.h
    atomic_win.h
    atomic_x86.h
    byteorder.h
    backtrace.cpp
    compat.cpp
    condvar.cpp
    context.cpp
    daemon.cpp
    datetime.cpp
    dynlib.cpp
    err.cpp
    error.cpp
    event.cpp
    execpath.cpp
    file.cpp
    filemap.cpp
    flock.cpp
    guard.h
    hostname.cpp
    info.cpp
    mutex.cpp
    oldfile.cpp
    rusage.cpp
    rwlock.cpp
    sem.cpp
    spinlock.cpp
    strlcpy.c
    fstat.cpp
    tempfile.h
    thread.cpp
    tls.cpp
    yield.cpp
    yassert.cpp
    pipe.cpp
    demangle.cpp
    progname.cpp
    fs.cpp
    mktemp.cpp
    user.cpp

    defaults.h
    maxlen.h
    utime.h
)

IF (LINUX)
    SRCS(
        valgrind.h
    )
    SET_APPEND(OBJADDE -lrt)
ENDIF (LINUX)

IF (LINUX OR SUN OR CYGWIN OR WIN32)
    SRCS(
        freeBSD_mktemp.cpp
    )
ENDIF (LINUX OR SUN OR CYGWIN OR WIN32)

IF (NOT WIN32)
    SRCS(
        context_x86.asm
    )
ENDIF (NOT WIN32)

END()
