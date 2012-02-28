#include "module.h"
#include "tlsio.h"

#include <web/httpd/kernel/http.h>
#include <web/httpd/kernel/module.h>
#include <web/httpd/kernel/config.h>
#include <web/httpd/kernel/concat.h>
#include <web/httpd/kernel/precharge.h>

using namespace NTls;
using namespace NSrvKernel;

MODULE(tls) {
    inline TModule(const TModuleParams& mp)
        : TModuleParams(mp)
    {
        Config->ForEach(this);

        if (Cert_.empty()) {
            ythrow TConfigParseError() << "no public keys loaded";
        }

        if (Priv_.empty()) {
            ythrow TConfigParseError() << "no private keys loaded";
        }

        Stroka ca;

        if (!CA_.empty()) {
            ca = CA_[0];
        }

        Keys_.Reset(new TKeys(Cert_[0], Priv_[0], ca));
    }

    START_PARSE {
        Stroka s;

        ON_KEY("cert", s) {
            Cert_ = ReadCert(s);

            return;
        }

        ON_KEY("priv", s) {
            Priv_ = ReadCert(s);

            return;
        }

        ON_KEY("ca", s) {
            CA_ = ReadCert(s);

            return;
        }

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
            S_->Run(descr.Copy(&input));
        } else {
            RunEncrypted(descr.Copy(&input));
        }
    }

    void RunEncrypted(const TConnDescr& descr) {
        TEncryptedSocket conn(*Keys_, descr.Input, descr.Output);

        S_->Run(descr.Copy(&conn, &conn));

        conn.Finish();
    }

    virtual void DoStats(TOutputStream& out) {
        S_->PrintStats(out);
    }

private:
    TInit Tls_;
    TCerts Cert_;
    TCerts Priv_;
    TCerts CA_;
    THolder<TKeys> Keys_;
    THolder<IModule> S_;
};

IModuleHandle* NModTls::Handle() {
    return TModule::ModuleHandle();
}
