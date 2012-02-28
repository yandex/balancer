#pragma once

#include "debug.h"

enum ETraceLevel {
    TRACE_ERR       = 1,
    TRACE_WARN      = 2,
    TRACE_NOTICE    = 3,
    TRACE_INFO      = 4,
    TRACE_DEBUG     = 5,
    TRACE_DETAIL    = 6,
    TRACE_VERBOSE   = 7
};

#if !defined(NDEBUG) && !defined(ENABLE_TRACE)
    #define ENABLE_TRACE
#endif

#ifdef ENABLE_TRACE

#define DBGTRACE(elevel, args) DBGTRACE0(int(TRACE_##elevel), args)
#define DBGTRACE0(level, args) do if ((level) <= StdDbgLevel()) { StdDbgStream() << args << Endl; } while(false)

#else

#define DBGTRACE(elevel, args) do {} while (false)
#define DBGTRACE0(level, args) do {} while (false)

#endif

