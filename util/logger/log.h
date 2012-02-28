#pragma once

#include "record.h"
#include "element.h"
#include "backend.h"
#include "priority.h"

#include <cstdarg>

#include <util/generic/ptr.h>

class Stroka;

class TLog {
    public:
        /*
         * construct empty logger
         */
        TLog();

        /*
         * construct file logger
         */
        TLog(const Stroka& fname);

        /*
         * construct any type of logger :)
         */
        TLog(TAutoPtr<TLogBackend> backend);

        ~TLog() throw ();

        /*
         * NOT thread-safe
         */
        void ResetBackend(TAutoPtr<TLogBackend> backend);

        void Write(const char* data, size_t len);
        void Write(TLogPriority priority, const char* data, size_t len);
        void printf_format(2,3) AddLog(const char* format, ...);
        void printf_format(3,4) AddLog(TLogPriority priority, const char* format, ...);
        void ReopenLog();

        /*
         * compat methods, remove in near future...
         */
        bool OpenLog(const char* path);
        bool IsOpen() const throw ();
        void AddLog(const char* format, va_list& lst);
        void CloseLog();

        /*
         * This affects all write methods without priority argument
         */
        void SetDefaultPriority(TLogPriority priority) throw ();
        TLogPriority DefaultPriority() const throw ();

        template <class T>
        inline TLogElement operator<< (const T& t) {
            TLogElement ret(this); ret << t; return ret;
        }

    private:
        class TImpl;
        TSimpleIntrusivePtr<TImpl> Impl_;
};
