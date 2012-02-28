LIBRARY()

ENABLE(WERROR)

ENABLE(NOUTIL)

SRCS(
    hostip.cpp
    init.cpp
    poller.cpp
    socket.cpp
    pair.cpp
    address.cpp

    ip.h
    pollerimpl.h
    sock.h
)

END()
