LIBRARY()

SRCS(
    http_digest.cpp
    http_socket.cpp
    httpload.cpp
    exthttpcodes.cpp
    sockhandler.cpp

    httpfsm.rl
    
    httpagent.h
    httpfetcher.h
    httpheader.h
    httpparser.h
    httpzreader.h
    sockhandler.h
)

PEERDIR(contrib/libs/matrixssl)

END()
