#include "parse.h"

%%{
    machine TParser;

    #================================================
    # RFC 2396 http://tools.ietf.org/html/rfc2396
    # RFC 3986 http://tools.ietf.org/html/rfc3986
    # with some modifications
    #================================================
    # The RegEx
    #
    # http://www.ics.uci.edu/pub/ietf/uri/#Related
    # ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
    #  12            3  4          5       6  7        8 9
    #results in the following subexpression matches:
    # $1 = http:
    # $2 = http
    # $3 = //www.ics.uci.edu
    # $4 = www.ics.uci.edu
    # $5 = /pub/ietf/uri/
    # $6 = <undefined>
    # $7 = <undefined>
    # $8 = #Related
    # $9 = Related
    #
    # So $2:scheme $4:authority $5:path $6:query $7:request
    #================================================

    alphaSym = ( upper >{ REQ(fpc, FeatureToLower) } ) |
                  lower;

    alnumSym = alphaSym | digit;

    #================================================
    # alpha         = lowalpha | upalpha
    #
    # lowalpha = "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" |
    #            "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" |
    #            "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z"
    # upalpha  = "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" |
    #            "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" |
    #            "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
    # digit    = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" |
    #            "8" | "9"
    #MOD: no needs here

    # alphanum      = alpha | digit

    #alphanum = alnum;
    alphanum = alnumSym;

    # hex           = digit | "A" | "B" | "C" | "D" | "E" | "F" |
    #                         "a" | "b" | "c" | "d" | "e" | "f"

    hex = (
        digit > { HexDigit(fpc, fc); }
        | [A-F] >{ HexUpper(fpc, fc); }
        | [a-f] >{ HexLower(fpc, fc); }
    );

    # RFC 3986 ABNFs

    # HexSet sets REQ so must apply in range
    pct_encoded   = ( "%" hex hex ) >{ HexReset(); } %{ HexSet(fpc - 1); };

    unreserved    = alphanum | "-" | "." | "_" | "~";

    # gen_delims  = ":" | "/" | "?" | "#" | "[" | "]" | "@";

    sub_delims    = "!" | "$" | "&" | "(" | ")"
                  | "*" | "+" | "," | ";" | "="
                  | ( ['] >{ REQ(fpc, FeatureEncodeForSQL) } );

    # safe character sequences
    safe          = unreserved | pct_encoded | sub_delims;

    # MOD: Yandex extensions
    ext_ascii     = ( -128..-1 ) >{ REQ(fpc, FeatureEncodeExtendedASCII) };
    ext_delims    = ( "[" | "]" | "|" |  "{" | "}" | "`" | "^"
                  | ( ["\\] >{ REQ(fpc, FeatureEncodeForSQL) } )
                  ) >{ REQ(fpc, FeatureEncodeExtendedDelim) };
    ext_safe      = safe | ext_ascii | ext_delims;

    # pchar
    # uric (RFC 2396)
    # MOD: extension to format, add extended delimiters and 8-bit ascii

    pchar_nc      = ext_safe | "@";
    pchar         = pchar_nc | ":";
    path_sep      = "/";
    uric          = pchar | path_sep | "?";

    fragment      = uric*;
    query         = uric*;

    # path

    path_seg_nc   = pchar_nc+;
    path_segment  = pchar*;
    path_segments = path_segment ( path_sep path_segment )*;

    # port          = *digit

    port          = digit+;

    # IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit

    IPv4address   = digit+ "." digit+ "." digit+ "." digit+;

    # toplabel      = alpha | alpha *( alphanum | "-" ) alphanum

    toplabelAlpha = alphaSym | pct_encoded | ext_ascii;
    toplabelAlnum = toplabelAlpha | digit;
    toplabel      = toplabelAlpha | ( toplabelAlpha ( toplabelAlnum | "-" )* toplabelAlnum );

    # domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum

    domainlabel   = [^/\. \t\r\n\v\f]+;

    # hostname      = *( domainlabel "." ) toplabel [ "." ]

    hostname      = ( domainlabel "." )* toplabel ("."?);

    # host          = ( hostname | IPv4address )

    host          = ( hostname | IPv4address );

    # hostport      = host [ ":" port ]
    ## do not use
    #
    # userinfo      = *( unreserved | escaped |
    #                    ";" | ":" | "&" | "=" | "+" | "$" | "," )
    # MOD: split onto pair of sections: username and password
    ## do not use

    username      = ( ext_safe )*;
    password      = ( ext_safe | ":" )*;

    #
    # server        = [ [ userinfo "@" ] hostport ]
    # reg_name      = 1*( unreserved | escaped | "$" | "," |
    #                     ";" | ":" | "@" | "&" | "=" | "+" )
    # MOD: ignore reg_name
    #
    # authority     = server | reg_name
    # MOD: ignore reg_name
    ## do not use
    #
    # scheme        = alpha *( alpha | digit | "+" | "-" | "." )
    # MOD: alphaSym required only for case insensitivity

    scheme        = ( alphaSym ( alphaSym | digit | "+" | "-" | "." )* );

    #================================================
    # checkPath rules
    #
    # REQ must apply to a char in range but not after the range has been reset

    checkPathHead =
        ( "../" )
            %{ REQ(fpc - 1, FeaturePathOperation) }
        any*;

    checkPathMid =
        any*
        ( "/../" | "/./" | "//" )
            %{ REQ(fpc - 1, FeaturePathOperation) }
        any*;

    checkPathTail =
        any*
        ( "/.." | "/." )
            %{ REQ(fpc - 1, FeaturePathOperation) }
        ;

    checkAbsPath = checkPathMid | checkPathTail | any*;
    checkRelPath = checkPathHead | checkAbsPath;

    #================================================

    abs_path_impl   = path_sep path_segments;
    abs_path        = abs_path_impl & checkAbsPath;

    rel_path_impl   = path_seg_nc abs_path_impl?;
    rel_path        = rel_path_impl & checkRelPath;

    #
    # uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
    #                 "&" | "=" | "+" | "$" | ","
    # opaque_part   = uric_no_slash *uric
    # opaque_part = ( uric - '/' ) uric*;
    # TRF: contradicts to "file:////C/..."
    opaque_part = uric*;

    #================================================
    # Parts that define sections:
    #

    pschm = scheme   >{ BEG(fpc, Scheme) } %{ END(fpc, Scheme) };
    puser = username >{ BEG(fpc, User) }   %{ END(fpc, User) };
    ppass = username >{ BEG(fpc, Pass) }   %{ END(fpc, Pass) };
    phost = host     >{ BEG(fpc, Host) }   %{ END(fpc, Host) };
    pport = port     >{ BEG(fpc, Port) }   %{ END(fpc, Port) };
    pqry  = query    >{ BEG(fpc, Query) }  %{ END(fpc, Query) };
    pfrag = fragment >{ BEG(fpc, Frag) }   %{ END(fpc, Frag) };
    pabsp = abs_path >{ BEG(fpc, Path) }   %{ END(fpc, Path) };
    prelp = rel_path >{ BEG(fpc, Path) }   %{ END(fpc, Path) };

    schemePart = [^:/?#]+ & pschm;

    userinfo = ( puser ( ":" ppass )? )
        ("@" %{ CLR(fpc, Host) } @lerr{ CLR(fpc, User) } )
    ;

    # like authority below, but port, if specified, must be non-empty
    authPartPort1 = [^/?#]* & ( userinfo? phost ( ":" pport )? );

    authorityPart = [^/?#]* & ( userinfo? phost ( ":" pport? )? );

    absPathPart =  ( ( [^?#]* - ("//" any*) ) &
                     pabsp
                   );

    absFullPathPart =  ( ( [^?#]* ) &
                     pabsp
                   );

    relPathPart =  ( [^?#]* &
                     ( ("." "/"+)* prelp )
                   );

    queryPart = ( "?"
                  ( [^#]* &
                    pqry
                  ) );

    fragmentPart = ( "#"
                     pfrag
                   );

    #================================================
    # net_path      = "//" authority [ abs_path ]
    # hier_part     = ( net_path | abs_path ) [ "?" query ]
    # relativeURI   = ( net_path | abs_path | rel_path ) [ "?" query ]
    # absoluteURI   = scheme ":" ( hier_part | opaque_part )
    # URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
    #================================================

    netPATH     = "//"
                  authorityPart
                  absFullPathPart?;
    hierPART    = ( netPATH | absPathPart )
                  queryPart?;
    relativeURI = ( netPATH %{NetPath=true;} |
                    absPathPart |
                    relPathPart |
                    null
                  ) %{HttpMode=true;}
                  queryPart?;
    absoluteURI = schemePart
                  ":"
                  ( hierPART %{HttpMode=true;} |
                    opaque_part);

    hierURIabs = (schemePart ":" netPATH) @lerr{CLR(fpc, Scheme)};

    # excludes relative paths, to make way for schemeless URIs
    hierURI := (
        # fully specified URI
        hierURIabs
        # next two: no scheme, but must prevent host://path
        | authPartPort1 absFullPathPart? # port cannot be empty
        | authorityPart absPathPart? # path cannot start with //
        # //host/path
        | netPATH %{NetPath=true;}
        # /path
        | absPathPart?
        # empty
        | null
        ) queryPart? fragmentPart?
        %{HttpMode=true;}
        $err{return false;}
        ;

    mainURI :=
            ( absoluteURI  |
              relativeURI  )
            fragmentPart?
            ;

    write data;

}%%

namespace NUri {

bool TParser::doParse(const char* str_beg, size_t length)
{
    const char* p = str_beg;
    const char* pe = str_beg + length;
    const char* eof = pe;
    int cs;

#define BEG(ptr, fld) startSection (ptr, TField::Field ## fld);
#define END(ptr, fld) finishSection(ptr, TField::Field ## fld);
#define CLR(ptr, fld) ResetSection (TField::Field ## fld, ptr);
#define REQ(ptr, req) setRequirement(ptr, TFeature :: req);

    %% write init nocs;

    int initcs;
    if (Flags & TFeature::FeatureHierURI)
        initcs = TParser_en_hierURI;
    else
        initcs = TParser_en_mainURI;
    cs = initcs;

    %% write exec;

#undef BEG
#undef END
#undef CLR
#undef REQ

    return (p == pe && cs != initcs && cs != TParser_error);
}

}
