#pragma once

#include "str_stl.h"

#include <util/system/defaults.h>
#include <util/string/cast.h>
#include <util/string/cgiparam.h>
#include <util/network/socket.h>
#include <util/generic/hash.h>
#include <util/generic/stroka.h>
#include <util/datetime/base.h>

class TServerRequestData
{
private:
    typedef yhash<const char *, const char *, ci_hash, ci_equal_to> HeaderInHash;
public:
    TServerRequestData(SOCKET s = INVALID_SOCKET)
      : Addr(0)
      , Host()
      , Port()
      , Path(0)
      , Search(0)
      , Socket(s)
      , BeginTime(MicroSeconds())
    {
    }
    TServerRequestData(const char *qs, SOCKET s = INVALID_SOCKET)
      : Addr(0)
      , Host()
      , Port()
      , Path(0)
      , Search((char*)qs)
      , Socket(s)
      , BeginTime(MicroSeconds())
    {
        Scan();
    }
    void SetHost(const Stroka& host, ui16 port) {
        Host = host;
        Port = ToString(port);
    }
    const char* ServerName() const {
        return ~Host;
    }
    const char* ServerPort() const {
        return ~Port;
    }
    const char* ScriptName() const {
        return Path;
    }
    const char* QueryString() const {
        return Search;
    }
    const char* RemoteAddr() const {
        if (!Addr) {
            *AddrData = 0;
            GetRemoteAddr(Socket, AddrData, sizeof(AddrData));
            Addr = AddrData;
        }
        return Addr;
    }
    const char* HeaderIn(const char* key) const {
        HeaderInHash::const_iterator I = HeadersIn.find((char *)key);
        if (I == HeadersIn.end())
            return 0;
        return (*I).second;
    }
    inline size_t HeadersCount() const throw () {
        return HeadersIn.size();
    }
    inline Stroka HeaderByIndex(size_t n) const throw () {
        if (n >= HeadersCount()) {
            return 0;
        }

        HeaderInHash::const_iterator i = HeadersIn.begin();

        while (n) {
            ++i;
            --n;
        }

        return Stroka(i->first) + ": " + i->second;
    }
    const char *Environment(const char* key) const {
        if (stricmp(key, "REMOTE_ADDR") == 0) {
            const char* ip = HeaderIn("X-Real-IP");
            if (ip)
                return ip;
            return RemoteAddr();
        } else if (stricmp(key, "QUERY_STRING") == 0) {
            return QueryString();
        } else if (stricmp(key, "SERVER_NAME") == 0) {
            return ServerName();
        } else if (stricmp(key, "SERVER_PORT") == 0) {
            return ServerPort();
        } else if (stricmp(key, "SCRIPT_NAME") == 0) {
            return ScriptName();
        }
        return 0;
    }
    void Scan() {
        CgiParam.Scan(Search);
    }
    void Clear() {
        HeadersIn.clear();
        Addr = Path = Search = NULL;
        Host.clear();
        Port.clear();
        CurPage.remove();
        ParseBuf.remove();
        CgiParam.Flush();
        BeginTime = MicroSeconds();
    }
    void SetSocket(SOCKET s) throw () {
        Socket = s;
    }
    ui64 RequestBeginTime() const throw () {
        return BeginTime;
    }
    void Scan(const char *const *scriptEnv); // for Apache only
    const char* GetCurPage() const;
    bool Parse(const char* req);
    void AddHeader(char* ss);
private:
    mutable char* Addr;
    Stroka Host;
    Stroka Port;
    char* Path;
    char* Search;
    HeaderInHash HeadersIn;
    mutable char AddrData[16];
    SOCKET Socket;
    ui64 BeginTime;
    mutable Stroka CurPage;
    Stroka ParseBuf;
public:
    TCgiParameters CgiParam;
};

inline const char* TServerRequestData::GetCurPage() const
{
    if (!CurPage && !!Host) {
        CurPage = "http://";
        CurPage += Host;
        if (!!Port) {
            CurPage += ":";
            CurPage += Port;
        }
        CurPage += Path;
        if (Search) {
            CurPage += "?";
            CurPage += Search;
        }
    }
    return ~CurPage;
}

inline bool TServerRequestData::Parse(const char* req_in) {
    ParseBuf = req_in;
    char* req = ParseBuf.begin();

    while (*req == ' ' || *req == '\t')
        req++;
    if (*req != '/')
        return false; // we are not a proxy
    while (req[1] == '/')   // remove redundant slashes
        req++;
    char *r = req;
    while (*r && *r != ' ' && *r != '\t')
        r++;
    if (*r)
        *r++ = 0;
    char *f = strchr(req, '#');
    if (f)
        *f = 0; // ignore fragment
    Path = req;
    Search = strchr(Path,'?');
    if (Search)
        *Search++ = 0;
    size_t len = strlen(Path);
    if (len > 1 && Path[len-1] == '/')
        Path[len-1] = 0;
    return true;
}

inline void TServerRequestData::AddHeader(char* ss)
{
    char* p = strchr(ss, ':');
    if (!p)
        return;

    *p++ = 0;
    p += strspn(p, " \t");
    HeadersIn[ss] = p;

    if (stricmp(ss, "Host") == 0) {
        size_t hostLen = strcspn(p, ":");
        if (p[hostLen] == ':')
            Port = p + hostLen + 1;
        Host = Stroka(p, hostLen);
    }
}

inline void TServerRequestData::Scan(const char *const *scriptEnv)
{
    for (const char *const *c = scriptEnv; *c; ++c) {
        const char *key = *c;
        if (strnicmp(key, "REMOTE_ADDR=", 12) == 0 || strnicmp(key, "HTTP_UserIP=", 12) == 0) {
            Addr = (char*)key + 12;
        } else if (strnicmp(key, "QUERY_STRING=", 13) == 0) {
            Search = (char*)key + 13;
            CgiParam.Scan(Search); // an additional job for '13' :)
        } else if (strnicmp(key, "SERVER_NAME=", 12) == 0) {
            Host = (char*)key + 12;
        } else if (strnicmp(key, "SERVER_PORT=", 12) == 0) {
            Port = (char*)key + 12;
        } else if (strnicmp(key, "SCRIPT_NAME=", 12) == 0) {
            Path = (char*)key + 12;
        }
    }
}
