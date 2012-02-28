#pragma once

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include <util/system/maxlen.h>
#include <util/system/compat.h>
#include <util/generic/stroka.h>
#include <util/generic/ylimits.h>
#include <util/generic/static_assert.h>
#include <util/httpcodes.h>

#define DEFAULT_IF_MODIFIED_SINCE -1
#define DEFAULT_MAX_AGE -1
#define DEFAULT_REQUEST_PRIORITY -1
#define DEFAULT_RESPONSE_TIMEOUT -1
#define HTTP_PREFIX "http://"

enum HTTP_COMPRESSION {
    HTTP_COMPRESSION_UNSET    = 0,
    HTTP_COMPRESSION_ERROR    = 1,
    HTTP_COMPRESSION_IDENTITY = 2,
    HTTP_COMPRESSION_GZIP     = 3,
    HTTP_COMPRESSION_DEFLATE  = 4,
    HTTP_COMPRESSION_COMPRESS = 5,
    HTTP_COMPRESSION_MAX      = 6
};

enum HTTP_METHOD {
    HTTP_METHOD_UNDEFINED = -1,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_EXTENSION
};

enum HTTP_CONNECTION {
    HTTP_CONNECTION_UNDEFINED = -1,
    HTTP_CONNECTION_KEEP_ALIVE = 0,
    HTTP_CONNECTION_CLOSE = 1
};

/// Class represents general http header fields.
struct THttpBaseHeader {
public:
    i16    error;
    i32    header_size;
    i32    entity_size;
    i32    content_length;
    i64    http_time; // seconds since epoch
    i32    content_range_start; // Content-Range: first-byte-pos
    i32    content_range_end; // Content-Range: last-byte-pos
    i32    content_range_entity_length; // Content-Range: entity-length
    i8     http_minor;
    i8     mime_type;
    i8     charset;
    i8     compression_method;
    i8     transfer_chunked;
    i8     connection_closed;
    char   base[URL_MAX];
public:
    void Init() {
        error = 0;
        header_size = 0;
        entity_size = 0;
        content_length = -1;
        http_time = -1;
        http_minor = -1;
        mime_type = -1;
        charset = -1;
        compression_method = HTTP_COMPRESSION_UNSET;
        transfer_chunked = -1;
        connection_closed = HTTP_CONNECTION_UNDEFINED;
        content_range_start = -1;
        content_range_end = -1;
        content_range_entity_length = -1;
        base[0] = 0;
    }

    void Print() const {
        printf("content_length: %"PRIi32"\n", content_length);
        printf("http_time: %"PRIi64"\n", http_time);
        printf("http_minor: %"PRIi8"\n", http_minor);
        printf("mime_type: %"PRIi8"\n", mime_type);
        printf("charset: %"PRIi8"\n", charset);
        printf("compression_method: %"PRIi8"\n", compression_method);
        printf("transfer_chunked: %"PRIi8"\n", transfer_chunked);
        printf("connection_closed: %"PRIi8"\n", connection_closed);
        printf("content_range_start: %"PRIi32"\n", content_range_start);
        printf("content_range_end: %"PRIi32"\n", content_range_end);
        printf("content_range_entity_length: %"PRIi32"\n", content_range_entity_length);
        printf("base: \"%s\"\n", base);
        printf("error: %"PRIi16"\n", error);
    }

    int SetBase(const char* path,
                const char* hostNamePtr    = 0,
                int         hostNameLength = 0)
    {
        if (*path == '/')
        {
            if (7 + hostNameLength + strlen(path) >= URL_MAX)
            {
                error = HTTP_URL_TOO_LARGE;
                return error;
            }
            memcpy(base, "http://", 7);
            memcpy(base + 7, hostNamePtr, hostNameLength);
            strcpy(base + 7 + hostNameLength, path);
        } else {
            if (strlen(path) >= URL_MAX)
            {
                error = HTTP_URL_TOO_LARGE;
                return error;
            }
            strcpy(base, path);
        }
        return error;
    }
};

/// Class represents Http Response Header.
struct THttpHeader: public THttpBaseHeader {
public:
    i8     accept_ranges;
    i8     squid_error;
    i16    http_status;
    char   location[URL_MAX];
public:
    void Init() {
        THttpBaseHeader::Init();
        accept_ranges = -1;
        squid_error = 0;
        http_status = -1;
        location[0] = 0;
    }

    void Print() const {
        THttpBaseHeader::Print();
        printf("http_status: %"PRIi16"\n", http_status);
        printf("squid_error: %"PRIi8"\n", squid_error);
        printf("accept_ranges: %"PRIi8"\n", accept_ranges);
        printf("location: \"%s\"\n", location);
    }
};

struct THttpRequestHeader: public THttpBaseHeader {
public:
    char request_uri[URL_MAX];
    char host[HOST_MAX];
    char from[MAXWORD_LEN];
    char user_agent[MAXWORD_LEN];
    char x_yandex_sourcename[MAXWORD_LEN];
    i8   http_method;
    i8   x_yandex_request_priority;
    i32  x_yandex_response_timeout;
    i32  max_age;
    i64  if_modified_since;
public:
    THttpRequestHeader() {
        Init();
    }

    void Init() {
        request_uri[0] = 0;
        host[0] = 0;
        from[0] = 0;
        user_agent[0] = 0;
        x_yandex_sourcename[0] = 0;
        http_method = HTTP_METHOD_UNDEFINED;
        x_yandex_request_priority = DEFAULT_REQUEST_PRIORITY;
        x_yandex_response_timeout = DEFAULT_RESPONSE_TIMEOUT;
        max_age = DEFAULT_MAX_AGE;
        if_modified_since = DEFAULT_IF_MODIFIED_SINCE;
        THttpBaseHeader::Init();
    }

    void Print() const {
        THttpBaseHeader::Print();
        printf("request_uri: \"%s\"\n", request_uri);
        printf("host: \"%s\"\n", host);
        printf("from: \"%s\"\n", from);
        printf("user_agent: \"%s\"\n", user_agent);
        printf("http_method: %"PRIi8"\n", http_method);
        printf("response_timeout: %"PRIi32"\n", x_yandex_response_timeout);
        printf("max_age: %"PRIi32"\n", max_age);
        printf("if_modified_since: %"PRIi64"\n", if_modified_since);
    }

    /// It doesn't care about errors in request or headers, where
    /// request_uri equals to '*'.
    /// This returns copy of the string, which you have to delete.
    Stroka GetUrl() {
        Stroka url;
        if (host[0] == 0 || !strcmp(host, "")) {
            url = request_uri;
        } else {
            url = HTTP_PREFIX;
            url += host;
            url += request_uri;
        }
        return url;
    }

    char *GetUrl(char *buffer, size_t size) {
        if (host[0] == 0 || !strcmp(host, "")) {
            strlcpy(buffer, request_uri, size);
        } else {
            snprintf(buffer, size, "http://%s%s", host, request_uri);
        }
        return buffer;
    }
};

class THttpAuthHeader: public THttpHeader {
public:
    char* realm;
    char* nonce;
    char* opaque;
    bool  stale;
    int   algorithm;
    bool  qop_auth;
    bool  use_auth;

    //we do not provide auth-int variant as too heavy
    //bool  qop_auth_int;

    THttpAuthHeader():
        realm     (0),
        nonce     (0),
        opaque    (0),
        stale     (false),
        algorithm (0),
        qop_auth  (false),
        use_auth  (true)
    {
        THttpHeader::Init();
    }

    ~THttpAuthHeader()
    {
        free(realm);
        free(nonce);
        free(opaque);
    }

    void Print() {
        THttpHeader::Print();
        if (use_auth)
        {
            if (realm)
                printf("realm: \"%s\"\n", realm);
            if (nonce)
                printf("nonce: \"%s\"\n", nonce);
            if (opaque)
                printf("opaque: \"%s\"\n", opaque);
            printf("stale: %d\n", stale);
            printf("algorithm: %d\n", algorithm);
            printf("qop_auth: %d\n", qop_auth);
        }
    }
};
