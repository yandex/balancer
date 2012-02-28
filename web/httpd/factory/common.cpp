#include "common.h"

#include <web/httpd/modules/http/module.h>
#include <web/httpd/modules/admin/module.h>
#include <web/httpd/modules/proxy/module.h>
#include <web/httpd/modules/stats/module.h>
#include <web/httpd/modules/debug/module.h>
#include <web/httpd/modules/ipdisp/module.h>
#include <web/httpd/modules/static/module.h>
#include <web/httpd/modules/regexp/module.h>
#include <web/httpd/modules/pinger/module.h>
#include <web/httpd/modules/headers/module.h>
#include <web/httpd/modules/balancer/module.h>
#include <web/httpd/modules/errorlog/module.h>
#include <web/httpd/modules/accesslog/module.h>
#include <web/httpd/modules/shared/module.h>
#include <web/httpd/modules/tls/module.h>
#include <web/httpd/modules/spdy/module.h>

#include <web/httpd/kernel/main.h>
#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/factory.h>

#include <util/generic/singleton.h>

namespace NModulesFactory {
    class TFactory: public TModuleFactory {
    public:
        inline TFactory() {
            AddHandle(NModBalancer::Handle());
            AddHandle(NModProxy::Handle());
            AddHandle(NModIpDispatch::Handle());
            AddHandle(NModAdmin::Handle());
            AddHandle(NModHeaders::Handle());
            AddHandle(NModStatistics::Handle());
            AddHandle(NModErrorLog::Handle());
            AddHandle(NModDebug::Handle());
            AddHandle(NModStatic::Handle());
            AddHandle(NModRegexp::Handle());
            AddHandle(NModAccessLog::Handle());
            AddHandle(NModHttp::Handle());
            AddHandle(NModPinger::Handle());
            AddHandle(NModShared::Handle());
            AddHandle(NModTls::Handle());
            AddHandle(NModSpdy::Handle());
        }
    };

    IModuleFactory* CommonModules() {
        return Singleton<TFactory>();
    }
}
