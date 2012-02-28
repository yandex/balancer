#include <library/unittest/registar.h>

#ifdef _unix_
#include <sys/resource.h>
#endif

#include "filemap.h"

class TFileMapTest : public TTestBase {
    UNIT_TEST_SUITE(TFileMapTest);
        UNIT_TEST(TestFileMap)
        UNIT_TEST(TestFileMapEmpty)
        UNIT_TEST(TestFileMappedArray)
        UNIT_TEST(TestMappedArray)
        UNIT_TEST(TestNotGreedy)
    UNIT_TEST_SUITE_END();

    TFileMapTest()
        : FileName_("./mappped_file")
    {
    }

    void TestFileMap() {
        char data[] = "abcdefgh";

        TFile file(FileName_, CreateAlways | WrOnly);
        file.Write(static_cast<void*>(data), sizeof(data));
        file.Close();

        {
            TFileMap mappedFile(FileName_, TMemoryMapCommon::oRdWr);
            mappedFile.Map(0, mappedFile.Length());
            UNIT_ASSERT(mappedFile.MappedSize() == sizeof(data) && mappedFile.Length() == sizeof(data));
            UNIT_ASSERT(mappedFile.IsOpen());
            for (size_t i = 0; i < sizeof(data); ++i) {
                UNIT_ASSERT(static_cast<char*>(mappedFile.Ptr())[i] == data[i]);
                static_cast<char*>(mappedFile.Ptr())[i] = data[i] + 1;
            }
            mappedFile.Flush();

            mappedFile.Map(2, 2);
            UNIT_ASSERT(mappedFile.MappedSize() == 2);
            UNIT_ASSERT(static_cast<char*>(mappedFile.Ptr())[0] == 'd' && static_cast<char*>(mappedFile.Ptr())[1] == 'e');

            mappedFile.Unmap();
            UNIT_ASSERT(mappedFile.MappedSize() == 0);

            FILE* f = fopen(FileName_, "rb");
            TFileMap mappedFile2(f);
            mappedFile2.Map(0, mappedFile2.Length());
            UNIT_ASSERT(mappedFile2.MappedSize() == sizeof(data));
            UNIT_ASSERT(static_cast<char*>(mappedFile2.Ptr())[0] == data[0] + 1);
            fclose(f);
        }
        unlink(FileName_);
    }

    void TestFileMapEmpty() {
        TFile file(FileName_, CreateAlways | WrOnly);
        file.Close();

        TMappedFile map;
        map.init(FileName_);
        map.getData(0);

        unlink(FileName_);
    }

    void TestNotGreedy() {
        int page[4096 / sizeof(int)];

#ifdef _unix_
        // Temporary limit allowed virtual memory size to 1Gb
        struct rlimit rlim;
        if (getrlimit(RLIMIT_AS, &rlim))
            throw TSystemError() << "Cannot get rlimit for virtual memory";
        rlim_t Limit = 1*1024*1024*1024;
        if (rlim.rlim_cur > Limit) {
            rlim.rlim_cur = Limit;
            if (setrlimit(RLIMIT_AS, &rlim))
                throw TSystemError() << "Cannot set rlimit for virtual memory to 1Gb";
        }
#endif
        // Make a 128M test file
        try {
            TFile file(FileName_, CreateAlways | WrOnly);
            int i = 0;
            for (unsigned pages = 128*1024*1024 / sizeof(page); pages--; i++) {
                std::fill(page, page + sizeof(page)/sizeof(*page), i);
                file.Write(page, sizeof(page));
            }
            file.Close();

            // Make 16 maps of our file, which would require 16*128M = 2Gb and exceed our 1Gb limit
            yvector< TAutoPtr<TFileMap> > maps;
            for (int i = 0; i < 16; ++i) {
                maps.push_back(TAutoPtr<TFileMap>(new TFileMap(FileName_, TMemoryMapCommon::OMode(TMemoryMapCommon::oRdOnly | TMemoryMapCommon::oNotGreedy))));
                maps.back()->Map(i * sizeof(page), sizeof(page));
            }

            // Oh, good, we're not dead yet
            for (int i = 0; i < 16; ++i) {
                TFileMap& map = *maps[i];
                UNIT_ASSERT_EQUAL(map.Length(), 128*1024*1024);
                UNIT_ASSERT_EQUAL(map.MappedSize(), sizeof(page));
                const int* mappedPage = (const int*) map.Ptr();
                for (size_t j = 0; j < sizeof(page) / sizeof(*page); ++j)
                    UNIT_ASSERT_EQUAL(mappedPage[j], i);
            }

#ifdef _unix_
            // Restore limits and cleanup
            rlim.rlim_cur = rlim.rlim_max;
            if (setrlimit(RLIMIT_AS, &rlim))
                throw TSystemError() << "Cannot restore rlimit for virtual memory";
#endif
            maps.clear();
            unlink(FileName_);
        } catch (...) {
            // TODO: RAII'ize all this stuff
#ifdef _unix_
            rlim.rlim_cur = rlim.rlim_max;
            if (setrlimit(RLIMIT_AS, &rlim))
                throw TSystemError() << "Cannot restore rlimit for virtual memory";
#endif
            unlink(FileName_);
            throw;
        }
    }

    void TestFileMappedArray() {
        {
            TFileMappedArray<ui32> mappedArray;
            ui32 data[] = {123, 456, 789, 10};
            size_t sz = sizeof(data) / sizeof(data[0]);

            TFile file(FileName_, CreateAlways | WrOnly);
            file.Write(static_cast<void*>(data), sizeof(data));
            file.Close();

            mappedArray.init(FileName_);
            UNIT_ASSERT(mappedArray.size() == sz);
            for (size_t i = 0; i < sz; ++i) {
                UNIT_ASSERT(mappedArray[i] == data[i]);
            }
            UNIT_ASSERT(mappedArray.GetAt(mappedArray.size()) == 0);
            UNIT_ASSERT(*mappedArray.begin() == data[0]);
            UNIT_ASSERT(size_t(mappedArray.end() - mappedArray.begin()) == sz);
            UNIT_ASSERT(!mappedArray.empty());
            mappedArray.term();

            file = TFile(FileName_, WrOnly);
            file.Seek(0, sEnd);
            file.Write("x", 1);
            file.Close();

            bool catched = false;
            try {
                mappedArray.init(FileName_);
            } catch (const yexception&) {
                catched = true;
            }
            UNIT_ASSERT(catched);
        }
        unlink(FileName_);
    }

    void TestMappedArray() {
        ui32 sz = 10;

        TMappedArray<ui32> mappedArray;

        ui32* ptr = mappedArray.Create(sz);
        UNIT_ASSERT(ptr != 0);
        UNIT_ASSERT(mappedArray.size() == sz);
        UNIT_ASSERT(mappedArray.begin() + sz == mappedArray.end());

        for (size_t i = 0; i < sz; ++i) {
            mappedArray[i] = (ui32)i;
        }
        for (size_t i = 0; i < sz; ++i) {
            UNIT_ASSERT(mappedArray[i] == i);
        }

        TMappedArray<ui32> mappedArray2(1000);
        mappedArray.swap(mappedArray2);
        UNIT_ASSERT(mappedArray.size() == 1000 && mappedArray2.size() == sz);
    }

    private:
        const char* FileName_;

};

UNIT_TEST_SUITE_REGISTRATION(TFileMapTest);
