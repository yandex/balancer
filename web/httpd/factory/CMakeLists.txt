LIBRARY()

PEERDIR(
    web/httpd/kernel

    web/httpd/modules/admin
    web/httpd/modules/balancer
    web/httpd/modules/ipdisp
    web/httpd/modules/headers
    web/httpd/modules/proxy
    web/httpd/modules/stats
    web/httpd/modules/errorlog
    web/httpd/modules/debug
    web/httpd/modules/static
    web/httpd/modules/regexp
    web/httpd/modules/accesslog
    web/httpd/modules/http
    web/httpd/modules/pinger
    web/httpd/modules/shared
    web/httpd/modules/tls
    web/httpd/modules/spdy
)

SRCS(
    common.cpp
)

END()
