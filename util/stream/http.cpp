#include "lz.h"
#include "http.h"
#include "zlib.h"
#include "lzma.h"
#include "bzip2.h"
#include "chunk.h"
#include "length.h"

#include <util/system/yassert.h>
#include <util/string/cast.h>
#include <util/string/util.h>
#include <util/string/http.h>
#include <util/generic/stroka.h>
#include <util/network/socket.h>
#include <util/generic/utility.h>
#include <util/generic/hash_set.h>
#include <util/generic/yexception.h>

static inline size_t SuggestBufferSize() {
    return 8192;
}

static inline TStringBuf Trim(const char* b, const char* e) throw () {
    return StripString(TStringBuf(b, e));
}

static inline TStringBuf RmSemiColon(const TStringBuf& s) {
    return s.Before(';');
}

template <class T, size_t N>
class TStreams {
        struct TDelete {
            inline void operator() (T* t) throw () {
                delete t;
            }
        };

        typedef T* TPtr;
    public:
        inline TStreams() throw ()
            : Beg_(T_ + N)
        {
        }

        inline ~TStreams() throw () {
            TDelete f;

            ForEach(f);
        }

        template <class S>
        inline S* Add(S* t) throw () {
            return (S*)AddImpl((T*)t);
        }

        template <class Functor>
        inline void ForEach(Functor& f) {
            const TPtr* end = T_ + N;

            for (TPtr* cur = Beg_; cur != end; ++cur) {
                f(*cur);
            }
        }

        TPtr Top() {
            const TPtr* end = T_ + N;
            return end == Beg_ ? NULL : *Beg_;
        }

    private:
        inline T* AddImpl(T* t) throw () {
            YASSERT(Beg_ > T_);

            return (*--Beg_ = t);
        }

    private:
        TPtr T_[N];
        TPtr* Beg_;
};

class THttpInput::TImpl {
        typedef yhash_set<Stroka> TAcceptCodings;
    public:
        inline TImpl(TInputStream* slave)
            : Slave_(slave)
            , Buffered_(Slave_, SuggestBufferSize())
            , Input_(0)
            , FirstLine_(Buffered_.ReadLine())
            , Headers_(&Buffered_)
            , KeepAlive_(false)
        {
            BuildInputChain();
            YASSERT(Input_);
        }

        inline ~TImpl() throw () {
        }

        inline size_t Read(void* buf, size_t len) {
            return Input_->Read(buf, len);
        }

        inline const Stroka& FirstLine() const throw () {
            return FirstLine_;
        }

        inline const THttpHeaders& Headers() const throw () {
            return Headers_;
        }

        inline bool IsKeepAlive() const throw () {
            return KeepAlive_;
        }

        inline bool AcceptEncoding(const Stroka& s) const {
            return Codings_.find(to_lower(s)) != Codings_.end();
        }

    private:
        struct TParsedHeaders {
            inline TParsedHeaders() throw ()
                : chunked(false)
                , keepalv(false)
                , lzipped(0)
            {
            }

            bool chunked;
            bool keepalv;
            char lzipped;
        };

        struct TTrEnc {
            inline void operator() (const TStringBuf& s) {
                if (s == STRINGBUF("chunked")) {
                    p->chunked = true;
                }
            }

            TParsedHeaders* p;
        };

        struct TContEnc {
            inline void operator() (const TStringBuf& s) {
                if (s == STRINGBUF("gzip") || s == STRINGBUF("x-gzip")) {
                    p->lzipped = 4;
                } else if (s == STRINGBUF("deflate") || s == STRINGBUF("x-deflate")) {
                    p->lzipped = 5;
                } else if (s == STRINGBUF("y-lzo")) {
                    p->lzipped = 1;
                } else if (s == STRINGBUF("y-lzf")) {
                    p->lzipped = 2;
                } else if (s == STRINGBUF("y-lzq")) {
                    p->lzipped = 3;
                } else if (s == STRINGBUF("y-bzip2")) {
                    p->lzipped = 6;
                } else if (s == STRINGBUF("y-lzma")) {
                    p->lzipped = 7;
                }
            }

            TParsedHeaders* p;
        };

        struct TAccCoding {
            inline void operator() (const TStringBuf& s) {
                c->insert(ToString(s));
            }

            TAcceptCodings* c;
        };

        template <class Functor>
        inline void ForEach(Stroka in, Functor& f) {
            in.to_lower();

            const char* b = in.begin();
            const char* c = b;
            const char* e = in.end();

            while (c != e) {
                if (*c == ',') {
                    f(RmSemiColon(Trim(b, c)));
                    b = c + 1;
                }

                ++c;
            }

            if (b != c) {
                f(RmSemiColon(Trim(b, c)));
            }
        }

        inline void BuildInputChain() {
            TParsedHeaders p;

            ui64 content_length = 0;
            bool have_content_length = false;

            size_t pos = FirstLine_.rfind(' ');
            if (pos != Stroka::npos && strcmp(FirstLine_.c_str() + pos + 1, "HTTP/1.1") == 0) {
                // In HTTP/1.1 Keep-Alive is turned on by default
                p.keepalv = true;
            }

            for (THttpHeaders::TConstIterator h = Headers_.Begin(); h != Headers_.End(); ++h) {
                const THttpInputHeader& header = *h;
                const Stroka hl = to_lower(header.Name());

                if (hl == "transfer-encoding") {
                    TTrEnc f = {&p};

                    ForEach(header.Value(), f);
                } else if (hl == "content-encoding") {
                    TContEnc f = {&p};

                    ForEach(header.Value(), f);
                } else if (hl == "connection") {
                    // accept header "Connection: Keep-Alive, TE"
                    if (strnicmp(~header.Value(), "keep-alive", 10) == 0) {
                        p.keepalv = true;
                    } else if (stricmp(~header.Value(), "close") == 0) {
                        p.keepalv = false;
                    }
                } else if (hl == "accept-encoding") {
                    TAccCoding f = {&Codings_};

                    ForEach(header.Value(), f);
                } else if (hl == "content-length") {
                    content_length = FromString<ui64>(header.Value());
                    have_content_length = true;
                }
            }

            if (p.chunked) {
                Input_ = Streams_.Add(new TChunkedInput(&Buffered_));
            } else {
                Buffered_.Reset(&Cnull);
                Input_ = Streams_.Add(new TMultiInput(&Buffered_, Slave_));

                if (strnicmp(~FirstLine(), "get", 3) == 0 || have_content_length) {
                    /*
                     * TODO - we have other cases
                     */

                    Input_ = Streams_.Add(new TLengthLimitedInput(Input_, content_length));
                }
            }

            if (p.lzipped) {
                if (p.lzipped == 1) {
                    Input_ = Streams_.Add(new TLzoDecompress(Input_));
                } else if (p.lzipped == 2) {
                    Input_ = Streams_.Add(new TLzfDecompress(Input_));
                } else if (p.lzipped == 3) {
                    Input_ = Streams_.Add(new TLzqDecompress(Input_));
                } else if (p.lzipped == 4 || p.lzipped == 5) {
                    Input_ = Streams_.Add(new TZLibDecompress(Input_));
                } else if (p.lzipped == 6) {
                    Input_ = Streams_.Add(new TBZipDecompress(Input_));
                } else if (p.lzipped == 7) {
                    Input_ = Streams_.Add(new TLzmaDecompress(Input_));
                }
            }

            KeepAlive_ = p.keepalv;
        }

    private:
        TInputStream* Slave_;

        /*
         * input helpers
         */
        TBufferedInput Buffered_;
        TStreams<TInputStream, 8> Streams_;

        /*
         * final input stream
         */
        TInputStream* Input_;

        Stroka FirstLine_;
        THttpHeaders Headers_;
        bool KeepAlive_;

        TAcceptCodings Codings_;
};

THttpInput::THttpInput(TInputStream* slave)
    : Impl_(new TImpl(slave))
{
}

THttpInput::~THttpInput() throw () {
}

size_t THttpInput::DoRead(void* buf, size_t len) {
    return Impl_->Read(buf, len);
}

const THttpHeaders& THttpInput::Headers() const throw () {
    return Impl_->Headers();
}

const Stroka& THttpInput::FirstLine() const throw () {
    return Impl_->FirstLine();
}

bool THttpInput::IsKeepAlive() const throw () {
    return Impl_->IsKeepAlive();
}

bool THttpInput::AcceptEncoding(const Stroka& coding) const {
    return Impl_->AcceptEncoding(coding);
}

static const char* best_codings[] = {
    "gzip",
    "deflate",
    "x-gzip",
    "x-deflate",
    "y-lzo",
    "y-lzf",
    "y-lzq",
    "y-bzip2",
    "y-lzma",
};

Stroka THttpInput::BestCompressionScheme() const {
    if (AcceptEncoding("*")) {
        return best_codings[0];
    }

    for (size_t i = 0; i < sizeof(best_codings) / sizeof(*best_codings); ++i) {
        if (AcceptEncoding(best_codings[i])) {
            return best_codings[i];
        }
    }

    return "identity";
}

THttpInputHeader::THttpInputHeader(const Stroka& header) {
    size_t pos = header.find(':');

    if (pos == Stroka::npos) {
        ythrow THttpParseException() << "can not parse http header(" << header.Quote() << ")";
    }

    Name_ = ::ToString(Trim(header.begin(), header.begin() + pos));
    Value_ = ::ToString(Trim(header.begin() + pos + 1, header.end()));
}

THttpInputHeader::THttpInputHeader(const Stroka& name, const Stroka& value)
    : Name_(name)
    , Value_(value)
{
}

THttpInputHeader::~THttpInputHeader() throw () {
}

void THttpInputHeader::OutTo(TOutputStream* stream) const {
    typedef TOutputStream::TPart TPart;

    const TPart parts[] = {
        TPart(Name_),
        TPart(": ", 2),
        TPart(Value_),
        TPart::CrLf(),
    };

    stream->Write(parts, sizeof(parts) / sizeof(*parts));
}

THttpHeaders::THttpHeaders() {
}

THttpHeaders::THttpHeaders(TInputStream* stream) {
    Stroka header;
    Stroka line;

    bool rdOk = stream->ReadLine(header);
    while (rdOk && !header.empty()) {
        rdOk = stream->ReadLine(line);
        if (rdOk && ((line[0] == ' ') || (line[0] == '\t'))) {
            header += line;
        } else {
            AddHeader(header);
            header = line;
        }
    }
}

void THttpHeaders::AddOrReplaceHeader(const THttpInputHeader& header) {
    for (THeaders::iterator h = Headers_.begin(); h != Headers_.end(); ++h) {
        if (stricmp(~h->Name(), ~header.Name()) == 0) {
            *h = header;

            return;
        }
    }

    AddHeader(header);
}

void THttpHeaders::AddHeader(const THttpInputHeader& header) {
    Headers_.push_back(header);
}

THttpHeaders::~THttpHeaders() throw () {
}

void THttpHeaders::OutTo(TOutputStream* stream) const {
    for (TConstIterator header = Begin(); header != End(); ++header) {
        header->OutTo(stream);
    }
}

class THttpOutput::TImpl {
        class TSizeCalculator: public TOutputStream {
            public:
                inline TSizeCalculator() throw ()
                    : Length_(0)
                {
                }

                virtual ~TSizeCalculator() throw () {
                }

                virtual void DoWrite(const void* /*buf*/, size_t len) {
                    Length_ += len;
                }

                inline size_t Length() const throw () {
                    return Length_;
                }

            private:
                size_t Length_;
        };

        enum TState {
            Begin = 0,
            FirstLineSent = 1,
            HeadersSent = 2
        };

        struct TFlush {
            inline void operator() (TOutputStream* s) {
                s->Flush();
            }
        };

        struct TFinish {
            inline void operator() (TOutputStream* s) {
                s->Finish();
            }
        };

    public:
        inline TImpl(TOutputStream* slave, THttpInput* request)
            : Slave_(slave)
            , State_(Begin)
            , Output_(Slave_)
            , Request_(request)
            , Version_(1100)
            , ComprSchemas_(0)
            , ComprSchemasLen_(0)
            , KeepAliveEnabled_(false)
            , Finished_(false)
        {
        }

        inline ~TImpl() throw () {
        }

        inline void Write(const void* buf, size_t len) {
            if (Finished_) {
                ythrow THttpException() << "can not write to finished stream";
            }

            if (State_ == HeadersSent) {
                Output_->Write(buf, len);

                return;
            }

            const char* b = (const char*)buf;
            const char* e = b + len;
            const char* c = b;

            while (c != e) {
                if (*c == '\n') {
                    Line_.append(b, c);

                    if (!Line_.empty() && Line_[Line_.length() - 1] == '\r') {
                        Line_.pop_back();
                    }

                    b = c + 1;

                    Process(Line_);

                    if (State_ == HeadersSent) {
                        Output_->Write(b, e - b);

                        return;
                    }

                    Line_.clear();
                }

                ++c;
            }

            if (b != c) {
                Line_.append(b, c);
            }
        }

        inline void Flush() {
            TOutputStream *top = Streams_.Top();
            top ? top->Flush() : Output_->Flush();
        }

        inline void Finish() {
            if (Finished_) {
                return;
            }

            Finished_ = true;

            TOutputStream *top = Streams_.Top();
            top ? top->Finish() : Output_->Finish();
        }

        inline const THttpHeaders& SentHeaders() const throw () {
            return Headers_;
        }

        inline void EnableCompression(const char** schemas, size_t len) {
            ComprSchemas_ = schemas;
            ComprSchemasLen_ = len;
        }

        inline void EnableKeepAlive(bool enable) {
            KeepAliveEnabled_ = enable;
        }

        inline bool IsCompressionEnabled() const throw () {
            return ComprSchemas_ && ComprSchemasLen_;
        }

        inline bool IsKeepAliveEnabled() const throw () {
            return KeepAliveEnabled_;
        }

        inline bool CanBeKeepAlive() const throw () {
            return SupportChunkedTransfer() && IsKeepAliveEnabled() && (Request_ ? Request_->IsKeepAlive() : true);
        }

    private:
        static inline bool IsResponse(const Stroka& s) throw () {
            return strnicmp(~s, "HTTP/", 5) == 0;
        }

        static inline bool IsRequest(const Stroka& s) throw () {
            return !IsResponse(s);
        }

        inline bool IsHttpRequest() const throw () {
            return IsRequest(FirstLine_);
        }

        inline bool IsHttpResponse() const throw () {
            return IsResponse(FirstLine_);
        }

        inline bool IsHttpPostRequest() const throw () {
            return strnicmp(~FirstLine_, "POST", 4) == 0;
        }

        static inline size_t ParseHttpVersion(const Stroka& s) {
            if (s.empty()) {
                ythrow THttpParseException() << "malformed http stream";
            }

            size_t parsed_version = 0;

            if (IsResponse(s)) {
                const char* b = ~s + 5;

                while (*b && *b != ' ') {
                    if (*b != '.') {
                        parsed_version *= 10;
                        parsed_version += (*b - '0');
                    }

                    ++b;
                }
            } else {
                /*
                 * s not empty here
                 */
                const char* e = s.end() - 1;
                const char* b = s.begin();
                size_t mult = 1;

                while (e != b && *e != '/') {
                    if (*e != '.') {
                        parsed_version += (*e - '0') * mult;
                        mult *= 10;
                    }

                    --e;
                }
            }

            return parsed_version * 100;
        }

        inline void ParseHttpVersion() {
            size_t parsed_version = ParseHttpVersion(FirstLine_);

            if (Request_) {
                parsed_version = Min(parsed_version, ParseHttpVersion(Request_->FirstLine()));
            }

            Version_ = parsed_version;
        }

        inline void Process(const Stroka& s) {
            YASSERT(State_ != HeadersSent);

            if (State_ == Begin) {
                FirstLine_ = s;
                ParseHttpVersion();
                State_ = FirstLineSent;
            } else {
                if (s.empty()) {
                    BuildOutputStream();
                    WriteCached();
                    State_ = HeadersSent;
                } else {
                    AddHeader(s);
                }
            }
        }

        inline void WriteCachedImpl(TOutputStream* s) const {
            s->Write(~FirstLine_, +FirstLine_);
            s->Write("\r\n", 2);
            Headers_.OutTo(s);
            s->Write("\r\n", 2);
            s->Finish();
        }

        inline void WriteCached() {
            size_t buflen = 0;

            {
                TSizeCalculator out;

                WriteCachedImpl(&out);
                buflen = out.Length();
            }

            {
                TBufferedOutput out(Slave_, buflen);

                WriteCachedImpl(&out);
            }

            if (IsHttpRequest() && !IsHttpPostRequest()) {
                /*
                 * if this is http request, then send it now
                 */

                Slave_->Flush();
            }
        }

        inline bool SupportChunkedTransfer() const throw () {
            return Version_ >= 1100;
        }

        inline void BuildOutputStream() {
            if (CanBeKeepAlive()) {
                AddOrReplaceHeader(THttpInputHeader("Connection", "Keep-Alive"));
            } else {
                AddOrReplaceHeader(THttpInputHeader("Connection", "Close"));
            }

            if (IsHttpResponse()) {
                if (IsCompressionEnabled()) {
                    Stroka scheme = Request_->BestCompressionScheme();
                    if (scheme != "identity") {
                        AddOrReplaceHeader(THttpInputHeader("Content-Encoding", scheme));
                    }
                }

                RebuildStream();
            } else if (IsHttpPostRequest()) {
                RebuildStream();
            } else {
                if (IsCompressionEnabled()) {
                    AddOrReplaceHeader(THttpInputHeader("Accept-Encoding", BuildAcceptEncoding()));
                }
            }
        }

        inline Stroka BuildAcceptEncoding() const {
            Stroka ret;

            for (size_t i = 0; i < ComprSchemasLen_; ++i) {
                if (!!ret) {
                    ret += ", ";
                }

                ret += ComprSchemas_[i];
            }

            return ret;
        }

        template<class TStream>
        class TLazy: public TOutputStream {
            public:
                TLazy(TOutputStream* out, ui16 bs)
                    : Output_(out)
                    , BlockSize_(bs)
                    , Slave_(0)
                {
                }

                virtual void DoWrite(const void* buf, size_t len) {
                    ConstructSlave();
                    Slave_->Write(buf, len);
                }

                virtual void DoFlush() {
                    ConstructSlave();
                    Slave_->Flush();
                }

                virtual void DoFinish() {
                    ConstructSlave();
                    Slave_->Finish();
                }

            private:
                inline void ConstructSlave() {
                    if (!Slave_) {
                        Slave_.Reset(new TStream(Output_, BlockSize_));
                    }
                }

            private:
                TOutputStream* Output_;
                ui16 BlockSize_;
                THolder<TOutputStream> Slave_;
        };

        inline void RebuildStream() {
            bool keepalv = false;
            char lzipped = 0;
            bool chunked = false;
            bool have_content_length = false;

            for (THttpHeaders::TConstIterator h = Headers_.Begin(); h != Headers_.End(); ++h) {
                const THttpInputHeader& header = *h;
                const Stroka hl = to_lower(header.Name());

                if (hl == "connection") {
                    keepalv = to_lower(header.Value()) == "keep-alive";
                } else if (hl == "content-encoding") {
                    const Stroka value = to_lower(header.Value());

                    if (value == "gzip" || value == "x-gzip") {
                        lzipped = 4;
                    } else if (value == "deflate" || value == "x-deflate") {
                        lzipped = 5;
                    } else if (value == "y-lzo") {
                        lzipped = 1;
                    } else if (value == "y-lzf") {
                        lzipped = 2;
                    } else if (value == "y-lzq") {
                        lzipped = 3;
                    } else if (value == "y-bzip2") {
                        lzipped = 6;
                    } else if (value == "y-lzma") {
                        lzipped = 7;
                    }
                } else if (hl == "transfer-encoding") {
                    chunked = to_lower(header.Value()) == "chunked";
                } else if (hl == "content-length") {
                    have_content_length = true;
                }
            }

            if (!have_content_length && !chunked && SupportChunkedTransfer() && (keepalv || lzipped)) {
                AddHeader(THttpInputHeader("Transfer-Encoding", "chunked"));
                chunked = true;
            }
            if (chunked) {
                Output_ = Streams_.Add(new TChunkedOutput(Output_));
            }

            if (lzipped) {
                const ui16 bs = 32 * 1024;

                if (lzipped == 1) {
                    Output_ = Streams_.Add(new TLazy<TLzoCompress>(Output_, bs));
                } else if (lzipped == 2) {
                    Output_ = Streams_.Add(new TLazy<TLzfCompress>(Output_, bs));
                } else if (lzipped == 3) {
                    Output_ = Streams_.Add(new TLazy<TLzqCompress>(Output_, bs));
                } else if (lzipped == 4) {
                    Output_ = Streams_.Add(new TZLibCompress(Output_, ZLib::GZip));
                } else if (lzipped == 5) {
                    Output_ = Streams_.Add(new TZLibCompress(Output_, ZLib::ZLib));
                } else if (lzipped == 6) {
                    Output_ = Streams_.Add(new TBZipCompress(Output_));
                } else if (lzipped == 7) {
                    Output_ = Streams_.Add(new TLzmaCompress(Output_));
                }
            }
        }

        inline void AddHeader(const THttpInputHeader& hdr) {
            Headers_.AddHeader(hdr);
        }

        inline void AddOrReplaceHeader(const THttpInputHeader& hdr) {
            Headers_.AddOrReplaceHeader(hdr);
        }

    private:
        TOutputStream* Slave_;
        TState State_;
        TOutputStream* Output_;
        TStreams<TOutputStream, 8> Streams_;
        Stroka Line_;
        Stroka FirstLine_;
        THttpHeaders Headers_;
        THttpInput* Request_;
        size_t Version_;

        const char** ComprSchemas_;
        size_t ComprSchemasLen_;

        bool KeepAliveEnabled_;

        bool Finished_;
};

THttpOutput::THttpOutput(TOutputStream* slave)
    : Impl_(new TImpl(slave, 0))
{
}

THttpOutput::THttpOutput(TOutputStream* slave, THttpInput* request)
    : Impl_(new TImpl(slave, request))
{
}

THttpOutput::~THttpOutput() throw () {
    try {
        Finish();
    } catch (...) {
    }
}

void THttpOutput::DoWrite(const void* buf, size_t len) {
    Impl_->Write(buf, len);
}

void THttpOutput::DoFlush() {
    Impl_->Flush();
}

void THttpOutput::DoFinish() {
    Impl_->Finish();
}

const THttpHeaders& THttpOutput::SentHeaders() const throw () {
    return Impl_->SentHeaders();
}

void THttpOutput::EnableCompression(bool enable) {
    if (enable) {
        EnableCompression(best_codings, ARRAY_SIZE(best_codings));
    } else {
        EnableCompression(0, 0);
    }
}

void THttpOutput::EnableCompression(const char** schemas, size_t count) {
    Impl_->EnableCompression(schemas, count);
}

void THttpOutput::EnableKeepAlive(bool enable) {
    Impl_->EnableKeepAlive(enable);
}

bool THttpOutput::IsKeepAliveEnabled() const throw () {
    return Impl_->IsKeepAliveEnabled();
}

bool THttpOutput::IsCompressionEnabled() const throw () {
    return Impl_->IsCompressionEnabled();
}

bool THttpOutput::CanBeKeepAlive() const throw () {
    return Impl_->CanBeKeepAlive();
}

class THttpServerConn::TImpl {
    public:
        inline TImpl(const TSocket& s)
            : S_(s)
            , SI_(S_)
            , SO_(S_)
            , BO_(&SO_, s.MaximumTransferUnit())
            , HI_(&SI_)
            , HO_(&BO_, &HI_)
        {
        }

        inline ~TImpl() throw () {
        }

        inline THttpInput* Input() throw () {
            return &HI_;
        }

        inline THttpOutput* Output() throw () {
            return &HO_;
        }

    private:
        TSocket S_;
        TSocketInput SI_;
        TSocketOutput SO_;
        TBufferedOutput BO_;
        THttpInput HI_;
        THttpOutput HO_;
};

THttpServerConn::THttpServerConn(const TSocket& s)
    : Impl_(new TImpl(s))
{
}

THttpServerConn::~THttpServerConn() throw () {
}

THttpInput* THttpServerConn::Input() throw () {
    return Impl_->Input();
}

THttpOutput* THttpServerConn::Output() throw () {
    return Impl_->Output();
}

unsigned ParseHttpRetCode(const TStringBuf& ret) {
    const TStringBuf code = StripString(StripString(ret.After(' ')).Before(' '));

    return FromString<unsigned>(~code, +code);
}

void SendMinimalHttpRequest(TSocket& s, const TStringBuf& host, const TStringBuf& request, const TStringBuf& agent, const TStringBuf& from) {
    TSocketOutput so(s);
    THttpOutput output(&so);

    output.EnableKeepAlive(false);
    output.EnableCompression(false);

    const TOutputStream::TPart parts[] = {
        TOutputStream::TPart(STRINGBUF("GET ")),
        TOutputStream::TPart(request),
        TOutputStream::TPart(STRINGBUF(" HTTP/1.1")),
        TOutputStream::TPart::CrLf(),
        TOutputStream::TPart(STRINGBUF("Host: ")),
        TOutputStream::TPart(host),
        TOutputStream::TPart::CrLf(),
        TOutputStream::TPart(STRINGBUF("User-Agent: ")),
        TOutputStream::TPart(agent),
        TOutputStream::TPart::CrLf(),
        TOutputStream::TPart(STRINGBUF("From: ")),
        TOutputStream::TPart(from),
        TOutputStream::TPart::CrLf(),
        TOutputStream::TPart::CrLf()
    };

    output.Write(parts, sizeof(parts) / sizeof(*parts));
    output.Finish();
}
