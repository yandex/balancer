#include "fair.h"
#include "rrobin.h"
#include "robust.h"
#include "hashing.h"
#include "smartrr.h"
#include "weighted.h"
#include "weighted2.h"
#include "pidgroup.h"
#include "iterator.h"

#include <web/httpd/kernel/config.h>

using namespace NModBalancer;

void IBackEnds::PrintStats(TOutputStream& out) const {
    out << "<backends>";
    DoPrintStats(out);
    out << "</backends>";
}

TAutoPtr<IBackEnds> NModBalancer::ConstructBackEnds(const Stroka& type, const TModuleParams& mp) {
    if (type == "fair") {
        return CreateFair(mp);
    }

    if (type == "srr") {
        return CreateSmartRoundRobin(mp);
    }

    if (type == "rr") {
        return CreateRoundRobin(mp);
    }

    if (type == "weighted") {
        return CreateWeighted(mp);
    }

    if (type == "weighted2") {
        return CreateWeighted2(mp);
    }

    if (type == "robust") {
        return CreateRobust(mp);
    }

    if (type == "pidgroup") {
        return CreatePIDGroup(mp);
    }

    if (type == "hashing" || type == "subnet") { // backward compability
        return CreateHashing(mp);
    }

    return 0;
}
