#pragma once

#include <util/logger/log.h>
#include <util/generic/buffer.h>

namespace NSrvKernel {
    class TSystemLog: public TLog {
    public:
        TSystemLog();

        inline TSystemLog(const Stroka& name) {
            Open(name);
        }

        void Open(const Stroka& path);

        template <class T>
        inline TLogElement operator<< (const T& t) {
            TLogElement ret(this);

            LogPrefix(ret);
            ret << t;

            return ret;
        }

    private:
        void LogPrefix(TLogElement& s);

    private:
        TBuffer Buffer_;
    };
}
