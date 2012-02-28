#include <util/stream/output.h>
#include <util/string/vector.h>
#include <util/folder/dirut.h>
#include <util/config/last_getopt.h>

void PrintVersionAndExit() {
    Cerr << "some version" << Endl;
    exit(0);
}

static Stroka tmpDir;

int main(int argc, char** argv) {
    int limit;

    using namespace NLastGetopt;
    TOpts opts;
    opts.AddLongOption("version", "print program version").Handler(&PrintVersionAndExit).NoArgument();
    opts.AddHelpOption();
    opts.AddLongOption("tmpdir").StoreResult(&tmpDir).DefaultValue(GetSystemTempDir()).RequiredArgument("DIR");
    opts.AddLongOption("sort").OptionalArgument("COLUMN");
    opts.AddLongOption("homedir").RequiredArgument("DIR").Required();
    opts.AddLongOption("limit").StoreResult(&limit).DefaultValue("10");
    opts.AddLongOption("page-size").DefaultValue("10");

    opts.FreeArgsMin_ = 1;
    opts.FreeArgsMax_ = 2;

    TOptsParseResult res(&opts, argc, argv);

    yvector<Stroka> freeArgs = res.GetFreeArgs();

    ui32 pageSize = res.Get<ui32>("page-size");

    Cerr << "tmpDir:        " << tmpDir << Endl;
    Cerr << "sort:          " << res.GetOrElse("sort", "undefined") << Endl;
    Cerr << "limit:         " << limit << Endl;
    Cerr << "page size:     " << pageSize << Endl;
    Cerr << "free args are: [" << JoinStroku(freeArgs, ", ") << "]" << Endl;

    return 0;
}
