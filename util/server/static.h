#pragma once

#include "http.h"

#include <ctime>
#include <cstring>

#include <util/system/oldfile.h>
#include <util/system/defaults.h>
#include <util/generic/hash.h>
#include <util/str_map.h>

class TStaticDoc
{
public:
    TStaticDoc() : data(0), mtime(0), size(0), mime(0), autodel(false) {}
    ~TStaticDoc() {
        if (autodel)
            delete [] data;
    }
    void SetData(const ui8* d, ui32 s, time_t t, const char* m, bool a) {
        data = d;
        size = s;
        mtime = t;
        mime = m;
        autodel = a;
    }
    void Print(time_t ims, THttpClientRequestEx *clr, const char* const* add_headers=0) const;
    void Print(time_t ims, TOutputStream& outStream, const char* const* add_headers=0) const;
private:
    const ui8* data;
    time_t mtime;
    ui32 size;
    const char* mime;
    bool autodel;

private:
    TStaticDoc(const TStaticDoc&);
    TStaticDoc& operator=(const TStaticDoc&);
};

class TLocalDoc
{
public:

    TLocalDoc(const char * local, const char * mime)
        : m_file(local, TOldOsFile::oOpenExisting|TOldOsFile::oRdOnly|TOldOsFile::oBinary)
        , m_mime(mime)
    {
        if (m_file.IsOpen()) {
            struct stat fs;
            if (fstat(m_file.GetHandle(), &fs) == 0) {
                m_mtime = fs.st_mtime;
                m_size = fs.st_size;
            }
        }
    }

    bool Print(time_t IfModSince, THttpClientRequestEx *, const char* const* add_headers=0);

private:

    TOldOsFile     m_file;
    const char * m_mime;
    time_t       m_mtime;
    ui32         m_size;

private:

    TLocalDoc(const TLocalDoc&);
    TLocalDoc& operator=(const TLocalDoc&);
};

class CHttpServerStatic
{
public:
    CHttpServerStatic() {}
    ~CHttpServerStatic();
    void PathHandle(const char* path, THttpClientRequestEx *clr, time_t ims = 0);
    void PathHandle(const char* path, TOutputStream& outStream, time_t ims = 0);
    void AddMime(const char* ext, const char* mime);
    bool AddDoc(const char* webpath, const char* locpath);
    bool AddDoc(const char* webpath, ui8* d, ui32 s, const char* m);
    bool HasDoc(const char* webpath) const;
    void InitImages(const char* locpath, const char* webpath = "/");
protected:
    string_hash<TStaticDoc*> StaticDocs;
    yhash<stroka, Stroka> Mimes;
    DECLARE_NOCOPY(CHttpServerStatic);
};
