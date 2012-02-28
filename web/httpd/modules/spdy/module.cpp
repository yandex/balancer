#include "module.h"
#include "zlib.h"

#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/chunks.h>
#include <web/httpd/kernel/iobase.h>
#include <web/httpd/kernel/split.h>
#include <web/httpd/kernel/null.h>
#include <web/httpd/kernel/queue.h>
#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/precharge.h>
#include <web/httpd/kernel/concat.h>
#include <web/httpd/kernel/iterator.h>

using namespace NSrvKernel;

namespace {
    enum EFrameType {
          SYN_STREAM = 1
        , SYN_REPLY = 2
        , RST_STREAM = 3
        , SETTINGS = 4
        , PING = 6
        , GOAWAY = 7
        , HEADERS = 8
    };

    enum EErrorCode {
          PROTOCOL_ERROR = 1
        , INTERNAL_ERROR = 6
    };

    enum EFlags {
          FLAG_FIN = 0x01
        , FLAG_UNIDIRECTIONAL = 0x02
    };

    struct TProtocolError: public yexception {
        inline TProtocolError(EErrorCode code)
            : Code(code)
        {
        }

        EErrorCode Code;
    };

    //this is just proof of concept, please, do not look here
    struct TSaver {
        inline TSaver(TChunkList& lst)
            : Lst(lst)
        {
        }

        template <class T>
        inline void Save(const T& t) {
            T tmp = HostToInet(t);

            TChunkPtr chunk(NewChunk(sizeof(tmp)));

            chunk->Shrink(sizeof(tmp));

            memcpy(chunk->Data(), &tmp, sizeof(tmp));
            Lst.Push(chunk);
        }

        inline void Save(TChunkList& lst) {
            MoveTo(&lst, &Lst);
        }

        inline void SaveLowerCase(TChunkList& lst) {
            TChunkPtr chunk(NewChunk(lst.FullLength()));

            chunk->Shrink(lst.FullLength());

            char* buf = chunk->Data();
            size_t len = chunk->Length();

            lst.CopyData(buf, len);

            for (size_t i = 0; i < len; ++i) {
                buf[i] = tolower(buf[i]);
            }

            Lst.Push(chunk);
        }

        TChunkList& Lst;
    };

    template <class T, class It>
    static inline T ReadToken(It& it) {
        T t;

        it.Read(&t, sizeof(t));

        return InetToHost(t);
    }

    template <class T, class It>
    static inline void ReadToken(It& it, T& t) {
        t = ReadToken<T>(it);
    }

    struct TControlHeader {
        union {
            ui32 Field1;

            struct {
                ui16 Type : 16;
                ui16 Version : 15;
                bool IsControl : 1;
            };
        };

        union {
            ui32 Field2;

            struct {
                ui32 Length : 24;
                ui8 Flags : 8;
            };
        };

        inline TControlHeader(ui16 type) throw () {
            Field2 = 0;
            Type = type;
            Version = 2;
            IsControl = 1;
        }

        template <class It>
        inline TControlHeader(ui32 field, It& it) {
            Field1 = field;
            ReadToken(it, Field2);
        }

        inline void Save(TChunkList& lst) {
            TSaver saver(lst);

            saver.Save(Field1);
            saver.Save(Field2);
        }
    };

    struct TDataFrame {
        union {
            ui32 Field1;

            struct {
                ui32 StreamID : 31;
                bool IsControl : 1;
            };
        };

        union {
            ui32 Field2;

            struct {
                ui32 Length : 24;
                ui8 Flags : 8;
            };
        };

        TChunkList Data;

        template <class It>
        inline TDataFrame(ui32 field, It& it) {
            Field1 = field;
            ReadToken(it, Field2);
            it.Read(Data, Length);
        }

        inline TDataFrame(ui32 id, TChunkList& data) throw () {
            StreamID = id;
            IsControl = 0;
            Data.Swap(data);
            Flags = 0;
        }

        //FIN frame
        inline TDataFrame(ui32 id) throw () {
            StreamID = id;
            IsControl = 0;
            Flags = FLAG_FIN;
        }

        inline void Save(TChunkList& lst) {
            Length = Data.FullLength();

            {
                TSaver saver(lst);

                saver.Save(Field1);
                saver.Save(Field2);
            }

            MoveTo(&Data, &lst);
        }
    };

    template <class It>
    static inline void ReadString(It& it, TChunkList& lst) {
        ui16 len;

        ReadToken(it, len);
        it.Read(lst, len);
    }

    struct THeadersBlock: public TRequest {
        template <class It>
        inline THeadersBlock(It& it) {
            Parse(it);
        }

        inline THeadersBlock() {
        }

        template <class It>
        inline void Parse(It& it) {
            ui16 fields;

            ReadToken(it, fields);

            TChunkList method, request, version;

            for (size_t i = 0; i < fields; ++i) {
                TChunkList key, value;

                ReadString(it, key);
                ReadString(it, value);

                if (key == STRINGBUF("method")) {
                    method.Swap(value);
                } else if (key == STRINGBUF("url")) {
                    SplitUrl(value, AddHeader(STRINGBUF("host")), request);
                } else if (key == STRINGBUF("version")) {
                    version.Swap(value);
                } else {
                    THeader* hdr = NewHeader();

                    hdr->Key.Swap(key);
                    hdr->Value.Swap(value);
                }
            }

            if (method.Empty()) {
                ythrow TProtocolError(PROTOCOL_ERROR) << "no method";
            }

            if (request.Empty()) {
                ythrow TProtocolError(PROTOCOL_ERROR) << "no request";
            }

            if (version.Empty()) {
                ythrow TProtocolError(PROTOCOL_ERROR) << "no version";
            }

            method.Push(STRINGBUF(" "));
            MoveTo(&request, &method);
            method.Push(STRINGBUF(" "));
            MoveTo(&version, &method);

            Request().Swap(method);
        }

        static inline void SplitUrl(TChunkList& url, TChunkList& host, TChunkList& req) {
            //this is correct, because we parse it from one chunk
            if (url.Size() != 1) {
                return;
            }

            TChunk& chunk = *url.Begin();
            TStringBuf strurl(chunk.Data(), chunk.Length());
            TStringBuf data = strurl.After(':').Skip(2);
            TStringBuf hoststr, reqstr;

            data.Split('/', hoststr, reqstr);

            host.Push(chunk.SubChunk(hoststr.begin(), hoststr.end()));

            if (reqstr.Empty()) {
                req.Push(STRINGBUF("/"));
            } else {
                req.Push(chunk.SubChunk(reqstr.begin() - 1, reqstr.end()));
            }
        }
    };

    struct TSynStreamFrameBase {
        ui32 StreamID;
        ui32 AssocID;

        union {
            ui16 Field3;

            struct {
                ui16 Reserved : 14;
                ui8 Priority : 2;
            };
        };

        template <class It>
        inline TSynStreamFrameBase(It& it) {
            ReadToken(it, StreamID);
            ReadToken(it, AssocID);
            ReadToken(it, Field3);
        }
    };

    struct TSynStreamFrame: public TSynStreamFrameBase {
        THeadersBlock Headers;

        template <class It>
        inline TSynStreamFrame(It& it, const TControlHeader& ctl, TInflate& inf)
            : TSynStreamFrameBase(it)
        {
            TChunkList tmp;

            it.Read(tmp, ctl.Length - 10);
            inf.Decompress(tmp);

            TStreamIterator slave(tmp);

            Headers.Parse(slave);
        }

        static inline ui16 Type() throw () {
            return SYN_STREAM;
        }
    };

    //TODO
    static inline void SplitRequest(TChunkList& req, TChunkList& status, TChunkList& version) {
        TChunkList tmp;

        Split(8, req, version, tmp);
        Split(1, tmp, req, status);
    }

    static inline void SaveHeaders(TRequest& req, TChunkList& lst) {
        SplitRequest(req.Request(), req.AddHeader(STRINGBUF("status")), req.AddHeader(STRINGBUF("version")));

        TSaver saver(lst);

        typedef TRequest::THeaders THeaders;
        THeaders& headers = req.Headers();

        saver.Save((ui16)headers.Size());

        for (THeaders::TIterator it = headers.Begin(); it != headers.End(); ++it) {
            saver.Save((ui16)it->Key.FullLength());
            saver.SaveLowerCase(it->Key);
            saver.Save((ui16)it->Value.FullLength());
            saver.Save(it->Value);
        }
    }

    struct TSynReplyFrame {
        ui32 StreamID;
        TRequest* Headers;
        TDeflate* Deflate;

        inline TSynReplyFrame(ui32 id, TRequest* headers, TDeflate* deflate)
            : StreamID(id)
            , Headers(headers)
            , Deflate(deflate)
        {
        }

        static inline ui16 Type() throw () {
            return SYN_REPLY;
        }

        inline void Save(TChunkList& lst) {
            {
                TSaver saver(lst);

                saver.Save(StreamID);
                saver.Save((ui16)0);
            }

            {
                TChunkList tmp;

                SaveHeaders(*Headers, tmp);
                Deflate->Compress(tmp);

                MoveTo(&tmp, &lst);
            }
        }
    };

    struct TRstStreamFrame {
        ui32 StreamID;
        ui32 StatusCode;

        template <class It>
        inline TRstStreamFrame(It& it) {
            ReadToken(it, StreamID);
            ReadToken(it, StatusCode);
        }

        inline TRstStreamFrame(ui32 id, EErrorCode code) throw ()
            : StreamID(id)
            , StatusCode(code)
        {
        }

        static inline ui16 Type() throw () {
            return RST_STREAM;
        }

        inline void Save(TChunkList& lst) {
            TSaver saver(lst);

            saver.Save(StreamID);
            saver.Save(StatusCode);
        }
    };
}

MODULE(spdy) {
    struct TProto {
        template <class T>
        class TIntMap {
            typedef yhash<size_t, T> THardMap;
        public:
            inline TIntMap() {
                Zero(T_);
            }

            inline T& operator[] (size_t n) {
                if (n < ARRAY_SIZE(T_)) {
                    return T_[n];
                }

                return Map()[n];
            }

        private:
            inline THardMap& Map() {
                if (!H_) {
                    H_.Reset(new THardMap());
                }

                return *H_;
            }

        private:
            T T_[100];
            TAutoPtr<THardMap> H_;
        };

        struct TConnection: public TSmallObj, public TIntrusiveListItem<TConnection>
                          , public IIoInput, public IIoOutput, public TSynStreamFrame {
            TProto* Parent;

            template <class It>
            inline TConnection(TProto* parent, It& it, const TControlHeader& ctl)
                : TSynStreamFrame(it, ctl, parent->Inflate)
                , Parent(parent)
                , Q_(Parent->Executor()->Create(*this, "spdyjob")->ContPtr()->Executor())
            {
                if (ctl.Flags & FLAG_FIN) {
                    FinishRead();
                }
            }

            inline void operator() (TCont* c) {
                THolder<TConnection> This(this);

                try {
                    Run(c);
                } catch (...) {
                    Parent->SendRst(StreamID, INTERNAL_ERROR);

                    return;
                }

                SendFin();
            }

            inline void Reset() {
                FinishRead();
            }

            inline void OnDataFrame(TDataFrame& frame) {
                Push(frame.Data);

                if (frame.Flags & FLAG_FIN) {
                    FinishRead();
                }
            }

        private:
            inline void Push(TChunkList& lst) {
                Q_.Push(lst);
            }

            inline void FinishRead() {
                Q_.Stop();
            }

            inline void Run(TCont* c) {
                TConnDescr::TProperties props = {
                      c
                    , Parent->Conn->Properties->RemoteAddress
                    , Parent->Conn->Properties->LocalAddress
                    , TInstant::Now()
                };

                const TConnDescr descr = {
                      this
                    , this
                    , &Headers
                    , 0
                    , &props
                };

                Parent->Parent->S_->Run(descr);
            }

            virtual void DoSend(TChunkList& lst) {
                if (C_.HeadersParsed()) {
                    OnData(lst);
                } else {
                    if (C_.ParseChunk(lst)) {
                        OnHeaders(C_);
                        OnData(lst);
                    }
                }
            }

            inline void SendFin() {
                TDataFrame data(StreamID);

                Parent->Send(data);
            }

            inline void OnData(TChunkList& lst) {
                TDataFrame data(StreamID, lst);

                Parent->Send(data);
            }

            inline void OnHeaders(TRequest& req) {
                TSynReplyFrame reply(StreamID, &req, &Parent->Deflate);

                Parent->Send(reply);
            }

            virtual void DoRecv(TChunkList& lst) {
                Q_.Pop(lst);
            }

        private:
            TChunkQueue Q_;
            TPushParseContext C_;
        };

        inline TProto(TModule* parent, const TConnDescr* conn)
            : Parent(parent)
            , Conn(conn)
        {
        }

        inline TContExecutor* Executor() const throw () {
            return Parent->Executor;
        }

        inline void SendRst(ui32 id, EErrorCode err) {
            TRstStreamFrame rst(id, err);

            Send(rst);
        }

        inline void Send(TDataFrame& data) {
            TChunkList lst;

            data.Save(lst);

            Conn->Output->Send(lst);
        }

        template <class T>
        inline void Send(T& t) {
            TControlHeader hdr(t.Type());
            TChunkList lst;

            t.Save(lst);
            hdr.Length = lst.FullLength();

            TChunkList data;

            hdr.Save(data);
            MoveTo(&lst, &data);

            Conn->Output->Send(data);
        }

        inline void Loop() {
            typedef TIntrusiveList<TConnection> TConnections;
            typedef TIntMap<TConnection*> TConnMap;

            TConnections conns;
            TConnMap connmap;

            TStreamIterator it(Conn->Input);

            while (it.Peek()) {
                const ui32 field = ReadToken<ui32>(it);

                if ((field >> 31)) {
                    TControlHeader ctl(field, it);

                    switch (ctl.Type) {
                        case SYN_STREAM: {
                            ++Parent->Spdy_;

                            THolder<TConnection> conn(new TConnection(this, it, ctl));
                            TConnection*& connptr = connmap[conn->StreamID];

                            if (connptr) {
                                connptr->Reset();
                                SendRst(conn->StreamID, PROTOCOL_ERROR);

                                break;
                            }

                            connptr = conn.Get();
                            conns.PushBack(conn.Release());

                            break;
                        }

                        case RST_STREAM: {
                            TRstStreamFrame rs(it);
                            TConnection*& conn = connmap[rs.StreamID];

                            if (conn) {
                                conn->Reset();
                            }

                            break;
                        }

                        default: {
                            TChunkList tmp;

                            it.Read(tmp, ctl.Length);
                        }
                    }
                } else {
                    TDataFrame data(field, it);
                    TConnection*& conn = connmap[data.StreamID];

                    if (conn) {
                        conn->OnDataFrame(data);
                    } else {
                        SendRst(data.StreamID, PROTOCOL_ERROR);
                    }
                }
            }
        }

        TModule* Parent;
        const TConnDescr* Conn;
        TInflate Inflate;
        TDeflate Deflate;
    };

public:
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
        , Http_(0)
        , Spdy_(0)
    {
        Config->ForEach(this);

        if (!S_) {
            ythrow TConfigParseError() << "no module configured";
        }
    }

    START_PARSE {
        {
            S_.Reset(Loader->MustLoad(key, Copy(value->AsSubConfig())).Release());

            return;
        }
    } END_PARSE

    virtual void DoRun(const TConnDescr& descr) {
        TOwnedInput msg;

        descr.Input->Recv(msg);

        if (msg.Empty()) {
            ythrow THttpParseError() << STRINGBUF("empty request");
        }

        TConcatInput input(&msg, descr.Input);

        if (IsHttp(msg)) {
            ++Http_;

            S_->Run(descr.Copy(&input));
        } else {
            const TConnDescr d(descr.Copy(&input));

            TProto(this, &d).Loop();
        }
    }

    virtual void DoStats(TOutputStream& out) {
        out << STRINGBUF("<http>") << Http_ << STRINGBUF("</http>");
        out << STRINGBUF("<spdy>") << Spdy_ << STRINGBUF("</spdy>");

        S_->PrintStats(out);
    }

private:
    THolder<IModule> S_;
    ui64 Http_;
    ui64 Spdy_;
};

IModuleHandle* NModSpdy::Handle() {
    return TModule::ModuleHandle();
}
