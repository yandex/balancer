#include "httpcodes.h"

#include <cstring>

const char* HttpCodeStr(int code) {
  switch (code) {
    case HTTP_CONTINUE              : return "Continue";
    case HTTP_SWITCHING_PROTOCOLS   : return "Switching protocols";

    case HTTP_OK                    : return "Ok";
    case HTTP_CREATED               : return "Created";
    case HTTP_ACCEPTED              : return "Accepted";
    case HTTP_NON_AUTHORITATIVE_INFORMATION : return "None authoritative information";
    case HTTP_NO_CONTENT            : return "No content";
    case HTTP_RESET_CONTENT         : return "Reset content";
    case HTTP_PARTIAL_CONTENT       : return "Partial content";

    case HTTP_MULTIPLE_CHOICES        : return "Multiple choices";
    case HTTP_MOVED_PERMANENTLY        : return "Moved permanently";
    case HTTP_FOUND                    : return "Moved temporarily";
    case HTTP_SEE_OTHER             : return "See other";
    case HTTP_NOT_MODIFIED          : return "Not modified";
    case HTTP_USE_PROXY             : return "Use proxy";
    case HTTP_TEMPORARY_REDIRECT    : return "Temporarily redirect";

    case HTTP_BAD_REQUEST           : return "Bad request";
    case HTTP_UNAUTHORIZED          : return "Unauthorized";
    case HTTP_PAYMENT_REQUIRED        : return "Payment required";
    case HTTP_FORBIDDEN             : return "Forbidden";
    case HTTP_NOT_FOUND             : return "Not found";
    case HTTP_METHOD_NOT_ALLOWED    : return "Method not allowed";
    case HTTP_NOT_ACCEPTABLE        : return "Not acceptable";
    case HTTP_PROXY_AUTHENTICATION_REQUIRED : return "Proxy Authentication required";
    case HTTP_REQUEST_TIME_OUT        : return "Request time out";
    case HTTP_CONFLICT              : return "Conflict";
    case HTTP_GONE                  : return "Gone";
    case HTTP_LENGTH_REQUIRED        : return "Length required";
    case HTTP_PRECONDITION_FAILED    : return "Precondition failed";
    case HTTP_REQUEST_ENTITY_TOO_LARGE    : return "Request entity too large";
    case HTTP_REQUEST_URI_TOO_LARGE    : return "Request uri too large";
    case HTTP_UNSUPPORTED_MEDIA_TYPE: return "Unsupported media type";
    case HTTP_REQUESTED_RANGE_NOT_SATISFIABLE: return "Requested Range Not Satisfiable";
    case HTTP_EXPECTATION_FAILED: return "Expectation Failed";
    case HTTP_UNPROCESSABLE_ENTITY: return "Unprocessable Entity";

    case HTTP_INTERNAL_SERVER_ERROR    : return "Internal server error";
    case HTTP_NOT_IMPLEMENTED        : return "Not implemented";
    case HTTP_BAD_GATEWAY           : return "Bad gateway";
    case HTTP_SERVICE_UNAVAILABLE    : return "Service unavailable";
    case HTTP_GATEWAY_TIME_OUT        : return "Gateway time out";
    case HTTP_HTTP_VERSION_NOT_SUPPORTED : return "HTTP version not supported";

    // Custom
    case HTTP_BAD_RESPONSE_HEADER    : return "Bad response header";
    case HTTP_CONNECTION_LOST        : return "Connection lost";
    case HTTP_BODY_TOO_LARGE        : return "Body too large";
    case HTTP_ROBOTS_TXT_DISALLOW    : return "robots.txt disallow";
    case HTTP_BAD_URL               : return "Bad url";
    case HTTP_BAD_MIME              : return "Bad mime type";
    case HTTP_DNS_FAILURE           : return "Dns failure";
    case HTTP_BAD_STATUS_CODE        : return "Bad status code";
    case HTTP_BAD_HEADER_STRING        : return "Bad header string";
    case HTTP_BAD_CHUNK             : return "Bad chunk";
    case HTTP_CONNECT_FAILED        : return "Connect failed";
    case HTTP_FILTER_DISALLOW        : return "Filter disallow";
    case HTTP_LOCAL_EIO             : return "Local eio";
    case HTTP_BAD_CONTENT_LENGTH    : return "Bad content length";
    case HTTP_BAD_ENCODING          : return "Bad encoding";
    case HTTP_LENGTH_UNKNOWN        : return "Length unknown";
    case HTTP_HEADER_EOF            : return "Header EOF";
    case HTTP_MESSAGE_EOF           : return "Message EOF";
    case HTTP_CHUNK_EOF             : return "Chunk EOF";
    case HTTP_PAST_EOF              : return "Past EOF";
    case HTTP_HEADER_TOO_LARGE      : return "Header is too large";
    case HTTP_URL_TOO_LARGE         : return "Url is too large";
    case HTTP_INTERRUPTED           : return "Interrupted";
    case HTTP_CUSTOM_NOT_MODIFIED   : return "Signature detector thinks that doc is not modified";
    case HTTP_BAD_CONTENT_ENCODING  : return "Bad content encoding";
    case HTTP_NO_RESOURCES          : return "No resources";
    case HTTP_FETCHER_SHUTDOWN      : return "Fetcher shutdown";
    case HTTP_CHUNK_TOO_LARGE       : return "Chunk size is too big";
    case HTTP_SERVER_BUSY           : return "Server is busy";
    case HTTP_SERVICE_UNKNOWN       : return "Service is unknown";
    case HTTP_PROXY_UNKNOWN         : return "Zora: unknown error";
    case HTTP_PROXY_REQUEST_TIME_OUT: return "Zora: request time out";
    case HTTP_PROXY_INTERNAL_ERROR  : return "Zora: internal server error";
    case HTTP_PROXY_CONNECT_FAILED  : return "Spider proxy connect failed";
    case HTTP_PROXY_CONNECTION_LOST : return "Spider proxy connection lost";
    case HTTP_PROXY_NO_PROXY        : return "Spider proxy no proxy alive in region";
    case HTTP_PROXY_ERROR           : return "Spider proxy returned custom error";
    default                         : return "Unknown HTTP code";
  }
}
