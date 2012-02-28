#include "filelist.h"
#include "dirut.h"

#include <library/unittest/registar.h>

#include <util/system/fs.h>
#include <util/system/file.h>
#include <util/generic/stroka.h>

class TFileListTest: public TTestBase {
        UNIT_TEST_SUITE(TFileList);
            UNIT_TEST(TestSimple);
        UNIT_TEST_SUITE_END();

    public:
        void TestSimple();
};

class TTempDir {
public:
    TTempDir(const Stroka& tempDir)
        : TempDir(tempDir)
    {
        NFs::Remove(~TempDir);
        MakeDirIfNotExist(~TempDir);
    }

    ~TTempDir() {
        RemoveDirWithContents(TempDir);
    }

    Stroka operator ()() const {
        return TempDir;
    }
private:
    Stroka TempDir;
};

void TFileListTest::TestSimple() {
    TTempDir tempDir("nonexistingdir");
    MakeDirIfNotExist(~(tempDir() + LOCSLASH_S"subdir"));
    TFile(~(tempDir() + LOCSLASH_S"subdir"LOCSLASH_S"file"), CreateAlways);

    TFileList fileList;
    fileList.Fill(~tempDir(), "", "", 1000);
    Stroka fileName(fileList.Next());
    UNIT_ASSERT_EQUAL(fileName, "subdir"LOCSLASH_S"file");
    UNIT_ASSERT_EQUAL(fileList.Next(), 0);
}

UNIT_TEST_SUITE_REGISTRATION(TFileListTest);

