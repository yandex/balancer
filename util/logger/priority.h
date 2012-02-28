#pragma once

enum TLogPriority {
    TLOG_EMERG       = 0,   /* system is unusable */
    TLOG_ALERT       = 1,   /* action must be taken immediately */
    TLOG_CRIT        = 2,   /* critical conditions */
    TLOG_ERR         = 3,   /* error conditions */
    TLOG_WARNING     = 4,   /* warning conditions */
    TLOG_NOTICE      = 5,   /* normal but significant condition */
    TLOG_INFO        = 6,   /* informational */
    TLOG_DEBUG       = 7    /* debug-level messages */
};

#define LOG_DEF_PRIORITY TLOG_INFO
