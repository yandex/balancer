#pragma once

#include <util/system/defaults.h>
#include <util/httpcodes.h>

enum ExtHttpCodes {

    // Custom (replace HTTP 200/304)
    EXT_HTTP_MIRRMOVE                  = 2000,
    EXT_HTTP_MANUAL_DELETE             = 2001,
    EXT_HTTP_NOTUSED2                  = 2002,
    EXT_HTTP_NOTUSED3                  = 2003,
    EXT_HTTP_REFRESH                   = 2004,
    EXT_HTTP_NOINDEX                   = 2005,
    EXT_HTTP_BADCODES                  = 2006,
    EXT_HTTP_SITESTAT                  = 2007,
    EXT_HTTP_IOERROR                   = 2008,
    EXT_HTTP_BASEERROR                 = 2009,
    EXT_HTTP_PARSERROR                 = 2010,
    EXT_HTTP_BAD_CHARSET               = 2011,
    EXT_HTTP_BAD_LANGUAGE              = 2012,
    EXT_HTTP_NUMERERROR                = 2013,
    EXT_HTTP_EMPTYDOC                  = 2014,
    EXT_HTTP_HUGEDOC                   = 2015,
    EXT_HTTP_LINKGARBAGE               = 2016,
    EXT_HTTP_EXDUPLICATE               = 2017,
    EXT_HTTP_FILTERED                  = 2018,
    EXT_HTTP_PARSERFAIL                = 2019, // parser crashed (in this case image spider will redownload such document)
    EXT_HTTP_GZIPERROR                 = 2020,
    EXT_HTTP_CLEANPARAM                = 2021,
    EXT_HTTP_MANUAL_DELETE_URL         = 2022,
    EXT_HTTP_CUSTOM_PARTIAL_CONTENT    = 2023,
    EXT_HTTP_EMPTY_RESPONSE            = 2024,
    EXT_HTTP_REL_CANONICAL             = 2025,

    EXT_HTTP_HOSTFILTER                = 3001,
    EXT_HTTP_URLFILTER                 = 3002,
    EXT_HTTP_SUFFIXFILTER              = 3003,
    EXT_HTTP_DOMAINFILTER              = 3004,
    EXT_HTTP_EXTDOMAINFILTER           = 3005,
    EXT_HTTP_PORTFILTER                = 3006,
    EXT_HTTP_MIRROR                    = 3007,
    EXT_HTTP_DEEPDIR                   = 3008,
    EXT_HTTP_DUPDIRS                   = 3009,
    EXT_HTTP_REGEXP                    = 3010,
    EXT_HTTP_OLDDELETED                = 3012,
    EXT_HTTP_PENALTY                   = 3013,
    EXT_HTTP_POLICY                    = 3015,
    EXT_HTTP_TOOOLD                    = 3016,
    EXT_HTTP_GARBAGE                   = 3017,
    EXT_HTTP_FOREIGN                   = 3018,
    EXT_HTTP_EXT_REGEXP                = 3019,
    EXT_HTTP_HOPS                      = 3020,
    EXT_HTTP_SELRANK                   = 3021,
    EXT_HTTP_NOLINKS                   = 3022,
    EXT_HTTP_WRONGMULTILANG            = 3023,
    EXT_HTTP_SOFTMIRRORS               = 3024,
    EXT_HTTP_BIGLEVEL                  = 3025,

    // fast robot codes

    EXT_HTTP_FASTHOPS                  = 4000,
    EXT_HTTP_NODOC                     = 4001,

    EXT_HTTP_MAX
};

enum HttpFlags {
    // connection
    ShouldDisconnect = 1,
    ShouldRetry      = 2,
    CheckRarely      = 4,

    // indexer
    ShouldReindex    = 8,
    ShouldDelete     = 16,
    CheckLocation    = 32,
    CheckLinks       = 64,
    MarkSuspect      = 128,
    CheckVeryRarely  = 256,
    StrangeError     = 512,
    MoveRedir        = 1024,
    CanBeFake        = 2048,
};

const size_t EXT_HTTP_CODE_MAX = 1 << 12;

static inline int Http2Status(int code) {
    extern ui16 *http2status;
    return http2status[code&(EXT_HTTP_CODE_MAX-1)];
}
