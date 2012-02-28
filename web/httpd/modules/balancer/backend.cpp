#include "backend.h"

#include <web/httpd/kernel/iface.h>
#include <web/httpd/kernel/config.h>

using namespace NModBalancer;

TBackEndDescriptor::TBackEndDescriptor(const TModuleParams& mp)
    : Weight_(1.0)
{
    class TParser: public IConfig::IFunc, public TModuleParams {
        public:
            inline TParser(TBackEndDescriptor* parent, const TModuleParams& mp)
                : TModuleParams(mp)
                , Parent_(parent)
            {
                Config->ForEach(this);
            }

            START_PARSE {
                ON_KEY("weight", Parent_->Weight_) {
                    return;
                }

                {
                    Parent_->Module_ = Loader->MustLoad(key, Copy(value->AsSubConfig()));

                    return;
                }
            } END_PARSE

        private:
            TBackEndDescriptor* Parent_;
    };

    TParser parser(this, mp);

    if (!Module_) {
        ythrow TConfigParseError() << "no module configured";
    }
}
