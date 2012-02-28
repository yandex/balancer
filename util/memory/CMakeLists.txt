LIBRARY()

ENABLE(NOUTIL)

SRCS(
    profile.cpp
    tempbuf.cpp
    blob.cpp
    mmapalloc.cpp
    alloc.cpp
    pool.cpp
    
    addstorage.h
    gc.h
    segmented_string_pool.h
    segpool_alloc.h
    smallobj.h
)

END()
