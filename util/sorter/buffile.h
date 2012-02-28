#pragma once

#include <sys/types.h>

#include <errno.h>

#include <util/system/filemap.h>
#include <util/system/defaults.h>

template<class C, int size = 0x100000>
class CBufferedReadFile
{
public:
    TMappedAllocation *m_Map;
    ui32 m_bufSize;
    ui32 m_current;
    TFile m_file;

public:
    CBufferedReadFile()
      : m_Map(NULL)
      , m_bufSize(0)
      , m_current(0)
    {
    }
    ~CBufferedReadFile()
    {
        delete m_Map;
    }
    bool Next(C* elem)
    {
        if (m_current >= m_bufSize)
        {
            ui32 sz = size / sizeof(C);
            if (m_Map == NULL)
                m_Map = new TMappedAllocation(sz * sizeof(C));
            if (m_Map == 0 || m_Map->Ptr() == 0) // out of memory
                return false;
            m_current = 0;
            try {
                size_t r = m_file.Read(m_Map->Ptr(), sz * sizeof(C));
                if (r == 0)
                    return false;
                m_bufSize = r / sizeof(C);
            } catch (...) {
                fprintf(stderr, "Error in read, errno: %i\n", errno);
                return false;
            }
        }
        C* buf = (C*)m_Map->Ptr();
        *elem = buf[m_current++];
        return true;
    }
    void Finish()
    {
        if (m_file.IsOpen()) {
            TFileHandle temp(m_file.GetHandle());
            temp.Resize(0);
            temp.Release();
            m_file.Close();
        }
    }
    void SeekToBegin()
    {
        m_file.Seek(0, sSet);
        m_current = 0;
        m_bufSize = 0;
    }
    bool SetFile(const char* name) {
        try {
            m_file = TFile(name, RdOnly);
        } catch (const yexception& e) {
            fprintf(stderr, "Can't open file %s: %s\n", name, e.what());
            return false;
        }
        return true;
    }
};
