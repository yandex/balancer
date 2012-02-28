#pragma once

#include "sorttree.h"
#include "filesort.h"

#include <util/system/maxlen.h>
#include <util/system/filemap.h>
#include <util/system/defaults.h>
#include <util/generic/stroka.h>
#include <util/generic/algorithm.h>

template <class C, class F, class CF = C>
class CFileSortST : public CFileSortBase
{
public:
    CSortTree<CF, F> m_sortTree;

protected:
    TMappedArray<C> m_SortedMap;
    size_t m_max1;

public:
    CFileSortST()
    {
        m_max1 = 0;
    }
    virtual ~CFileSortST()
    {
        m_SortedMap.Destroy();
    }
    void Open(const char* prefix, size_t maxElements)
    {
        CFileSortBase::Open(prefix, maxElements);
        m_SortedMap.Create(m_maxElements);
    }
    void SortPiece()
    {
        if (m_curSort)
        {
            C* slinks = &m_SortedMap[0];
            QuickSort(slinks, slinks + m_curSort);
            size_t sz = m_sortTree.size();
            m_sortTree.AppendSource();
            m_sortTree.back().m_file = OpenPortion(sz);
            m_maxsSort = m_curSort; // for WritePiece
            WritePiece(m_sortTree.back().m_file);
//          fsync(f);
        }
        m_curSort = 0;
    }
    virtual void WritePiece(TFile& f)
    {
        f.Write(m_SortedMap.begin(), sizeof(C) * m_maxsSort);
    }
    void AddElement(const C& elem)
    {
        m_SortedMap[m_curSort++] = elem;
        if (m_curSort >= m_maxElements)
        {
            SortPiece();
        }
    }
    void Finish(bool force_disk = false)
    {
        if (m_sortTree.size() || force_disk)
        {
            SortPiece();
            m_sortTree.Init();
        } else {
            if (m_curSort)
            {
                C* slinks = &m_SortedMap[0];
                QuickSort(slinks, slinks + m_curSort);
                m_max1 = m_curSort;
                m_curSort = 0;
            }
        }
    }
    bool NextNU(C& elem)
    {
        if (m_sortTree.size())
        {
            return m_sortTree.NextNU(elem);
        } else {
            if (m_curSort < m_max1)
            {
                elem = m_SortedMap[m_curSort++];
                return true;
            } else
                return false;
        }
    }
    bool Peek(C& elem, int& source)
    {
        if (m_sortTree.size())
            return m_sortTree.Peek(elem, source);
        else {
            if (m_curSort < m_max1)
            {
                elem = m_SortedMap[m_curSort];
                source = 0;
                return true;
            } else
                return false;
        }
    }
    void AdvanceEof()
    {
        if (m_sortTree.size())
            m_sortTree.AdvanceEof();
        else {
            if (m_curSort < m_max1)
                m_curSort++;
        }
    }
    void Initialize()
    {
        if (m_sortTree.size())
            m_sortTree.Finish();
        else {
            m_max1 = 0;
            m_curSort = 0;
        }
    }
    void Reset()
    {
        if (m_sortTree.size())
            m_sortTree.Init();
        else
            m_curSort = 0;
    }
};
