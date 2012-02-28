#include "encode.h"

namespace NUri {
namespace NEncode {

%%{
    machine TEncoder;

    hex = (
        digit >{ HexDigit(fc); } |
        [A-F] >{ HexUpper(fc); } |
        [a-f] >{ HexLower(fc); }
    );

    escaped = ( "%" hex hex )
            > { HexReset(); }
            % { DoHex(); };

    sym = ( (1..127) - "%") >{ DoSym(fc); };

    exsym = (-128..0) >{ DoEx(fc); };

    main := ( escaped | sym | exsym )*;

    write data;
}%%

void TEncoder::ReEncode(const TStringBuf &url)
{
    const char *p  = url.data();
    const char *pe = p + url.length();
    const char *eof = pe;
    int cs;

    %% write init;
    %% write exec;
}

}
}
