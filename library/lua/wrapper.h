#pragma once

#include <util/stream/buffered.h>
#include <util/memory/alloc.h>
#include <util/generic/ptr.h>
#include <util/generic/stroka.h>
#include <util/generic/yexception.h>

#include <contrib/libs/lua/lua.h>

class TLuaReader {
    public:
        inline TLuaReader(IZeroCopyInput* in)
            : In_(in)
        {
        }

        static inline const char* ReadCallback(lua_State*, void* data, size_t* size) {
            return ((TLuaReader*)(data))->Read(size);
        }

    private:
        inline const char* Read(size_t* readed) {
            const char* ret;

            if (In_->Next(&ret, readed)) {
                return ret;
            }

            return 0;
        }

    private:
        IZeroCopyInput* In_;
};

class TLuaStateHolder {
        struct TDeleteState {
            static inline void Destroy(lua_State* state) {
                lua_close(state);
            }
        };

    public:
        class TError: public yexception {
        };

        inline TLuaStateHolder()
            : MyState_(lua_newstate(Alloc, 0))
            , State_(MyState_.Get())
        {
            if (!State_) {
                ythrow TError() << "can not construct lua state: not enough memory";
            }
        }

        inline TLuaStateHolder(lua_State* state) throw ()
            : State_(state)
        {
        }

        inline operator lua_State* () throw () {
            return State_;
        }

        inline void BootStrap() {
            luaL_openlibs(State_);
        }

        inline void error() {
            ythrow TError() << "lua error(" << pop_string() << ")";
        }

        inline Stroka pop_string() {
            require(1);

            if (!lua_isstring(State_, -1)) {
                ythrow TError() << "internal lua error(not a string)";
            }

            size_t len = 0;
            const char* data = lua_tolstring(State_, -1, &len);

            if (!data) {
                ythrow TError() << "internal error(something strange)";
            }

            Stroka ret(data, len);

            lua_pop(State_, 1);

            return ret;
        }

        inline void* pop_void() {
            require(1);

            if (!lua_isuserdata(State_, -1)) {
                ythrow TError() << "internal error(not a void*)";
            }

            const void* ret = lua_topointer(State_, -1);
            lua_pop(State_, 1);

            return (void*)ret;
        }

        inline void push_void(void* ptr) {
            lua_pushlightuserdata(State_, ptr);
        }

        inline void* pop_userdata() {
            return pop_void();
        }

        inline void push_userdata(void* ptr) {
            push_void(ptr);
        }

        inline int pop_int() {
            require(1);

            if (!lua_isnumber(State_, -1)) {
                ythrow TError() << "internal error(not a number)";
            }

            const int ret = (int)lua_tonumber(State_, -1);
            lua_pop(State_, 1);

            return ret;
        }

        inline void push_int(int val) {
            lua_pushnumber(State_, val);
        }

        inline void push_double(double val) {
            lua_pushnumber(State_, val);
        }

        inline void push_float(float val) {
            push_double(val);
        }

        inline void push_string(const char* st) {
            if (!st) {
                st = "";
            }

            lua_pushstring(State_, st);
        }

        inline void push_global(const char* name) {
            lua_getglobal(State_, name);
        }

        inline void set_global(const char* name, const char* value) {
            lua_pushstring(State_, value);
            lua_setglobal(State_, name);
        }

        inline void register_function(const char* name, lua_CFunction func) {
            lua_register(State_, name, func);
        }

        inline void require(int n) {
            if (on_stack() < n) {
                ythrow TError() << "lua requirement failed";
            }
        }

        inline void call(int args, int rets) {
            if (lua_pcall(State_, args, rets, 0)) {
                error();
            }
        }

        inline int on_stack() {
            return lua_gettop(State_);
        }

        inline void gc() {
            lua_gc(State_, LUA_GCCOLLECT, 0);
        }

        inline void Load(TInputStream* in, const Stroka& name) {
            TBufferedInput wi(in, 8192);
            TLuaReader reader(&wi);

            load(reader, name);
        }

        inline void load(TLuaReader& reader, const Stroka& name) {
            if (lua_load(State_, TLuaReader::ReadCallback, &reader, ~name)) {
                ythrow TError() << "can not parse lua chunk " << name;
            }
        }

    private:
        static void* Alloc(void* ud, void* ptr, size_t /*osize*/, size_t nsize) {
            (void)ud;

            if (nsize == 0) {
                y_deallocate(ptr);

                return 0;
            }

            return y_reallocate(ptr, nsize);
        }

    private:
        THolder<lua_State, TDeleteState> MyState_;
        lua_State* State_;
};
