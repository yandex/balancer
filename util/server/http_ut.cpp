#include "http.h"

#include <library/unittest/registar.h>

#include <util/system/datetime.h>
#include <util/stream/ios.h>
#include <util/generic/cast.h>

class TEchoServer: public THttpServer::ICallBack {
        class TRequest: public TClientRequest {
            public:
                inline TRequest(TEchoServer* parent)
                    : Parent_(parent)
                {
                }

                virtual bool Reply(void* /*tsr*/) {
                    Output() << "HTTP/1.1 200 Ok\r\n\r\n";
                    Output() << Parent_->Res_;
                    Output().Finish();

                    return true;
                }

            private:
                TEchoServer* Parent_;
        };

    public:
        inline TEchoServer(const Stroka& res)
            : Res_(res)
        {
        }

        virtual TClientRequest* CreateClient() {
            return new TRequest(this);
        }

    private:
        Stroka Res_;
};

class THttpServerTest: public TTestBase {
        UNIT_TEST_SUITE(THttpServerTest);
            UNIT_TEST(TestEchoServer)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestEchoServer() {
            Stroka res;

            for (size_t i = 0; i < 4096 * 5; ++i) {
                res += (char)i;
            }

            const ui16 port = 12345 + (MicroSeconds() / 1000) % 10000;

            TEchoServer serverImpl(res);
            THttpServer server(&serverImpl, THttpServer::TOptions(port).EnableKeepAlive(true).EnableCompression(true));

            server.Start();

            TNetworkAddress addr("localhost", port);
            TSocket s(addr, TDuration::Seconds(10));

            Stroka r;

            r += "GET / HTTP/1.1";
            r += "\r\n";
            r += "Host: localhost:" + ToString(port);
            r += "\r\n";
            r += "\r\n";

            {
                TSocketOutput so(s);
                THttpOutput output(&so);

                output.EnableKeepAlive(true);
                output.EnableCompression(true);

                output.Write(~r, +r);
                output.Finish();
            }

            {
                TSocketInput si(s);
                THttpInput input(&si);
                TStringStream ss;
                TransferData(&input, &ss);

                UNIT_ASSERT_EQUAL(ss.Str(), res);
            }

            server.Stop();
        }
};

UNIT_TEST_SUITE_REGISTRATION(THttpServerTest);
