#include "dirut.h"

#include <library/unittest/registar.h>

#include <util/system/platform.h>

SIMPLE_UNIT_TEST_SUITE(TDirutTest) {
    SIMPLE_UNIT_TEST(TestRealPath) {
        UNIT_ASSERT(IsDir(RealPath(".")));
    }

    SIMPLE_UNIT_TEST(TestResolvePath) {
        Stroka path;

        path = "bar";
        UNIT_ASSERT(resolvepath(path, "/foo/baz"));
        UNIT_ASSERT(path == "/foo/bar");

        path = "/foo/bar";
        UNIT_ASSERT(resolvepath(path, "/bar/baz"));
        UNIT_ASSERT(path == "/foo/bar");

#ifdef NDEBUG
        path = "bar";
        UNIT_ASSERT(resolvepath(path, "./baz"));
        UNIT_ASSERT(path == "./bar");

#if 0 // should we support, for consistency, single-label dirs
        path = "bar";
        UNIT_ASSERT(resolvepath(path, "baz"));
        UNIT_ASSERT(path == "bar");
#endif
#endif
    }

    SIMPLE_UNIT_TEST(TestGetCwd) {
        Stroka cwd = GetCwd();
#ifdef _win_
        UNIT_ASSERT(cwd.at(1) == ':');
#else
        UNIT_ASSERT(cwd.at(0) == '/');
#endif
    }
};

