#include "tlsio.h"

#include <util/string/base64.h>
#include <util/stream/file.h>

using namespace NTls;

TCerts NTls::ReadCert(const Stroka& fname) {
    TCerts ret;
    Stroka cur;
    Stroka line;

    TBufferedFileInput fi(fname);

    while (fi.ReadLine(line)) {
        if (line.find("----") == Stroka::npos) {
            cur += line;
        } else if (!!cur) {
            ret.push_back(Base64Decode(cur));
            cur.clear();
        }
    }

    if (ret.empty()) {
        ythrow yexception() << STRINGBUF("can not parse ") << fname;
    }

    return ret;
}
