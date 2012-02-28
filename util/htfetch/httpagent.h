#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>

#ifndef WIN32
  #include <unistd.h>
  #include <fcntl.h>
#endif

#include <util/http_url.h>
#include <util/network/hostip.h>
#include <util/network/ip.h>
#include <util/generic/utility.h>
#include <util/httpcodes.h>

#include "sockhandler.h"

class TIpResolver {
    public:
        static int GetHostIP(const char* hostname, ui32* ip) {
            return NResolver::GetHostIP(hostname, ip);
        }

        static int GetDnsError() {
            return NResolver::GetDnsError();
        }
};

template <class TSocketHandler = TSimpleSocketHandler, class TDnsClient = TIpResolver>
    class THttpAgent
{
public:
    THttpAgent() : Ip(0), Persistent(0), Timeout(150), Hostheader(NULL), Footer(NULL), AltFooter(NULL), HostheaderLen(0) {
        SetIdentification("YandexSomething/1.0", "webadmin@yandex.ru");
    }

    ~THttpAgent() {
        Disconnect();
        free(Hostheader);
        free(Footer);
    }

    void SetIdentification(const char* user_agent, const char* http_from) {
        free(Footer);
        size_t len = user_agent ? strlen(user_agent) + 15 : 0;
        len += http_from ? strlen(http_from) + 9 : 0;
        len += 3;
        Footer = (char*) malloc(len);
        if (user_agent)
            strcat(strcat(strcpy(Footer, "User-Agent: "), user_agent), "\r\n");
        if (http_from)
            strcat(strcat(strcat(Footer, "From: "), http_from), "\r\n");
    }

    void SetUserAgentFooter(const char *altFooter) {
        AltFooter = altFooter;
    }

    ui32     GetIp() { return Ip; }
    TIpPort    GetPort() const { return Port; }
    int      GetScheme() const { return THttpURL::SchemeHTTP; }
    void     SetTimeout(time_t tim) { Timeout = tim; }
    int      Disconnected() { return !Persistent || !Socket.Good(); }

    int SetHost(const char* hostname, TIpPort port) {
        Disconnect();
        int ret;
        ui32 _ip = 0;
        if ((ret = DnsClient.GetHostIP(hostname, &_ip)))
            return ret;
        SetHost(hostname, port, _ip);
        return 0;
    }

    int SetHost(const char* hostname, TIpPort port, ui32 ip) {
        Disconnect();
        Ip = ip;
        Port = port;
        size_t reqHostheaderLen = strlen(hostname) + 20;
        if (HostheaderLen < reqHostheaderLen) {
            free(Hostheader);
            Hostheader = (char *)malloc((HostheaderLen = reqHostheaderLen));
        }
        if (port == 80)
            sprintf(Hostheader, "Host: %s\r\n", hostname);
        else
            sprintf(Hostheader, "Host: %s:%u\r\n", hostname, port);
        pHostBeg = strchr(Hostheader, ' ') + 1;
        pHostEnd = strchr(pHostBeg, '\r');
        // convert hostname to lower case since some web server don't like
        // uppper case (Task ROBOT-562)
        for (char *p = pHostBeg; p < pHostEnd; p++)
            *p = tolower(*p);
        return 0;
    }

    void SetSocket(SOCKET fd) {
        Socket.SetSocket(fd);
    }

    SOCKET PickOutSocket() {
        return Socket.PickOutSocket();
    }

    void Disconnect() {
        Socket.Disconnect();
    }

    ssize_t read(void *buffer, size_t buflen)
    {
        return Socket.read(buffer, buflen);
    }

    int RequestGet(const char* url, const char **headers, int persistent = 1, bool head_request = false) {
        if (!Ip)
            return HTTP_DNS_FAILURE;
        char message[MessageMax];
        ssize_t messlen = 0;
        if (head_request) {
            strcpy(message, "HEAD ");
            messlen = 5;
        } else {
            strcpy(message, "GET ");
            messlen = 4;
        }
#define _AppendMessage(mes) messlen += Min(MessageMax - messlen, \
(ssize_t)strlcpy(message + messlen, (mes), MessageMax - messlen))
        _AppendMessage(url);
        _AppendMessage(" HTTP/1.1\r\n");
        if (*url == '/')  //if not then Host is a proxy
            _AppendMessage(Hostheader);
        _AppendMessage("Connection: ");
        _AppendMessage(persistent? "Keep-Alive\r\n": "Close\r\n");
        while (headers && *headers)
            _AppendMessage(*headers++);
        if (AltFooter)
            _AppendMessage(AltFooter);
        else
            _AppendMessage(Footer);
        _AppendMessage("\r\n");
#undef _AppendMessage
        if (messlen >= MessageMax)
            return HTTP_HEADER_TOO_LARGE;
        if (!Persistent)
            Disconnect();
        Persistent = persistent;
        int connected = Socket.Good();
        for (int attempt = !connected; attempt < 2; attempt++) {
            if (!Socket.Good() && Socket.Connect(Ip, Port, Timeout))
                return HTTP_CONNECT_FAILED;
            if (!Socket.send(message, messlen)) {
                int err = errno;
                Disconnect();
                errno = err;
                continue;
            }

            if (!Socket.peek()) {
                int err = errno;
                Disconnect();
                if (err == EINTR) {
                    errno = err;
                    return HTTP_INTERRUPTED;
                }
            } else {
                if (!persistent)
                    Socket.shutdown();
                return 0;
            }
        }
        return connected ? HTTP_CONNECTION_LOST : HTTP_CONNECT_FAILED;
    }

protected:
    TSocketHandler Socket;
    TDnsClient DnsClient;
    ui32 Ip;
    int Persistent;
    time_t Timeout;
    char *Hostheader, *Footer, *pHostBeg, *pHostEnd;
    const char *AltFooter; // alternative footer can be set by the caller
    unsigned short HostheaderLen;
    TIpPort Port;
    static const ssize_t MessageMax = 4096;
};

template <class TSockHndl = TSimpleSocketHandler, class TDnsClient = TIpResolver, class TErrorLogger = TSslSocketBase::TFakeLogger>
class THttpsAgent
{
public:
    class TFakeLogger {
    public:
        static void Write(const char * /*format*/, ...) {
        }
    };

    typedef TSslSocketHandler<TSockHndl, TErrorLogger> TSocket;
    THttpsAgent()
        : Socket(new TSocket)
        , Ip(0)
        , Scheme(0)
        , Persistent(0)
        , Timeout(150)
        , Hostheader(NULL)
        , Footer(NULL)
        , pHostBeg(NULL)
        , pHostEnd(NULL)
        , AltFooter(NULL)
        , HostheaderLen(0)
        , Port(0)
        , ConnPort(0)
    {
        SetIdentification("YandexSomething/1.0", "webadmin@yandex.ru");
    }

    ~THttpsAgent() {
        Disconnect();
        delete[] Hostheader;
        delete[] Footer;
    }

    void SetIdentification(const char* userAgent, const char* httpFrom) {
        VERIFY(Socket.Get(), "HttpsAgent: socket is picked out. Can't use until a valid socket is set");
        delete[] Footer;
        size_t len = userAgent ? strlen(userAgent) + 15 : 0;
        len += httpFrom ? strlen(httpFrom) + 9 : 0;
        len += 3;
        Footer = new char[len];
        if (userAgent)
            strcat(strcat(strcpy(Footer, "User-Agent: "), userAgent), "\r\n");
        if (httpFrom)
            strcat(strcat(strcat(Footer, "From: "), httpFrom), "\r\n");
    }

    void SetUserAgentFooter(const char *altFooter) {
        AltFooter = altFooter;
    }

    ui32 GetIp() const {
        return Ip;
    }

    TIpPort GetPort() const {
        return Port;
    }

    int GetScheme() const {
        return Scheme;
    }

    void SetTimeout(time_t tim) {
        Timeout = tim;
    }

    int Disconnected() {
        return !Persistent || !Socket.Get() || !Socket->Good();
    }

    int SetHost(const char* hostname, TIpPort port, int scheme = THttpURL::SchemeHTTP) {
        int ret;
        ui32 _ip = 0;
        if ((ret = DnsClient.GetHostIP(hostname, &_ip)))
            return ret;
        SetHost(hostname, port, _ip, scheme);
        return 0;
    }

    int SetHost(const char* hostname, TIpPort port, ui32 ip, int scheme = THttpURL::SchemeHTTP, TIpPort connPort = 0) {
        Disconnect();
        Ip = ip;
        Port = port;
        ConnPort = connPort ? connPort : port;
        Scheme = scheme;
        size_t reqHostheaderLen = strlen(hostname) + 20;
        if (HostheaderLen < reqHostheaderLen) {
            delete[] Hostheader;
            Hostheader = new char[(HostheaderLen = reqHostheaderLen)];
        }
        if (Scheme == THttpURL::SchemeHTTPS && port == 443 || port == 80)
            sprintf(Hostheader, "Host: %s\r\n", hostname);
        else
            sprintf(Hostheader, "Host: %s:%u\r\n", hostname, port);
        pHostBeg = strchr(Hostheader, ' ') + 1;
        pHostEnd = strchr(pHostBeg, '\r');
        // convert hostname to lower case since some web server don't like
        // uppper case (Task ROBOT-562)
        for (char *p = pHostBeg; p < pHostEnd; p++)
            *p = tolower(*p);
        SocketCtx.Host = pHostBeg;
        SocketCtx.HostLen = pHostEnd - pHostBeg;
        return 0;
    }

    void SetSocket(TSocket *s) {
        VERIFY(s, "HttpsAgent: socket handler is null");
        SocketCtx.FreeBuffers();
        if (s->HasSsl())
            SocketCtx.AllocBuffers();
        Socket.Reset(s);
    }

    TSocket *PickOutSocket() {
        SocketCtx.FreeBuffers();
        SocketCtx.CachedSession.Destroy();
        return Socket.Release();
    }

    void Disconnect() {
        if (Socket.Get())
            Socket->Disconnect();
        SocketCtx.FreeBuffers();
        SocketCtx.CachedSession.Destroy();
    }

    ssize_t read(void *buffer, size_t buflen) {
        VERIFY(Socket.Get(), "HttpsAgent: socket is picked out. Can't use until a valid socket is set");
        return Socket->read(&SocketCtx, buffer, buflen);
    }

    int RequestGet(const char* url, const char **headers, int persistent = 1, bool head_request = false) {
        VERIFY(Socket.Get(), "HttpsAgent: socket is picked out. Can't use until a valid socket is set");
        if (!Ip)
            return HTTP_DNS_FAILURE;
        char message[MessageMax];
        ssize_t messlen = 0;
        if (head_request) {
            strcpy(message, "HEAD ");
            messlen = 5;
        } else {
            strcpy(message, "GET ");
            messlen = 4;
        }
#define _AppendMessage(mes) messlen += Min(MessageMax - messlen, \
(ssize_t)strlcpy(message + messlen, (mes), MessageMax - messlen))
        _AppendMessage(url);
        _AppendMessage(" HTTP/1.1\r\n");
        _AppendMessage(Hostheader);
        _AppendMessage("Connection: ");
        _AppendMessage(persistent? "Keep-Alive\r\n": "Close\r\n");
        while (headers && *headers)
            _AppendMessage(*headers++);
        if (AltFooter)
            _AppendMessage(AltFooter);
        else
            _AppendMessage(Footer);
        _AppendMessage("\r\n");
#undef _AppendMessage
        if (messlen >= MessageMax)
            return HTTP_HEADER_TOO_LARGE;
        if (!Persistent)
            Socket->Disconnect(&SocketCtx);
        Persistent = persistent;
        int connected = Socket->Good();
        SocketCtx.FreeBuffers();
        if (Scheme == THttpURL::SchemeHTTPS)
            SocketCtx.AllocBuffers();
        for (int attempt = !connected; attempt < 2; attempt++) {
            if (!Socket->Good() && Socket->Connect(&SocketCtx, Ip, ConnPort, Timeout, Scheme == THttpURL::SchemeHTTPS, true)) {
                SocketCtx.FreeBuffers();
                return HTTP_CONNECT_FAILED;
            }
            if (!Socket->send(&SocketCtx, message, messlen)) {
                int err = errno;
                Socket->Disconnect(&SocketCtx);
                errno = err;
                continue;
            }

            if (!Socket->peek(&SocketCtx)) {
                int err = errno;
                Socket->Disconnect(&SocketCtx);
                if (err == EINTR) {
                    errno = err;
                    SocketCtx.FreeBuffers();
                    return HTTP_INTERRUPTED;
                }
            } else {
                if (!persistent)
                    Socket->shutdown();
                return 0;
            }
        }
        SocketCtx.FreeBuffers();
        return connected ? HTTP_CONNECTION_LOST : HTTP_CONNECT_FAILED;
    }

    ui16 CertCheckErrors() const {
        return SocketCtx.CertErrors;
    }

protected:
    THolder<TSocket> Socket;
    typename TSocket::TSocketCtx SocketCtx;
    TDnsClient DnsClient;
    ui32 Ip;
    int Scheme;
    int Persistent;
    time_t Timeout;
    char *Hostheader, *Footer, *pHostBeg, *pHostEnd;
    const char *AltFooter; // alternative footer can be set by the caller
    unsigned short HostheaderLen;
    TIpPort Port;
    TIpPort ConnPort;
    static const ssize_t MessageMax = 4096;
};
