#include <util/generic/buffer.h>
#include <util/generic/yexception.h>
#include <util/generic/singleton.h>

#include <util/system/hostname.h>

#include <util/yarchive.h>
#include <util/stream/ios.h>

static const char data[] = {
    #include "common.inc"
};

class TLuaArchive: public TArchiveReader {
    public:
        inline TLuaArchive()
            : TArchiveReader(TBlob::NoCopy(data, sizeof(data)))
        {
        }
};

static inline TArchiveReader& Archive() {
    return *Singleton<TLuaArchive>();
}

static inline bool OpenInternalModule(const char* key, TBuffer& buf) {
    try {
        TAutoPtr<TInputStream> in(Archive().ObjectByKey(key));
        TBufferOutput out(buf);

        TransferData(in.Get(), &out);

        return true;
    } catch (...) {
    }

    return false;
}

#define luaall_c

#include "lapi.c"
#include "lcode.c"
#include "ldebug.c"
#include "ldo.c"
#include "ldump.c"
#include "lfunc.c"
#include "lgc.c"
#include "llex.c"
#include "lmem.c"
#include "lobject.c"
#include "lopcodes.c"
#include "lparser.c"
#include "lstate.c"
#include "lstring.c"
#include "ltable.c"
#include "ltm.c"
#include "lundump.c"
#include "lvm.c"
#include "lzio.c"

#include "lauxlib.c"
#include "lbaselib.c"
#include "ldblib.c"
#include "liolib.c"

static void RegisterYandexHooks(lua_State* l);

#include "linit.c"
#include "lmathlib.c"
#include "loadlib.c"
#include "loslib.c"
#include "lstrlib.c"
#include "ltablib.c"

static int OsHostName(lua_State* l) {
    lua_pushstring(l, ~HostName());

    return 1;
}

static const luaL_Reg YANDEX_OS_LIBS[] = {
    {"hostname", OsHostName},
    {0, 0}
};

LUALIB_API int RegisterYandexOsHooks(lua_State* l) {
    luaL_register(l, LUA_OSLIBNAME, YANDEX_OS_LIBS);

    return 1;
}

static void RegisterYandexHooks(lua_State* l) {
    lua_pushcfunction(l, RegisterYandexOsHooks);
    lua_pushstring(l, "RegisterYandexOsHooks");
    lua_call(l, 1, 0);
}
