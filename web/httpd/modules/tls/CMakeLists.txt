LIBRARY()

PEERDIR(
    web/httpd/kernel
    contrib/libs/matrixssl
)

SRCS(
    tlsio.cpp
    module.cpp
)

END()
