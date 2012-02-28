#pragma once

#include "priority.h"

#include <util/stream/tempbuf.h>

class TLog;

/*
 * better do not use directly
 */
class TLogElement: public TTempBufOutput {
    public:
        TLogElement(TLog* parent);
        ~TLogElement() throw ();

        template <class T>
        inline TLogElement& operator<< (const T& t) {
            static_cast<TOutputStream&>(*this) << t;

            return *this;
        }

        /*
         * for pretty usage: logger << TLOG_ERROR << "Error description";
         */
        inline TLogElement& operator<< (TLogPriority priority) {
            Flush();
            Priority_ = priority;
            return *this;
        }

    private:
        virtual void DoFlush();

    private:
        TLog* Parent_;
        TLogPriority Priority_;
};
