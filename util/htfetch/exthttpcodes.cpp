#include "exthttpcodes.h"

#include <cstring>

const ui16 CrazyServer = ShouldDelete|CheckRarely|MarkSuspect;

struct http_flag { ui16 http; ui16 flag; };
static http_flag HTTP_FLAG[] = {
{HTTP_CONTINUE                     , MarkSuspect },                      // 100
{HTTP_SWITCHING_PROTOCOLS          , CrazyServer },                      // 101

{HTTP_OK                           , ShouldReindex },                    // 200
{HTTP_CREATED                      , CrazyServer },                      // 201
{HTTP_ACCEPTED                     , ShouldDelete },                     // 202
{HTTP_NON_AUTHORITATIVE_INFORMATION, ShouldReindex },                    // 203
{HTTP_NO_CONTENT                   , ShouldDelete|CheckVeryRarely },     // 204
{HTTP_RESET_CONTENT                , ShouldDelete|CheckVeryRarely },     // 205
{HTTP_PARTIAL_CONTENT              , ShouldReindex },                    // 206

{HTTP_MULTIPLE_CHOICES             , CheckLinks|ShouldDelete|CheckVeryRarely },              // 300
{HTTP_MOVED_PERMANENTLY            , CheckLocation|ShouldDelete|CheckVeryRarely|MoveRedir }, // 301
{HTTP_FOUND                        , CheckLocation|ShouldDelete|CheckVeryRarely|MoveRedir }, // 302
{HTTP_SEE_OTHER                    , CheckLocation|ShouldDelete|MoveRedir|MoveRedir },       // 303
{HTTP_NOT_MODIFIED                 , 0 },                                                    // 304
{HTTP_USE_PROXY                    , ShouldDelete|CheckVeryRarely },                         // 305
{HTTP_TEMPORARY_REDIRECT           , CheckLocation|ShouldDelete|CheckRarely|MoveRedir },     // 307

{HTTP_BAD_REQUEST                  , CrazyServer },                      // 400
{HTTP_UNAUTHORIZED                 , ShouldDelete|CheckVeryRarely },     // 401
{HTTP_PAYMENT_REQUIRED             , ShouldDelete|CheckVeryRarely },     // 402
{HTTP_FORBIDDEN                    , ShouldDelete|CheckVeryRarely },     // 403
{HTTP_NOT_FOUND                    , ShouldDelete|CheckVeryRarely },     // 404
{HTTP_METHOD_NOT_ALLOWED           , ShouldDelete|CheckVeryRarely },     // 405
{HTTP_NOT_ACCEPTABLE               , ShouldDelete|CheckVeryRarely },     // 406
{HTTP_PROXY_AUTHENTICATION_REQUIRED, CrazyServer },                      // 407
{HTTP_REQUEST_TIME_OUT             , CheckRarely|ShouldDisconnect|ShouldRetry|MarkSuspect }, // 408
{HTTP_CONFLICT                     , MarkSuspect },                      // 409
{HTTP_GONE                         , ShouldDelete|CheckVeryRarely },     // 410
{HTTP_LENGTH_REQUIRED              , CrazyServer },                      // 411
{HTTP_PRECONDITION_FAILED          , CrazyServer },                      // 412
{HTTP_REQUEST_ENTITY_TOO_LARGE     , CrazyServer },                      // 413
{HTTP_REQUEST_URI_TOO_LARGE        , ShouldDelete| CheckVeryRarely },    // 414
{HTTP_UNSUPPORTED_MEDIA_TYPE       , CrazyServer },                      // 415

{HTTP_INTERNAL_SERVER_ERROR        , CheckRarely|MarkSuspect },          // 500
{HTTP_NOT_IMPLEMENTED              , ShouldDelete|ShouldDisconnect|CheckVeryRarely }, // 501
{HTTP_BAD_GATEWAY                  , MarkSuspect },                      // 502
{HTTP_SERVICE_UNAVAILABLE          , ShouldDisconnect|ShouldRetry|MarkSuspect },     // 503
{HTTP_GATEWAY_TIME_OUT             , ShouldDisconnect|ShouldRetry|MarkSuspect },     // 504
{HTTP_HTTP_VERSION_NOT_SUPPORTED   , CrazyServer|ShouldDisconnect },     // 505

// custom
{HTTP_BAD_RESPONSE_HEADER          , CrazyServer|StrangeError },        // 1000
{HTTP_CONNECTION_LOST              , ShouldRetry },                     // 1001
{HTTP_BODY_TOO_LARGE               , ShouldDelete|CheckRarely|CanBeFake },        // 1002
{HTTP_ROBOTS_TXT_DISALLOW          , ShouldDelete|CheckVeryRarely },    // 1003
{HTTP_BAD_URL                      , ShouldDelete|CheckVeryRarely },    // 1004
{HTTP_BAD_MIME                     , ShouldDelete|CheckVeryRarely },    // 1005
{HTTP_DNS_FAILURE                  , ShouldDisconnect|MarkSuspect },    // 1006
{HTTP_BAD_STATUS_CODE              , CrazyServer|StrangeError },        // 1007
{HTTP_BAD_HEADER_STRING            , CrazyServer|StrangeError },        // 1008
{HTTP_BAD_CHUNK                    , CrazyServer|StrangeError },        // 1009
{HTTP_CONNECT_FAILED               , ShouldDisconnect|ShouldRetry|MarkSuspect },    // 1010
{HTTP_FILTER_DISALLOW              , ShouldDelete },                    // 1011
{HTTP_LOCAL_EIO                    , ShouldRetry },                     // 1012
{HTTP_BAD_CONTENT_LENGTH           , ShouldDelete|CheckRarely },        // 1013
{HTTP_BAD_ENCODING                 , ShouldDelete|CheckVeryRarely },    // 1014
{HTTP_LENGTH_UNKNOWN               , ShouldDelete|CheckVeryRarely },    // 1015
{HTTP_HEADER_EOF                   , ShouldRetry|CanBeFake },           // 1016
{HTTP_MESSAGE_EOF                  , ShouldRetry|CanBeFake },           // 1017
{HTTP_CHUNK_EOF                    , ShouldRetry|CanBeFake },           // 1018
{HTTP_PAST_EOF                     , ShouldRetry|ShouldDelete|CheckRarely|CanBeFake },        // 1019
{HTTP_HEADER_TOO_LARGE             , ShouldDelete|CheckVeryRarely },    // 1020
{HTTP_URL_TOO_LARGE                , ShouldDelete|CheckVeryRarely },    // 1021
{HTTP_INTERRUPTED                  , 0 },                               // 1022
{HTTP_CUSTOM_NOT_MODIFIED          , 0 },                               // 1023
{HTTP_BAD_CONTENT_ENCODING         , ShouldDelete|CheckRarely },        // 1024
{HTTP_PROXY_UNKNOWN                , 0 },                               // 1030
{HTTP_PROXY_REQUEST_TIME_OUT       , 0 },                               // 1031
{HTTP_PROXY_INTERNAL_ERROR         , 0 },                               // 1032
{HTTP_PROXY_CONNECT_FAILED         , 0 },                               // 1033
{HTTP_PROXY_CONNECTION_LOST        , 0 },                               // 1034
{HTTP_PROXY_NO_PROXY               , 0 },                               // 1035
{HTTP_PROXY_ERROR                  , 0 },                               // 1036

// Custom (replace HTTP 200/304)
{EXT_HTTP_MIRRMOVE                 , 0 },                               // 2000
{EXT_HTTP_MANUAL_DELETE            , ShouldDelete },                    // 2001
{EXT_HTTP_NOTUSED2                 , ShouldDelete },                    // 2002
{EXT_HTTP_NOTUSED3                 , ShouldDelete },                    // 2003
{EXT_HTTP_REFRESH                  , ShouldDelete|CheckLinks|MoveRedir }, // 2004
{EXT_HTTP_NOINDEX                  , ShouldDelete|CheckLinks },         // 2005
{EXT_HTTP_BADCODES                 , ShouldDelete },                    // 2006
{EXT_HTTP_SITESTAT                 , ShouldDelete },                    // 2007
{EXT_HTTP_IOERROR                  , ShouldDelete },                    // 2008
{EXT_HTTP_BASEERROR                , ShouldDelete },                    // 2009
{EXT_HTTP_PARSERROR                , ShouldDelete|CanBeFake },          // 2010
{EXT_HTTP_BAD_CHARSET              , ShouldDelete|CheckLinks },         // 2011
{EXT_HTTP_BAD_LANGUAGE             , ShouldDelete|CheckLinks },         // 2012
{EXT_HTTP_NUMERERROR               , ShouldDelete },                    // 2013
{EXT_HTTP_EMPTYDOC                 , ShouldDelete|CheckLinks },         // 2014
{EXT_HTTP_HUGEDOC                  , ShouldDelete },                    // 2015
{EXT_HTTP_LINKGARBAGE              , ShouldDelete },                    // 2016
{EXT_HTTP_PARSERFAIL               , ShouldDelete },                    // 2019
{EXT_HTTP_GZIPERROR                , ShouldDelete|CheckRarely },        // 2020
{EXT_HTTP_MANUAL_DELETE_URL        , ShouldDelete },                    // 2022
{EXT_HTTP_CUSTOM_PARTIAL_CONTENT   , ShouldReindex },                   // 2023
{EXT_HTTP_EMPTY_RESPONSE           , ShouldDelete },                    // 2024
{EXT_HTTP_REL_CANONICAL            , ShouldDelete|CheckLinks|MoveRedir }, // 2025
{0,0}
};

static ui16 *prepare_flags(http_flag *arg)
{
    static ui16 flags[EXT_HTTP_CODE_MAX];
    http_flag *ptr;
    size_t i;

    // устанавливаем значение по умолчанию для кодов не перечисленных в таблице выше
    for (i = 0; i < EXT_HTTP_CODE_MAX; ++i)
        flags[i] = CrazyServer;

    // устанавливаем флаги для перечисленных кодов
    for (ptr = arg; ptr->http; ++ptr)
        flags[ptr->http & (EXT_HTTP_CODE_MAX-1)] = ptr->flag;

    // для стандартных кодов ошибок берем флаги из первого кода каждой группы и проставляем их
    // всем кодам не перечисленным в таблице выше
    for(size_t group = 0; group < 1000; group += 100)
        for(size_t j = group + 1; j < group + 100; ++j)
            flags[j] = flags[group];

    // предыдущий цикл затер некоторые флаги перечисленные в таблице выше
    // восстанавливаем их
    for (ptr = arg; ptr->http; ++ptr)
        flags[ptr->http & (EXT_HTTP_CODE_MAX-1)] = ptr->flag;

    return flags;
}

ui16 *http2status = prepare_flags(HTTP_FLAG);
