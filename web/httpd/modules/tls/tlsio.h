#pragma once

#include <contrib/libs/matrixssl/matrixSsl.h>

#include <web/httpd/kernel/chunks.h>
#include <web/httpd/kernel/iobase.h>

#include <util/generic/strbuf.h>
#include <util/generic/vector.h>
#include <util/generic/singleton.h>
#include <util/generic/yexception.h>

namespace NTls {
    using namespace NSrvKernel;

    typedef yvector<Stroka> TCerts;
    TCerts ReadCert(const Stroka& fname);

    class TError: public yexception {
    public:
        inline TError(int32 code)
            : Code_(code)
        {
            (*this) << STRINGBUF("(") << Code_ << STRINGBUF(") ");
        }

        inline int32 Code() const throw () {
            return Code_;
        }

    private:
        int32 Code_;
    };

    struct TKeysDestroy {
        static inline void Destroy(sslKeys_t* keys) throw () {
            matrixSslFreeKeys(keys);
        }
    };

    struct TKeys {
        inline TKeys(TStringBuf cert, TStringBuf priv, TStringBuf ca) {
            sslKeys_t* keys = 0;

            const int32 res = matrixSslReadKeysMem(
                  &keys
                , (unsigned char*)~cert, +cert
                , (unsigned char*)~priv, +priv
                , (unsigned char*)~ca, +ca
            );

            if ((res != SSL_SUCCESS) || !keys) {
                ythrow TError(res) << "can not read keys";
            }

            Keys.Reset(keys);
        }

        inline sslKeys_t* Handle() const throw () {
            return Keys.Get();
        }

        THolder<sslKeys_t, TKeysDestroy> Keys;
    };

    struct TSessionDestroy {
        static inline void Destroy(ssl_t* s) throw () {
            matrixSslDeleteSession(s);
        }
    };

    struct TSession {
        inline TSession(const TKeys& keys) {
            ssl_t* s = 0;

            const int32 res = matrixSslNewSession(&s, keys.Handle(), 0, SSL_FLAGS_SERVER);

            if (res != SSL_SUCCESS || !s) {
                ythrow TError(res) << "can not create session";
            }

            S.Reset(s);
        }

        inline ssl_t* Handle() const throw () {
            return S.Get();
        }

        THolder<ssl_t, TSessionDestroy> S;
    };

    struct TInputBuf: public TChunkPtr, public sslBuf_t {
        inline TInputBuf(TChunkPtr chunk)
            : TChunkPtr(chunk)
        {
            buf = start = (unsigned char*)Get()->Data();
            size = Get()->Length();
            end = start + size;
        }

        inline TChunkPtr Left() const {
            return Get()->SubChunk((const char*)start, (const char*)end);
        }
    };

    struct TOutputBuf: public TChunkPtr, public sslBuf_t {
        inline TOutputBuf(TChunkPtr chunk)
            : TChunkPtr(chunk)
        {
            buf = start = end = (unsigned char*)Get()->Data();
            size = Get()->Length();
        }

        inline TChunkPtr Response() const {
            return Get()->SubChunk((const char*)start, (const char*)end);
        }
    };

    class TEncryptedSocket: public IIoInput, public IIoOutput {
    public:
        inline TEncryptedSocket(const TKeys& keys, IIoInput* in, IIoOutput* out)
            : In_(in)
            , Out_(out)
            , S_(keys)
        {
        }

        virtual void DoRecv(TChunkList& lst) {
            Read(lst);
        }

        virtual void DoSend(TChunkList& lst) {
            TChunkList encoded;

            for (TChunkList::TConstIterator it = lst.Begin(); it != lst.End(); ++it) {
                Encode(it->Data(), it->Length(), encoded);
            }

            Out_->Send(encoded);
        }

        inline void Finish() {
            TChunkList lst;

            EncodeClosure(lst);
            Out_->Send(lst);
        }

    private:
        inline void Read(TChunkList& lst) {
            while (true) {
                TChunkPtr nextIn;

                if (!NextIn(nextIn)) {
                    return;
                }

                TInputBuf in(nextIn);
                TOutputBuf out(NextOut());

                unsigned char error = 0;
                unsigned char alertLevel = 0;
                unsigned char alertDescr = 0;

                const int rc = matrixSslDecode(S_.Handle(), &in, &out, &error, &alertLevel, &alertDescr);

                {
                    TChunkPtr left = in.Left();

                    if (left->Length()) {
                        Readed_.PushFront(left.Release());
                    }
                }

                switch (rc) {
                    case SSL_SUCCESS:
                        continue;

                    case SSL_PROCESS_DATA:
                        lst.Push(out.Response());

                        return;

                    case SSL_SEND_RESPONSE: {
                        TChunkList resp;

                        resp.Push(out.Response());
                        Out_->Send(resp);

                        break;
                    }

                    case SSL_ERROR:
                        ythrow TError(error) << "protocol error";

                    case SSL_ALERT:
                        return;

                    case SSL_PARTIAL:
                        ythrow TError(rc) << "unsupported";

                    case SSL_FULL:
                        ythrow TError(rc) << "unsupported";

                    default:
                        ythrow TError(rc) << "shit happen";
                }
            }
        }

        inline void Encode(const char* buf, size_t len, TChunkList& lst) {
            TOutputBuf out(NextOut());

            const int rc = matrixSslEncode(S_.Handle(), (const unsigned char*)buf, len, &out);

            switch (rc) {
                case SSL_SUCCESS:
                    break;

                case SSL_ERROR:
                case SSL_FULL:
                    ythrow TError(rc) << "encode error";
            }

            lst.Push(out.Response());
        }

        inline void EncodeClosure(TChunkList& lst) {
            TOutputBuf out(NextOut());
            matrixSslEncodeClosureAlert(S_.Handle(), &out);
            lst.Push(out.Response());
        }

        inline bool NextIn(TChunkPtr& in) {
            if (Readed_.Empty()) {
                In_->Recv(Readed_);
            }

            if (Readed_.Empty()) {
                return false;
            }

            in = Readed_.PopFront();

            return true;
        }

        inline TChunkPtr NextOut() {
            return NewChunk();
        }

    private:
        IIoInput* In_;
        IIoOutput* Out_;
        TSession S_;
        TChunkList Readed_;
    };

    struct TInit {
        inline TInit() {
            int32 ret = matrixSslOpen();

            if (ret != SSL_SUCCESS) {
                ythrow TError(ret) << "can not init tls subsystem";
            }
        }

        inline ~TInit() {
            matrixSslClose();
        }
    };
}
