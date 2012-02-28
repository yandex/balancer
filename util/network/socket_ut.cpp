#include "socket.h"

#include <ctime>

#include <library/unittest/registar.h>

#include <util/generic/vector.h>
#include <util/string/printf.h>

class TSockTest: public TTestBase {
    UNIT_TEST_SUITE(TSockTest);
        UNIT_TEST(TestSock);
        UNIT_TEST(TestTimeout);
#ifndef _win_ // Test hangs on Windows
        UNIT_TEST_EXCEPTION(TestConnectionRefused, yexception);
#endif
    UNIT_TEST_SUITE_END();
    public:
        void TestSock();
        void TestTimeout();
        void TestConnectionRefused();
};

UNIT_TEST_SUITE_REGISTRATION(TSockTest);

void TSockTest::TestSock() {
    TNetworkAddress addr("yandex.ru", 80);
    TSocket s(addr);
    TSocketOutput so(s);
    TSocketInput si(s);

    so.Write("GET / HTTP/1.0\r\n\r\n", 18);

    UNIT_ASSERT(!si.ReadLine().empty());
}

void TSockTest::TestTimeout() {
    static const int timeout = 1000;
    i64 startTime = millisec();
    try {
        TNetworkAddress addr("localhost", 1313);
        TSocket s(addr, TDuration::MilliSeconds(timeout));
    }
    catch (const yexception&) {
    }
    int realTimeout = (int)(millisec() - startTime);
    if (realTimeout > timeout + 2000) {
        Stroka err = Sprintf("Timeout exceeded: %d ms (expected %d ms)", realTimeout, timeout);
        UNIT_FAIL(err);
    }
}

void TSockTest::TestConnectionRefused() {
    TNetworkAddress addr("localhost", 1313);
    TSocket s(addr);
}

class TPollTest: public TTestBase {
        UNIT_TEST_SUITE(TPollTest);
            UNIT_TEST(TestPollInOut);
        UNIT_TEST_SUITE_END();
    public:
        inline TPollTest() {
            srand(static_cast<unsigned int>(time(0)));
        }

        void TestPollInOut();

    private:
        sockaddr_in GetAddress(ui32 ip, ui16 port);
        SOCKET CreateSocket();
        SOCKET StartServerSocket(ui16 port, int backlog);
        SOCKET StartClientSocket(ui32 ip, ui16 port);
        SOCKET AcceptConnection(SOCKET serverSocket);
};

UNIT_TEST_SUITE_REGISTRATION(TPollTest);

sockaddr_in TPollTest::GetAddress(ui32 ip, ui16 port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip);
    return addr;
}

SOCKET TPollTest::CreateSocket() {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET)
        ythrow yexception() << "Can not create socket (" << LastSystemErrorText() << ")";
    return s;
}

SOCKET TPollTest::StartServerSocket(ui16 port, int backlog) {
    TSocketHolder s(CreateSocket());
    sockaddr_in addr = GetAddress(ntohl(INADDR_ANY), port);
    if (bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        ythrow yexception() << "Can not bind server socket (" << LastSystemErrorText() << ")";
    if (listen(s, backlog) == SOCKET_ERROR)
        ythrow yexception() << "Can not listen on server socket (" << LastSystemErrorText() << ")";
    return s.Release();
}

SOCKET TPollTest::StartClientSocket(ui32 ip, ui16 port) {
    TSocketHolder s(CreateSocket());
    sockaddr_in addr = GetAddress(ip, port);
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        ythrow yexception() << "Can not connect client socket (" << LastSystemErrorText() << ")";
    return s.Release();
}

SOCKET TPollTest::AcceptConnection(SOCKET serverSocket) {
    SOCKET connectedSocket = accept(serverSocket, 0, 0);
    if (connectedSocket == INVALID_SOCKET)
        ythrow yexception() << "Can not accept connection on server socket (" << LastSystemErrorText() << ")";
    return connectedSocket;
}

void TPollTest::TestPollInOut() {
#ifdef _win_
    const size_t socketCount = 1000;

    ui16 port = static_cast<ui16>(1300 + rand() % 97);
    TSocketHolder serverSocket = StartServerSocket(port, socketCount);

    ui32 localIp = ntohl(inet_addr("127.0.0.1"));

    yvector< TSharedPtr<TSocketHolder> > clientSockets;
    yvector< TSharedPtr<TSocketHolder> > connectedSockets;
    yvector<pollfd> fds;

    for (size_t i = 0; i < socketCount; ++i) {
        TSharedPtr<TSocketHolder> clientSocket(new TSocketHolder(StartClientSocket(localIp, port)));
        clientSockets.push_back(clientSocket);

        if (i % 5 == 0 || i % 5 == 2) {
            char buffer = 'c';
            if (send(*clientSocket, &buffer, 1, 0) == -1)
                ythrow yexception() << "Can not send (" << LastSystemErrorText() << ")";
        }

        TSharedPtr<TSocketHolder> connectedSocket(new TSocketHolder(AcceptConnection(serverSocket)));
        connectedSockets.push_back(connectedSocket);

        if (i % 5 == 2 || i % 5 == 3) {
            closesocket(*clientSocket);
            shutdown(*clientSocket, SD_BOTH);
        }
    }

    for (size_t i = 0; i < connectedSockets.size(); ++i) {
        pollfd fd = { (i % 5 == 4) ? INVALID_SOCKET : *connectedSockets[i], POLLIN | POLLOUT, 0};
        fds.push_back(fd);
    }

    int polledCount = poll(&fds[0], fds.size(), INFTIM);
    UNIT_ASSERT_EQUAL(static_cast<int>(socketCount), polledCount);

    for (size_t i = 0; i < connectedSockets.size(); ++i) {
        if (i % 5 == 0) {
            UNIT_ASSERT_EQUAL(static_cast<short>(POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM), fds[i].revents);
        } else if (i % 5 == 1) {
            UNIT_ASSERT_EQUAL(static_cast<short>(POLLOUT | POLLWRNORM), fds[i].revents);
        } else if (i % 5 == 2) {
            UNIT_ASSERT_EQUAL(static_cast<short>(POLLHUP | POLLRDNORM | POLLIN), fds[i].revents);
        } else if (i % 5 == 3) {
            UNIT_ASSERT_EQUAL(static_cast<short>(POLLHUP), fds[i].revents);
        } else if (i % 5 == 4) {
            UNIT_ASSERT_EQUAL(static_cast<short>(POLLNVAL), fds[i].revents);
        }
    }
#endif
}
