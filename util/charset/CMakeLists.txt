LIBRARY()

ENABLE(NOUTIL)

SRCS(
    codepage.cpp
    cp_encrec.cpp
    doccodes.cpp
    utf.cpp
    wide.cpp

    recyr.hh
    recyr_int.hh

    normalization.cpp
    
    decomposition_table.h
    iconv.h
    unidata.h
)

SRCDIR(generated)

SRCS(
    cp_data.cpp
    composition.cpp
    decomposition.cpp
    encrec_data.cpp
    unidata.cpp
    uniscripts.cpp
)

PEERDIR(
    util/generic
    util/system
    util/memory
    util/stream
    util/string
    util/folder
    util/random
    util/digest
)

END()
