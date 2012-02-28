#pragma once

#include "sorttree.h"

#include <util/system/event.h>
#include <util/system/thread.h>
#include <util/system/maxlen.h>
#include <util/system/filemap.h>
#include <util/system/oldfile.h>
#include <util/system/defaults.h>
#include <util/stream/file.h>
#include <util/string/util.h>
#include <util/generic/stroka.h>
#include <util/generic/algorithm.h>
#include <util/fgood.h>

class CFileSortBase
{
protected:
    Stroka m_prefix;
    size_t m_maxElements;
    size_t m_curSort;
    size_t m_maxsSort;

    CFileSortBase()
        : m_maxElements(0)
        , m_curSort(0)
        , m_maxsSort(0)
    {}

    void Open(const char* prefix, size_t maxElements)
    {
        m_prefix = prefix;
        m_maxElements = maxElements;
    }

    TFile OpenPortion(size_t seq_no) {
        Stroka name = Sprintf("%s.%"PRISZT, ~m_prefix, seq_no);
        return TFile(name, RdWr | CreateAlways | AWUser|ARUser|AWGroup|ARGroup|AROther);
    }

    TFile ReadyPortion(size_t seq_no) {
        Stroka name = Sprintf("%s.%"PRISZT, ~m_prefix, seq_no);
        return TFile(name, RdOnly);
    }
};

template <class C, class F, class CF = C>
class CFileSort : public CFileSortBase
{
public:
    CSortTree<CF, F> m_sortTree;
protected:
    bool m_sortFinish;
    Event m_qSortEvent;
    Event m_aSortEvent;
    TMappedArray<C> m_CurrentMap;
    TMappedArray<C> m_SortedMap;
    TThread m_sortThread;

public:
    CFileSort()
        : m_sortFinish(false)
        , m_qSortEvent(Event::rAuto)
        , m_aSortEvent(Event::rAuto)
        , m_sortThread(SortProc, this)
    {}

    virtual ~CFileSort()
    {
        if (!m_sortFinish) {
            DoFinish();
        }
        m_CurrentMap.Destroy();
        m_SortedMap.Destroy();
    }

    void Open(const char* prefix, size_t maxElements)
    {
        CFileSortBase::Open(prefix, maxElements);
        m_CurrentMap.Create(m_maxElements);
        m_sortThread.Start();
    }

    void SaveCount()
    {
        Stroka name = Sprintf("%s.count", ~m_prefix);
        TFILEPtr(~name, "w+").fput((ui32)m_sortTree.size());
    }

    void OpenSorted(const char* prefix)
    {
        ui32 size;
        m_prefix = prefix;
        Stroka name = Sprintf("%s.count", ~m_prefix);
        if (!TFILEPtr(~name, "r").fget(size))
            ythrow yexception() << "OpenSorted: file " << name << " should not be empty";

        m_sortTree.resize(size);
        for (ui32 i = 0; i < size; i++)
            m_sortTree[i].m_file = ReadyPortion(i);
        m_sortTree.Init();
    }

    void SortPiece()
    {
        m_aSortEvent.Wait();
        m_SortedMap.swap(m_CurrentMap);
        m_maxsSort = m_curSort;
        m_qSortEvent.Signal();
        m_curSort = 0;
        assert(m_CurrentMap.begin() == 0);
    }

    void AddElement(const C& elem)
    {
        C* buffer = m_CurrentMap.begin();
        buffer[m_curSort++] = elem;
        if (m_curSort >= m_maxElements) {
            SortPiece();
            m_CurrentMap.Create(m_maxElements);
        }
    }

    void Finish()
    {
        SortPiece();
        m_aSortEvent.Wait();
        DoFinish();
        m_sortTree.Init();
    }

    void Reset()
    {
        m_sortTree.Init();
    }

private:
    void DoFinish()
    {
        m_sortFinish = true;
        m_qSortEvent.Signal();
        m_sortThread.Join();
    }

    // sorting thread's routines
    static void* SortProc(void* param)
    {
        try {
            CFileSort<C, F, CF>* p = (CFileSort<C, F, CF>*)param;
            p->SortThread();
        } catch (...) {
        }
        return 0;
    }

    void SortThread()
    {
        m_aSortEvent.Signal();
        do {
            m_qSortEvent.Wait();
            if (m_SortedMap.begin()) {
                if (m_maxsSort) {
                    SortAndWritePiece();
                }
                m_SortedMap.Destroy();
            }
            m_aSortEvent.Signal();
        } while (!m_sortFinish);
    }

protected:
    virtual void SortAndWritePiece()
    {
        C* slinks = m_SortedMap.begin();
        QuickSort(slinks, slinks + m_maxsSort);
        size_t sz = m_sortTree.size();
        m_sortTree.AppendSource();
        m_sortTree.back().m_file = OpenPortion(sz);
        WritePiece(m_sortTree.back().m_file);
    }
    virtual void WritePiece(TFile& f)
    {
        f.Write(m_SortedMap.begin(), sizeof(C) * m_maxsSort);
    }
};

template<class C, class F, template <class C1, class F1, class CF1 = C1> class S = CFileSort>
class CFileSortU : public S<C, F>
{
protected:
    virtual void WritePiece(TFile& f)
    {
        C* start = S<C, F>::m_SortedMap.begin();
        C* finish = start + S<C, F>::m_maxsSort;
        C* d = start;
        for (C* p = start; p < finish;)
        {
            if (p > d)
                *d = *p;

            d++;
            C* pp = p;

            while ((p < finish) && (*p == *pp)) {
                p++;
            }
        }
        f.Write(start, (d - start) * sizeof(C));
    }
};

template<class C, class F, template <class C1, class F1, class CF1 = C1> class S = CFileSort>
class CFileSortUS : public S<C, F>
{
protected:
    virtual void WritePiece(TFile& f)
    {
        C* start = S<C, F>::m_SortedMap.begin();
        C* finish = start + S<C, F>::m_maxsSort;
        C* d = start;
        for (C* p = start; p < finish;)
        {
            if (p > d)
                *d = *p;
            C* pp = p;
            p++;

            while ((p < finish) && (*p == *pp)) {
                *d += *p++;
            }

            d++;
        }
        f.Write(start, (d - start) * sizeof(C));
    }
};
