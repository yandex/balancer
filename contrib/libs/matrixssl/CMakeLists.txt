LIBRARY()

ENABLE(NOUTIL)
ENABLE(NO_COMPILER_WARNINGS)

DEFAULT(MATRIXSSL_DEBUG 0)
IF(MATRIXSSL_DEBUG)
    CFLAGS(-DMSSLDEBUG)
ENDIF(MATRIXSSL_DEBUG)

DEFAULT(MATRIXSSL_DLL 0)
IF(MATRIXSSL_DLL)
    CFLAGS(-DMATRIXSSL_DLL)
ENDIF(MATRIXSSL_DLL)

SRCS(
    src/cipherSuite.c
    src/matrixSsl.c
    src/sslDecode.c
    src/sslEncode.c
    src/sslv3.c
    src/yandex.c
    src/crypto/peersec/arc4.c
    src/crypto/peersec/base64.c
    src/crypto/peersec/des3.c
    src/crypto/peersec/md2.c
    src/crypto/peersec/md5.c
    src/crypto/peersec/mpi.c
    src/crypto/peersec/rsa.c
    src/crypto/peersec/sha1.c
    src/os/debug.c
    src/os/yandex/yandex.cpp
    src/pki/asn1.c
    src/pki/rsaPki.c
    src/pki/x509.c

    matrixSsl.h
    matrixCommon.h
    src/matrixConfig.h
    src/matrixInternal.h
    src/crypto/cryptoLayer.h
    src/crypto/matrixCrypto.h 
    src/crypto/peersec/mpi.h
    src/crypto/peersec/pscrypto.h
    src/os/osLayer.h
    src/os/psMalloc.h
    src/pki/matrixPki.h
    src/pki/pkiInternal.h
)

END()
