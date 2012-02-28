#include <util/generic/stroka.h>
#include <util/generic/yexception.h>
#include <util/draft/dbgdump.h>
#include <library/pire/pire.h>

#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/ucontext.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

void Run(const NPire::TScanner& scanner, NPire::TScanner::State& state, const Stroka& data)
{
    PIRE_IFDEBUG(Cdump << "---run---" << Endl);
    NPire::Run(scanner, state, data.c_str(), data.c_str() + data.size());
}

bool ReadLine(FILE* f, Stroka& str)
{
    int ch;
    str.clear();
    while ((ch = getc(f)) != EOF && ch != '\n')
        str.push_back((char) ch);
    return !str.empty() || ch != EOF;
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2)
            ythrow yexception() << "Usage: echo <string> | pire_dbg <regexp> <options>";
        NPire::TLexer lexer(argv[1], argv[1] + strlen(argv[1]));
        bool surround = false;
        if (argc >= 3)
            for (const char* option = argv[2]; *option; ++option)
                if (*option == 'i')
                    lexer.AddFeature(NPire::NFeatures::CaseInsensitive());
                else if (*option == 'u')
                    lexer.SetEncoding(NPire::NEncodings::Utf8());
                else if (*option == 's')
                    surround = true;
                else if (*option == 'y')
                    lexer.AddFeature(NPire::NFeatures::GlueSimilarGlyphs());
                else if (*option == 'a')
                    lexer.AddFeature(NPire::NFeatures::AndNotSupport());
                else
                    ythrow yexception() << "Unknown option";
        NPire::TFsm fsm = lexer.Parse();
        if (surround)
            fsm.Surround();
        NPire::TScanner scanner(fsm);

        Stroka str;
        while (ReadLine(stdin, str)) {
            NPire::TScanner::State state;
            scanner.Initialize(state);
            if (surround)
                NPire::Step(scanner, state, NPire::BeginMark);
            Run(scanner, state, str);
            if (surround)
                NPire::Step(scanner, state, NPire::EndMark);
            Cout << (scanner.Final(state) ? "ACCEPTS" : "DENIES") << Endl;
        }

        return 0;
    } catch (std::exception& e) {
        Cerr << "pire_dbg: " << e.what() << Endl;
        return 1;
    }
}
