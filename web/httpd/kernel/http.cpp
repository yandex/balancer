#include "http.h"

#include <util/stream/output.h>

using namespace NSrvKernel;

namespace {
    #define MACHINE_DATA
    #include "httpfsm.cpp"
}

bool TRequestParseContextHard::InFinalState() const throw () {
    return cs == ParseHttpRequest_first_final;
}

void TRequestParseContextHard::Init() {
    curTok = 0;
    curHdr = 0;

    #define MACHINE_INIT
    #include "httpfsm.cpp"
}

bool TRequestParseContextHard::ParseChunk(TChunkList& lst) {
    while (!lst.Empty()) {
        TChunkPtr cur(lst.PopFront());

        const char* sp = cur->Data();
        const char* p = sp;
        const char* pe = p + cur->Length();

        #define MACHINE_EXEC
        #include "httpfsm.cpp"

        if (cs == ParseHttpRequest_error) {
            ythrow THttpParseError() << "can not parse http request";
        }

        if (curTok) {
            cur->AddPartTo(sp, pe, *curTok);
        }
    }

    return false;
}

bool NSrvKernel::IsHttp(const TChunkList& lst) throw () {
    char ch;

    if (!lst.CopyData(&ch, 1)) {
        return false;
    }

    switch (ch) {
        case 'g':
        case 'G':
        case 'p':
        case 'P':
        case 'h':
        case 'H':
        case 'd':
        case 'D':
        case 't':
        case 'T':
        case 'c':
        case 'C':
        case 'o':
        case 'O':
            return true;
    }

    return false;
}

template <>
void Out<TRequest>(TOutputStream& out, const TRequest& req) {
    TChunkList lst;

    req.BuildTo(lst);
    out << lst;
}
