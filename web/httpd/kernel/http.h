#pragma once

#include "chunks.h"
#include "iobase.h"
#include "errors.h"

#include <util/memory/pool.h>
#include <util/generic/intrlist.h>

#define CRLF STRINGBUF("\r\n")
#define HDRD STRINGBUF(": ")

namespace NSrvKernel {
    class TRequest;

    class THttpParseError: public THttpError {
    public:
        inline THttpParseError(ui16 code = 400)
            : THttpError(code)
        {
        }
    };

    struct THeader: public TSmallObj, public TIntrusiveListItem<THeader> {
        inline THeader() {
        }

        inline THeader(const THeader& hdr) {
            CopyTo(&hdr.Key, &Key);
            CopyTo(&hdr.Value, &Value);
        }

        TChunkList Key;
        TChunkList Value;
    };

    struct TRequestParseContextHard {
        inline TRequestParseContextHard(TChunkList* unparsed, TRequest* request) throw ()
            : Unparsed(unparsed)
            , Request(request)
        {
            Init();
        }

        TChunkList* Unparsed;
        TRequest* Request;

        bool ParseChunk(TChunkList& lst);
        bool InFinalState() const throw ();

    private:
        //parser internals
        void Init();

        int cs;
        TChunkList* curTok;
        THeader* curHdr;
    };

    struct TRequestParseContext: public TRequestParseContextHard {
        inline TRequestParseContext(IIoInput* in, TChunkList* unparsed, TRequest* request) throw ()
            : TRequestParseContextHard(unparsed, request)
            , In(in)
        {
        }

        inline void Parse() {
            TChunkList lst;

            while (Recv(In, lst)) {
                if (ParseChunk(lst)) {
                    return;
                }
            }

            ythrow THttpParseError() << STRINGBUF("incomplete request");
        }

        IIoInput* In;
    };

    class TRequest {
        friend struct TRequestParseContextHard;
    public:
        typedef TIntrusiveListWithAutoDelete<THeader, TDelete> THeaders;

        inline TRequest() {
        }

        inline TRequest(IIoInput* in, TChunkList& unparsed) {
            Parse(in, unparsed);
        }

        inline TRequest(const TRequest& req) {
            CopyTo(&req.Request(), &Request_);

            {
                const THeaders& h = req.Headers();

                for (THeaders::TConstIterator it = h.Begin(); it != h.End(); ++it) {
                    Headers_.PushBack(new THeader(*it));
                }
            }
        }

        template <class T>
        inline TRequest(const T& req) {
            Request_.Push(req);
        }

        inline TRequest(TChunkList& req) {
            Request_.Swap(req);
        }

        inline void Parse(IIoInput* in, TChunkList& unparsed) {
            TRequestParseContext(in, &unparsed, this).Parse();
        }

        inline void BuildTo(TChunkList& lst) const {
            BuildTo(lst, 0);
        }

        inline void BuildTo(TChunkList& lst, TChunkList* customHeaders) const {
            CopyTo(&Request_, &lst);
            lst.Push(CRLF);

            for (THeaders::TConstIterator it = Headers_.Begin(); it != Headers_.End(); ++it) {
                CopyTo(&it->Key, &lst);
                lst.Push(HDRD);
                CopyTo(&it->Value, &lst);
                lst.Push(CRLF);
            }

            if (customHeaders) {
                MoveTo(customHeaders, &lst);
            }

            lst.Push(CRLF);
        }

        inline const TChunkList& Request() const throw () {
            return Request_;
        }

        inline TChunkList& Request() throw () {
            return Request_;
        }

        inline const THeaders& Headers() const throw () {
            return Headers_;
        }

        inline THeaders& Headers() throw () {
            return Headers_;
        }

        inline THeader* NewHeader() {
            THeader* ret = new THeader();
            Headers_.PushBack(ret);

            return ret;
        }

        template <class T1, class T2>
        inline void AddHeader(const T1& key, const T2& value) {
            AddHeader(key).Push(value);
        }

        template <class T1>
        inline TChunkList& AddHeader(const T1& key) {
            THeader* hdr = NewHeader();

            hdr->Key.Push(key);

            return hdr->Value;
        }

        template <class F>
        inline void ForEachHeader(F& f) {
            Headers_.ForEach(f);
        }

    private:
        TChunkList Request_;
        THeaders Headers_;
    };

    struct TPushParseContext: public TRequest {
        inline TPushParseContext()
            : Ctx(&Unparsed, this)
        {
        }

        inline bool ParseChunk(TChunkList& lst) {
            if (Ctx.ParseChunk(lst)) {
                Unparsed.Swap(lst);

                return true;
            }

            return false;
        }

        inline bool HeadersParsed() const throw () {
            return Ctx.InFinalState();
        }

        TChunkList Unparsed;
        TRequestParseContextHard Ctx;
    };

    class THttpRequestInput: public TChunkList, public IIoInput {
    public:
        inline THttpRequestInput(TRequest* r)
            : R_(r)
        {
            if (R_->Request().Empty()) {
                R_ = 0;
            }
        }

        virtual void DoRecv(TChunkList& lst) {
            if (R_) {
                R_->BuildTo(lst, this);
                R_ = 0;
            }
        }

    private:
        TRequest* R_;
    };

    bool IsHttp(const TChunkList& lst) throw ();
}
