#pragma once

#include "httpheader.h"

#include <time.h>

#include <util/system/maxlen.h>
#include <util/datetime/parser.h>

struct THttpHeaderParser {
    int Execute(const void *inBuf, size_t len) {
        return execute((unsigned char*)inBuf, (int)len);
    }

    int Init(THttpHeader *h) {
        int ret = Init((THttpBaseHeader*)(h));
        hd = h;
        hd->Init();
        return ret;
    }

    int Init(THttpAuthHeader *h) {
        int ret = Init((THttpHeader*)(h));
        auth_hd = h;
        return ret;
    }
    int Init(THttpRequestHeader *h) {
        int ret = Init((THttpBaseHeader*)(h));
        request_hd = h;
        request_hd->Init();
        return ret;
    }

    THttpHeader *hd;
    long        I;
    int         Dc;
    TDateTimeFields DateTimeFields;
    char        buf[URL_MAX];
    size_t      buflen;
    char       *lastchar;

    THttpAuthHeader *auth_hd;
    THttpRequestHeader *request_hd;

private:
    THttpBaseHeader *base_hd;
    int cs;
private:
    int Init(THttpBaseHeader *header) {
        base_hd = header;
        auth_hd = NULL;
        request_hd = NULL;
        hd = NULL;
        init();
        return 0;
    }

    int execute(unsigned char *inBuf, int len);
    void init();
};

struct THttpChunkParser {
    int Execute(const void *inBuf, int len) {
        return execute((unsigned char*)inBuf, len);
    }

    int Init() {
        init();
        return 0;
    }

    int     chunk_length;
    char   *lastchar;
    long    I;
    int     Dc;
    i64     cnt64;

private:
    int cs;
    int execute(unsigned char *inBuf, int len);
    void init();
};
