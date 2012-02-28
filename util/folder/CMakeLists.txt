LIBRARY()

ENABLE(NOUTIL)

SRCS(
    iterator.h
    fts.cpp
    filelist.cpp
    dirut.cpp
)

IF (WIN32)
    SRCS(
        lstat_win.c
        dirent_win.c
    )
ENDIF (WIN32)

END()
