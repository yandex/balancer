#include "iface.h"
#include "luacfg.h"
#include "config.h"

#include <library/lua/wrapper.h>

#include <util/stream/input.h>

namespace NSrvKernel {
    TAutoPtr<IConfig> LuaConfigParser(TInputStream& in) {
        class TConfig: public IConfig, public IConfig::IValue, public TLuaStateHolder {
            public:
                inline TConfig(TInputStream& in) {
                    BootStrap();
                    Load(&in, "config");
                    call(0, 0);

                    lua_getfield(*this, LUA_GLOBALSINDEX, "instance");
                }

                virtual void DoForEach(IFunc* func) {
                    lua_State* L = *this;
                    lua_pushnil(L);
                    while (lua_next(L, -2) != 0) {
                        const Stroka key = CurrentKey();

                        func->Consume(key, this);
                        lua_pop(L, 1);
                    }
                }

                inline Stroka CurrentKey() {
                    if (lua_type(*this, -2) == LUA_TSTRING) {
                        return luaL_checklstring(*this, -2, 0);
                    }

                    if (lua_type(*this, -2) == LUA_TNUMBER) {
                        return ToString(luaL_checknumber(*this, -2));
                    }

                    ythrow TConfigParseError() << "parse error";
                }

                virtual Stroka AsString() {
                    return luaL_checklstring(*this, -1, 0);
                }

                virtual bool AsBool() {
                    luaL_checktype(*this, -1, LUA_TBOOLEAN);
                    return lua_toboolean(*this, -1);
                }

                virtual IConfig* AsSubConfig() {
                    if (lua_type(*this, -1) != LUA_TTABLE) {
                        ythrow TConfigParseError() << "parse error";
                    }

                    return this;
                }
        };

        return new TConfig(in);
    }
}
