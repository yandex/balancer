#include <web/httpd/kernel/main.h>
#include <web/httpd/kernel/factory.h>
#include <web/httpd/factory/common.h>

#include <util/generic/yexception.h>

using namespace NSrvKernel;
using namespace NModulesFactory;

int main(int argc, char** argv) {
    try {
        RunMain(argc, argv, CommonModules());

        return 0;
    } catch (...) {
        Cerr << CurrentExceptionMessage() << Endl;
    }

    return 1;
}
