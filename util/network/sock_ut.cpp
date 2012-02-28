#include "sock.h"

#include <library/unittest/registar.h>

class TSocketTest: public TTestBase {
        UNIT_TEST_SUITE(TSocketTest);
            UNIT_TEST(InetDgramTest);
            UNIT_TEST(LocalDgramTest);
            UNIT_TEST(InetStreamTest);
            UNIT_TEST(LocalStreamTest);
        UNIT_TEST_SUITE_END();
    private:
        void InetDgramTest();
        void LocalDgramTest();
        void InetStreamTest();
        void LocalStreamTest();
};

UNIT_TEST_SUITE_REGISTRATION(TSocketTest);

void TSocketTest::InetDgramTest() {
    char buf[256];
    TSockAddrInetDgram servAddr(IpFromString("127.0.0.1"), 12345);
    TSockAddrInetDgram cliAddr(IpFromString("127.0.0.1"), 32345);
    TSockAddrInetDgram servFromAddr;
    TSockAddrInetDgram cliFromAddr;
    TInetDgramSocket cliSock;
    TInetDgramSocket servSock;
    cliSock.CheckSock();
    servSock.CheckSock();

    TBaseSocket::Check(cliSock.Bind(&cliAddr));
    TBaseSocket::Check(servSock.Bind(&servAddr));

    // client
    const char reqStr[] = "Hello, world!!!";
    TBaseSocket::Check(cliSock.SendTo(reqStr, sizeof(reqStr), &servAddr));

    // server
    TBaseSocket::Check(servSock.RecvFrom(buf, 256, &servFromAddr));
    UNIT_ASSERT(strcmp(reqStr, buf) == 0);
    const char repStr[] = "The World's greatings to you";
    TBaseSocket::Check(servSock.SendTo(repStr, sizeof(repStr), &servFromAddr));

    // client
    TBaseSocket::Check(cliSock.RecvFrom(buf, 256, &cliFromAddr));
    UNIT_ASSERT(strcmp(repStr, buf) == 0);
}

void TSocketTest::LocalDgramTest() {
    char buf[256];
    TSockAddrLocalDgram servAddr("./serv_sock");
    TSockAddrLocalDgram cliAddr("./cli_sock");
    TSockAddrLocalDgram servFromAddr;
    TSockAddrLocalDgram cliFromAddr;
    TLocalDgramSocket cliSock;
    TLocalDgramSocket servSock;
    cliSock.CheckSock();
    servSock.CheckSock();

    TBaseSocket::Check(cliSock.Bind(&cliAddr));
    TBaseSocket::Check(servSock.Bind(&servAddr));

    // client
    const char reqStr[] = "Hello, world!!!";
    TBaseSocket::Check(cliSock.SendTo(reqStr, sizeof(reqStr), &servAddr));

    // server
    TBaseSocket::Check(servSock.RecvFrom(buf, 256, &servFromAddr));
    UNIT_ASSERT(strcmp(reqStr, buf) == 0);
    const char repStr[] = "The World's greatings to you";
    TBaseSocket::Check(servSock.SendTo(repStr, sizeof(repStr), &servFromAddr));

    // client
    TBaseSocket::Check(cliSock.RecvFrom(buf, 256, &cliFromAddr));
    UNIT_ASSERT(strcmp(repStr, buf) == 0);
}

void TSocketTest::InetStreamTest() {
    char buf[256];
    TSockAddrInetStream servAddr(IpFromString("127.0.0.1"), 12345);
    TSockAddrInetStream newAddr;
    TInetStreamSocket cliSock;
    TInetStreamSocket servSock;
    TInetStreamSocket newSock;
    cliSock.CheckSock();
    servSock.CheckSock();
    newSock.CheckSock();

    // server
    int yes = 1;
    CheckedSetSockOpt(servSock, SOL_SOCKET, SO_REUSEADDR, yes, "servSock, SO_REUSEADDR");
    TBaseSocket::Check(servSock.Bind(&servAddr), "bind");
    TBaseSocket::Check(servSock.Listen(10), "listen");

    // client
    TBaseSocket::Check(cliSock.Connect(&servAddr), "connect");

    // server
    TBaseSocket::Check(servSock.Accept(&newSock, &newAddr), "accept");

    // client
    const char reqStr[] = "Hello, world!!!";
    TBaseSocket::Check(cliSock.Send(reqStr, sizeof(reqStr)), "send");

    // server - new
    TBaseSocket::Check(newSock.Recv(buf, 256), "recv");
    UNIT_ASSERT(strcmp(reqStr, buf) == 0);
    const char repStr[] = "The World's greatings to you";
    TBaseSocket::Check(newSock.Send(repStr, sizeof(repStr)), "send");

    // client
    TBaseSocket::Check(cliSock.Recv(buf, 256), "recv");
    UNIT_ASSERT(strcmp(repStr, buf) == 0);
}

void TSocketTest::LocalStreamTest() {
    char buf[256];
    TSockAddrLocalStream servAddr("./serv_sock2");
    TSockAddrLocalStream newAddr;
    TLocalStreamSocket cliSock;
    TLocalStreamSocket servSock;
    TLocalStreamSocket newSock;
    cliSock.CheckSock();
    servSock.CheckSock();
    newSock.CheckSock();

    // server
    TBaseSocket::Check(servSock.Bind(&servAddr), "bind");
    TBaseSocket::Check(servSock.Listen(10), "listen");

    // client
    TBaseSocket::Check(cliSock.Connect(&servAddr), "connect");

    // server
    TBaseSocket::Check(servSock.Accept(&newSock, &newAddr), "accept");

    // client
    const char reqStr[] = "Hello, world!!!";
    TBaseSocket::Check(cliSock.Send(reqStr, sizeof(reqStr)), "send");

    // server - new
    TBaseSocket::Check(newSock.Recv(buf, 256), "recv");
    UNIT_ASSERT(strcmp(reqStr, buf) == 0);
    const char repStr[] = "The World's greatings to you";
    TBaseSocket::Check(newSock.Send(repStr, sizeof(repStr)), "send");

    // client
    TBaseSocket::Check(cliSock.Recv(buf, 256), "recv");
    UNIT_ASSERT(strcmp(repStr, buf) == 0);
}

