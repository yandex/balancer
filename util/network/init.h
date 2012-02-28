#pragma once

#include <util/system/error.h>

#if defined(_unix_)
    #include <fcntl.h>
    #include <netdb.h>
    #include <time.h>
    #include <unistd.h>

    #include <sys/uio.h>
    #include <sys/time.h>
    #include <sys/poll.h>
    #include <sys/types.h>
    #include <sys/socket.h>

    #include <netinet/in.h>
    #include <arpa/inet.h>

    typedef int SOCKET;

    #define closesocket(s)           close(s)
    #define SOCKET_ERROR             -1
    #define INVALID_SOCKET           -1
    #define WSAGetLastError()        errno
#elif defined(_win_)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <io.h>
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>

    typedef ULONG nfds_t;

    #undef Yield
#endif

void InitNetworkSubSystem();

static struct TNetworkInitializer {
    inline TNetworkInitializer() {
        InitNetworkSubSystem();
    }
} NetworkInitializerObject;
