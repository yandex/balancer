LIBRARY()

PEERDIR(
    web/httpd/kernel
)

SRCS(
    module.cpp
    fair.cpp
    weighted.cpp
    weighted2.cpp
    robust.cpp
    arbiter.cpp
    pidgroup.cpp
    iterator.cpp
    rrobin.cpp
    smartrr.cpp
    backend.cpp
    hashing.cpp
)

END()
