#include "cputimer.h"

#include <util/system/compat.h>
#include <util/system/defaults.h>
#include <util/string/util.h>
#include <util/generic/singleton.h>
#include <util/stream/output.h>

#if defined(_unix_)
#   include <unistd.h>
#   include <sched.h>
#   include <sys/types.h>
#   include <sys/resource.h>
#   include <sys/param.h>
    #if !defined(_cygwin_)
    #   include <sys/sysctl.h>
    // #   include <sys/user.h>
    #endif
#else
#if defined(_win_)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include    <windows.h>
#endif
#endif

TTimer::TTimer(const char* message) {
    Start_ = TInstant::Now();
    Message_ = message;
}

TTimer::~TTimer() {
    Cerr << Message_ << (TInstant::Now() - Start_) << Endl;
}

static ui64 ManuallySetCyclesPerSecond = 0;

struct TCyclesPerSecondHolder {
    ui64 Cycles_;
    TCyclesPerSecondHolder() {
        ui64 before = GetCycleCount();
        // XXX: returns wrong value if sleep takes more then 1s, need calibrate
        usleep(100000);
        Cycles_ = (GetCycleCount() - before)*10;
    }
};

static ui64 GetCyclesPerSecond() {
    if (ManuallySetCyclesPerSecond != 0)
        return ManuallySetCyclesPerSecond;
    else
        return Singleton<TCyclesPerSecondHolder>()->Cycles_;
}

void SetCyclesPerSecond(ui64 cycles) {
    ManuallySetCyclesPerSecond = cycles;
}

ui64 GetCyclesPerMillisecond() {
    return GetCyclesPerSecond() / 1000;
}

TDuration CyclesToDuration(ui64 cycles) {
    return TDuration::MicroSeconds(cycles * 1000000 / GetCyclesPerSecond());
}

ui64 DurationToCycles(TDuration duration) {
    return duration.MicroSeconds() * GetCyclesPerSecond() / 1000000;
}

TPrecisionTimer::TPrecisionTimer(const char* message)
    : Start(GetCycleCount())
    , Message(message)
{
}

TPrecisionTimer::~TPrecisionTimer() {
    Cout << Message << ": " << (double)(GetCycleCount() - Start) << Endl;
}

Stroka FormatCycles(ui64 cycles) {
    ui64 milliseconds = cycles / GetCyclesPerMillisecond();
    ui32 ms = ui32(milliseconds % 1000);
    milliseconds /= 1000;
    ui32 secs = ui32(milliseconds % 60);
    milliseconds /= 60;
    ui32 mins = ui32(milliseconds);
    Stroka result;
    sprintf(result, "%" PRIu32 " m %.2" PRIu32 " s %.3" PRIu32 " ms", mins, secs, ms);
    return result;
}

TFormattedPrecisionTimer::TFormattedPrecisionTimer(const char* message, TOutputStream* out)
    : Message(message)
    , Out(out)
{
    Start = GetCycleCount();
}

TFormattedPrecisionTimer::~TFormattedPrecisionTimer() {
    const ui64 end = GetCycleCount();
    const ui64 diff = end - Start;

    *Out << Message << ": " << diff << " ticks " << FormatCycles(diff) << Endl;
}

TFuncTimer::TFuncTimer(const char* func)
    : Start_(TInstant::Now())
    , Func_(func)
{
    Cerr << "enter " << Func_ << Endl;
}

TFuncTimer::~TFuncTimer() throw () {
    Cerr << "leave " << Func_ << " -> " << (TInstant::Now() - Start_) << Endl;
}

TTimeLogger::TTimeLogger(const Stroka& message, bool verbose)
    : Message(message)
    , Verbose(verbose)
    , OK(false)
    , Begin(time(0))
    , BeginCycles(GetCycleCount())
{
    if (Verbose) {
        fprintf(stderr, "=========================================================\n");
        fprintf(stderr, "%s started: %.24s (%lu) (%d)\n", ~Message, ctime(&Begin), (unsigned long)Begin, (int)getpid());
    }
}

double TTimeLogger::ElapsedTime() const {
    return time(0) - Begin;
}

void TTimeLogger::SetOK() {
    OK = true;
}

TTimeLogger::~TTimeLogger() {
    time_t tim = time(0);
    ui64 endCycles = GetCycleCount();
    if (Verbose) {
        const char* prefix = (OK) ? "" : "!";
        fprintf(stderr, "%s%s ended: %.24s (%lu) (%d) (took %lu, %s)\n", prefix, ~Message, ctime(&tim), (unsigned long)tim, (int)getpid(), (unsigned long)tim - (unsigned long)Begin, ~FormatCycles(endCycles - BeginCycles));
        fprintf(stderr, "%s=========================================================\n", prefix);
    }
}
