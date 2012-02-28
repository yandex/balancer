LIBRARY()

PEERDIR(
    web/httpd/kernel
    #TODO - refactor common code
    web/httpd/modules/balancer
)

SRCS(
    module.cpp
)

END()
