#pragma once

#include "defaults.h"

class TFile;
class TFileHandle;
class Stroka;

struct TFileStat {

    ui32 Mode;        /* protection */
    ui32 Uid;          /* user ID of owner */
    ui32 Gid;          /* group ID of owner */

    ui64 NLinks;      /* number of hard links */
    ui64 Size;         /* total size, in bytes */

    time_t ATime;       /* time of last access */
    time_t MTime;       /* time of last modification */
    time_t CTime;       /* time of last status change */

public:
    TFileStat();
    explicit TFileStat(const TFile&);
    explicit TFileStat(const TFileHandle&);
    explicit TFileStat(const Stroka& f);
};
