LIBRARY()

PEERDIR(
    util/server

    library/lua
    library/pire
    library/svnversion
)

SRCS(
    main.cpp
    log.cpp
    chunks.cpp
    luacfg.cpp
    iobase.cpp
    socket.cpp
    iface.cpp
    regexp.cpp
    format.cpp
    headers.cpp
    alloc.cpp
    null.cpp
    httpfsm.rl
    http.cpp
)

END()
