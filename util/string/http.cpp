#include "http.h"
#include "util.h"

#include <util/generic/yexception.h>

static inline TStringBuf StripLeft(const TStringBuf& s) throw () {
    const char* b = s.begin();
    const char* e = s.end();

    StripRangeBegin(b, e);

    return TStringBuf(b, e);
}

TParsedHttpRequest::TParsedHttpRequest(const TStringBuf& str) {
    TStringBuf tmp;

    if (!StripLeft(str).SplitImpl(' ', Method, tmp)) {
        ythrow yexception() << "bad request(" << ToString(str).Quote() << ")";
    }

    if (!StripLeft(tmp).SplitImpl(' ', Request, Proto)) {
        ythrow yexception() << "bad request(" << ToString(str).Quote() << ")";
    }

    Proto = StripLeft(Proto);
}

TParsedHttpLocation::TParsedHttpLocation(const TStringBuf& req) {
    req.Split('?', Path, Cgi);
}
