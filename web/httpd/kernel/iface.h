#pragma once

#include "log.h"
#include "config.h"
#include "address.h"

#include <util/generic/ptr.h>

class TCont;
class TMemoryPool;
class TOuputStream;
class TContExecutor;
class TSocketHolder;

namespace NSrvKernel {
    class TRequest;
    class IIoInput;
    class IIoOutput;
    class IModuleHandle;
    class IModuleFactory;
    class THttpCommonData;

    class ICtl {
    public:
        inline void ShutDown() throw () {
            DoShutDown();
        }

        inline bool ListenerStarted() throw() {
            return DoListenerStarted();
        }

        inline void PrintStats(TOutputStream& o) {
            DoPrintStats(o);
        }

        inline void StopListener() {
            DoStopListener();
        }

        inline void StartListener() {
            DoStartListener();
        }

        inline void RestartListener() {
            StopListener();
            StartListener();
        }

        inline TSystemLog* DefaultLog() {
            return Log(Stroka());
        }

        inline TSystemLog* Log(const Stroka& name) {
            return DoLog(name);
        }

        inline void ReopenLog() {
            DoReopenLog();
        }

        inline void GetListenAddrs(TOutputStream& o) const {
            DoGetListenAddrs(o);
        }

        inline void GetListenPorts(TOutputStream& o) const {
            DoGetListenPorts(o);
        }

    private:
        virtual void DoShutDown() throw () = 0;
        virtual bool DoListenerStarted() throw() = 0;
        virtual void DoPrintStats(TOutputStream& o) = 0;
        virtual void DoStopListener() = 0;
        virtual void DoStartListener() = 0;
        virtual TSystemLog* DoLog(const Stroka& name) = 0;
        virtual void DoReopenLog() = 0;
        virtual void DoGetListenAddrs(TOutputStream& o) const = 0;
        virtual void DoGetListenPorts(TOutputStream& o) const = 0;
    };

    struct TModuleParams {
        TMemoryPool* Pool;
        IConfig* Config;
        IModuleFactory* Loader;
        ICtl* Control;
        TContExecutor* Executor;

        inline TModuleParams Copy(IConfig* cfg) const throw () {
            TModuleParams ret = *this;
            ret.Config = cfg;
            return ret;
        }
    };

    typedef ui64 TRequestHash;

    struct TConnDescr {
        struct TProperties {
            TCont* Cont;
            const IRemoteAddr* RemoteAddress;
            const IRemoteAddr* LocalAddress;
            TInstant Start;
        };

        IIoInput* Input;
        IIoOutput* Output;
        TRequest* Request;
        TRequestHash Hash;
        TProperties* Properties;

        inline TConnDescr Copy(IIoInput* input) const throw () {
            TConnDescr copy = *this; copy.Input = input; return copy;
        }

        inline TConnDescr CopyIn(IIoInput* input) const throw () {
            return Copy(input);
        }

        inline TConnDescr Copy(IIoOutput* output) const throw () {
            TConnDescr copy = *this; copy.Output = output; return copy;
        }

        inline TConnDescr CopyOut(IIoOutput* output) const throw () {
            return Copy(output);
        }

        inline TConnDescr Copy(TRequest* req) const throw () {
            TConnDescr copy = *this; copy.Request = req; return copy;
        }

        inline TConnDescr Copy(IIoInput* in, IIoOutput* out) const throw () {
            return Copy(in).Copy(out);
        }

        inline TConnDescr Copy(TRequestHash hash) const throw () {
            TConnDescr copy = *this; copy.Hash = hash; return copy;
        }

        inline const IRemoteAddr* RemoteAddr() const throw () {
            return Properties->RemoteAddress;
        }

        inline const IRemoteAddr* LocalAddr() const throw () {
            return Properties->LocalAddress;
        }
    };

    class IModule {
    public:
        virtual ~IModule() {
        }

        inline void Run(const TConnDescr& descr) {
            DoRun(descr);
        }

        void PrintStats(TOutputStream& out);

        inline IModuleHandle* Handle() const throw () {
            return DoHandle();
        }

    private:
        virtual void DoStats(TOutputStream& out);
        virtual void DoRun(const TConnDescr& descr) = 0;
        virtual IModuleHandle* DoHandle() const throw () = 0;
    };

    class IModuleHandle {
    public:
        inline const char* Name() const throw () {
            return DoName();
        }

        inline TAutoPtr<IModule> Construct(const TModuleParams& mp) {
            return DoConstruct(mp);
        }

    private:
        virtual const char* DoName() const throw () = 0;
        virtual IModule* DoConstruct(const TModuleParams& mp) = 0;
    };

    class IModuleFactory {
    public:
        inline TAutoPtr<IModule> Load(const Stroka& name, const TModuleParams& params) {
            IModuleHandle* handle = SelectHandle(name);

            if (!handle) {
                return 0;
            }

            return handle->Construct(params);
        }

        inline TAutoPtr<IModule> MustLoad(const Stroka& name, const TModuleParams& params) {
            TAutoPtr<IModule> module = Load(name, params);

            if (!module) {
                ythrow yexception() << "can not load module(" << name.Quote() << ")";
            }

            return module;
        }

        inline IModuleHandle* SelectHandle(const Stroka& name) {
            return DoSelectHandle(name);
        }

    private:
        virtual IModuleHandle* DoSelectHandle(const Stroka& name) = 0;
    };
}
