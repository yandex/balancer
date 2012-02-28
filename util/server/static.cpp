#include "static.h"

#include <locale.h>

#include <util/system/oldfile.h>
#include <util/folder/dirut.h>
#include <util/httpdate.h>

#ifdef _win32_
#   include <io.h>
#else
#   include <unistd.h>
#   include <sys/stat.h>
#   include <util/folder/fts.h>
#   include <netdb.h>
#endif

template <class T1>
inline void PrintContentType(const char* mime, T1& conn) {
    if (mime) {
        conn << "Content-Type: " << mime << "\r\n";
    }
}

template <class T1>
inline void PrintLastModified(time_t T, T1& conn) {
    if (T) {
        char lastmodstr[100];
        format_http_date(lastmodstr, sizeof(lastmodstr), T);

        conn << "Last-Modified: " << lastmodstr << "\r\n";
    }
}

template <class T1>
inline void PrintContentLength(ui32 size, T1& conn) {
    if (size) {
        conn << "Content-Length: " << size << "\r\n";
    }
}

template <class T1>
inline void PrintAddHeaders(const char* const* add_headers, T1& conn) {
    if (add_headers) {
        for (const char *p = *add_headers++; p; p=*add_headers++) {
            conn << p;
        }
    }
}

void TStaticDoc::Print(time_t ims, THttpClientRequestEx *clr, const char* const* add_headers) const {
    return Print(ims, clr->Output(), add_headers);
}

void TStaticDoc::Print(time_t ims, TOutputStream& conn, const char* const* add_headers) const {
    if (mtime <= ims) {
        conn << "HTTP/1.1 304 Not modified\r\n\r\n";

        return;
    }

    conn << "HTTP/1.1 200 OK\r\n";
    PrintContentType(mime, conn);
    PrintLastModified(mtime, conn);
    PrintContentLength(size, conn);
    PrintAddHeaders(add_headers, conn);
    conn << "\r\n";
    conn.Write((const char*)data, size);
}

bool TLocalDoc::Print(time_t ims, THttpClientRequestEx *clr, const char* const* add_headers) {
    TOutputStream& conn = clr->Output();

    if (!m_file.IsOpen()) {
        return false;
    } else {
        if (m_mtime <= ims) {
            conn << "HTTP/1.1 304 Not modified\r\n\r\n";

            return true;
        }

        conn << "HTTP/1.1 200 OK\r\n";
        PrintContentType(m_mime, conn);
        PrintLastModified(m_mtime, conn);
        PrintContentLength(m_size, conn);
        PrintAddHeaders(add_headers, conn);
        conn << "\r\n";

        i32 size = 0;
        ui8 data[4096];

        while ((size = m_file.Read(&data, sizeof(data))) > 0) {
            conn.Write((const char*)data, size);
        }

        return true;
    }
}

CHttpServerStatic::~CHttpServerStatic() {
    for (string_hash<TStaticDoc*>::iterator I = StaticDocs.begin();
         I != StaticDocs.end(); I++) {
        delete (*I).second;
    }
}

void CHttpServerStatic::AddMime(const char* ext, const char* mime) {
    Mimes.insert(yhash<stroka, Stroka>::value_type(ext, mime));
}

#define MAX_STATIC_DOC_SIZE (1 << 24)

bool CHttpServerStatic::AddDoc(const char* webpath, const char* locpath) {
    const char* mime = 0;
    const char* ext = strrchr(locpath, '.');
    if (ext) {
        yhash<stroka, Stroka>::const_iterator it = Mimes.find(ext);
        if (it == Mimes.end())
            return false;
        mime = ~it->second;
    } else {
        return false;
    }
    string_hash<TStaticDoc*>::iterator I = StaticDocs.find(webpath);
    if (I != StaticDocs.end())
        return false;
    TOldOsFile doc(locpath, TOldOsFile::oOpenExisting|TOldOsFile::oRdOnly|TOldOsFile::oBinary);
    if (!doc.IsOpen())
        return false;
    i64 Len = doc.Length();
    if (Len > MAX_STATIC_DOC_SIZE) {
        doc.Close();
        return false;
    }
    ui32 size = (ui32)Len;
    ui8* data = new ui8[size];
    doc.Read(data, size);
    doc.Close();
    time_t mtime = 0;
    struct stat fs;
    if (!stat(locpath, &fs))
        mtime = fs.st_mtime;
    TStaticDoc* sd = new TStaticDoc;
    sd->SetData(data, size, mtime, mime, true);
    I = StaticDocs.insert_copy(webpath, sd).first;
    return true;
}

bool CHttpServerStatic::AddDoc(const char* webpath, ui8* d, ui32 s, const char* m)
{
    string_hash<TStaticDoc*>::iterator I = StaticDocs.find(webpath);
    if (I != StaticDocs.end())
        return false;
    TStaticDoc* sd = new TStaticDoc;
    sd->SetData(d, s, time(0), m, false);
    I = StaticDocs.insert_copy(webpath, sd).first;
    return true;
}

bool CHttpServerStatic::HasDoc(const char* webpath) const
{
    return StaticDocs.find(webpath) != StaticDocs.end();
}

void
CHttpServerStatic::PathHandle(const char* path, THttpClientRequestEx *clr, time_t ims) {
    PathHandle(path, clr->Output(), ims);
}

void CHttpServerStatic::PathHandle(const char* path, TOutputStream& outStream, time_t ims) {
    string_hash<TStaticDoc*>::iterator I = StaticDocs.find(path);

    if (I == StaticDocs.end()) {
        outStream << "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
             << "Content-Length: 38\r\n\r\n"
            "HTTP 404 - The page cannot be found.\r\n";

        return;
    }

    (*I).second->Print(ims, outStream);
}

void CHttpServerStatic::InitImages(const char* locpath, const char* webpath)
{
    assert(locpath);
    assert(webpath);
    Stroka SitePath(webpath);
    if (SitePath[+SitePath-1] != '/')
        SitePath.append("/");
    Stroka Local(locpath);
    if (Local[+Local-1] != LOCSLASH_C)
        Local.append(LOCSLASH_S);
#ifdef _win32_
    char buf[512];
    sprintf(buf, "%s*.*", ~Local);
    struct _finddata_t findData;
    intptr_t hFind = _findfirst(buf, &findData);
    if (hFind != -1) {
        do {
            if (findData.attrib & _A_SUBDIR) {
                if (findData.name[0] != '.')
                    InitImages(~(Local + findData.name), ~(SitePath + findData.name));
            } else
                AddDoc(~(SitePath + findData.name), ~(Local + findData.name));
        } while (_findnext(hFind, &findData) == 0);
        _findclose(hFind);
    }
#else
    size_t LocalLen = +Local;
    char* trees[2];
    trees[0] = (char*)locpath;
    trees[1] = 0;
    FTS* FileTree = yfts_open(trees, FTS_LOGICAL, 0);
    if (FileTree == 0)
        return;
    FTSENT* ent = 0;
    while ((ent=yfts_read(FileTree))) {
        if (ent->fts_info != FTS_F)
            continue;
        AddDoc(~(SitePath + (ent->fts_path + LocalLen)), ent->fts_path);
    }
    yfts_close(FileTree);
#endif
}
