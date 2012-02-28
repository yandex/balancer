#pragma once

#include <util/generic/vector.h>
#include <util/system/defaults.h>

template <class C, class F>
class CSortTree: public yvector<F> {
public:
    using yvector<F>::begin;
    using yvector<F>::end;
    using yvector<F>::clear;
    using yvector<F>::size;

    class CTreeElem {
    public:
        C* m_elem;
    };

    typedef F Source;

public:
    yvector<CTreeElem*> m_levels;
    typename yvector<CTreeElem*>::iterator m_topLevel;
    yvector<int> m_sources;
    C* m_bottomLevel;
    F** m_files;
    int m_size;
    int m_comparisons;
    int m_comparisonsr;
    bool m_eof;
    bool m_first;
    C m_prev;

public:
    CSortTree()
    {
        m_bottomLevel = NULL;
        m_files = NULL;
        m_first = true;
    }
    void Finish()
    {
        for (typename yvector<F>::iterator it = begin(); it != end(); it++) {
            it->Finish();
        }
        clear();
        if (m_files) {
            delete [] m_files;
            m_files = NULL;
        }
        if (m_bottomLevel) {
            delete [] m_bottomLevel;
            m_bottomLevel = NULL;
        }
    }
    ~CSortTree() {
        Finish();
        delete [] m_bottomLevel;
        delete [] m_files;
        for (typename yvector<CTreeElem*>::iterator lit = m_levels.begin(); lit != m_levels.end(); lit++) {
            delete [] *lit;
        }
    }
    void AppendSource() {
        push_back(F());
    }
    void Init() {
        m_first = true;
        m_sources.resize(size());
        AllocLevels();
        BuildTree();
    }
    bool Advance();
    void AllocLevels();
    void BuildTree();
    void UpdateTree(int index);
    void AdvanceEof() {
        m_eof = !Advance();
    }
    bool NextNU(C& elem) {
        if (m_eof)
            return false;
        elem = *((*m_topLevel)[0].m_elem);
        m_eof = !Advance();
        return true;
    }
    bool NextNU(C& elem, int& source) {
        if (m_eof)
            return false;
        elem = *((*m_topLevel)[0].m_elem);
        source = m_sources[(*m_topLevel)[0].m_elem - m_bottomLevel];
        m_eof = !Advance();
        return true;
    }
    bool Peek(C& elem, int& source) {
        if (m_eof)
            return false;
        elem = *((*m_topLevel)[0].m_elem);
        source = m_sources[(*m_topLevel)[0].m_elem - m_bottomLevel];
        return true;
    }
    bool Peek(C& elem, F*& source) {
        if (m_eof)
            return false;
        elem = *((*m_topLevel)[0].m_elem);
        source = m_files[(*m_topLevel)[0].m_elem - m_bottomLevel];
        return true;
    }
    bool Peek(C*& elem) {
        if (m_eof)
            return false;
        elem = (*m_topLevel)[0].m_elem;
        return true;
    }
    bool Next(C& value) {
        while (true) {
            if (!NextNU(value))
                return false;
            if (m_first || !(value == m_prev)) {
                m_first = false;
                m_prev = value;
                return true;
            }
        }
    }
    ui32 Size() {
        return size();
    }
};

template<class C, class F>
void CSortTree<C, F>::AllocLevels() {
    m_size = size();
    int levels = 0;
    m_comparisons = 0;
    m_comparisonsr = 0;
    if (!m_bottomLevel)
        m_bottomLevel = new C[m_size];
    C* bottom = m_bottomLevel;
    if (!m_files)
        m_files = new F*[m_size];
    F** pfiles = m_files;
    ui32 src = 0;
    ui32 src1 = 0;
    for (typename yvector<F>::iterator it = begin(); it != end(); it++, src++) {
        it->SeekToBegin();
        if (it->Next(bottom)) {
            bottom++;
            *pfiles++ = &*it;
            m_sources[src1++] = src;
        }
    }
    m_size = pfiles - m_files;
    int size = m_size;
    if (size == 1)
        size++;
    int s = size;
    while (s > 1) {
        levels++;
        s = (s + 1) >> 1;
    }
    for (typename yvector<CTreeElem*>::iterator it = m_levels.begin(); it != m_levels.end(); it++) {
        delete [] *it;
    }
    m_levels.resize(levels);
    for (; size > 1;) {
        levels--;
        size = (size + 1) >> 1;
        m_levels[levels] = new CTreeElem[size];
    }
    m_eof = m_size == 0;
}

template<class C, class F>
void CSortTree<C, F>::UpdateTree(int i) {
    typename yvector<CTreeElem*>::iterator it = m_levels.end() - 1;
    i &= 0xFFFFFFFE;
    C* bottom = m_bottomLevel + i;
    int least = i + 1 >= m_size ? 0 :
            (m_comparisonsr++, bottom[0] < bottom[1]) ? 0 : 1;
//    (*it)[i >> 1].m_least = least;
    (*it)[i >> 1].m_elem = m_bottomLevel + (i + least);
    i >>= 1;
    int curSize = (m_size + 1) >> 1;
    while (curSize > 1) {
        typename yvector<CTreeElem*>::iterator itp = it - 1;
        i &= 0xFFFFFFFE;
        CTreeElem* elem = (*it) + i;
        int least = i + 1 >= curSize ? 0 :
            (m_comparisonsr++, *(elem[0].m_elem) < *(elem[1].m_elem)) ? 0 : 1;
        CTreeElem& e = (*itp)[i >> 1];
//        e.m_least = least;
        e.m_elem = (*it)[i + least].m_elem;
        i >>= 1;
        it = itp;
        curSize = (curSize + 1) >> 1;
    }
    m_topLevel = it;
}

template<class C, class F>
void CSortTree<C, F>::BuildTree() {
    typename yvector<CTreeElem*>::iterator it = m_levels.end() - 1;
    int i;
    for (i = 0; i < m_size; i += 2) {
        int least = i + 1 >= m_size ? 0 :
            (m_comparisonsr++, m_bottomLevel[i] < m_bottomLevel[i + 1]) ? 0 : 1;
//        (*it)[i >> 1].m_least = least;
        (*it)[i >> 1].m_elem = m_bottomLevel + (i + least);
    }
    int curSize = (m_size + 1) >> 1;
    while (curSize > 1) {
        typename yvector<CTreeElem*>::iterator itp = it - 1;
        for (i = 0; i < curSize; i += 2) {
            int least = i + 1 >= curSize ? 0 :
                (m_comparisonsr++, *((*it)[i].m_elem) < *((*it)[i + 1].m_elem)) ? 0 : 1;
            CTreeElem& e = (*itp)[i >> 1];
//            e.m_least = least;
            e.m_elem = (*it)[i + least].m_elem;
        }
        it = itp;
        curSize = (curSize + 1) >> 1;
    }
    m_topLevel = it;
}

template<class C, class F>
bool CSortTree<C, F>::Advance() {
    typename yvector<CTreeElem*>::iterator it;
    int ind = (*m_topLevel)[0].m_elem - m_bottomLevel;
    F* file = m_files[ind];
    int curSize = m_size;
    if (!file->Next(m_bottomLevel + ind)) {
        if (m_size <= 1)
            return false;
        m_size--;
        if (ind < m_size) {
            m_files[ind] = m_files[m_size];
            m_bottomLevel[ind] = m_bottomLevel[m_size];
            m_sources[ind] = m_sources[m_size];
            UpdateTree(m_size);
        }
//        BuildTree();
        UpdateTree(ind);
        return true;
    }
    ind &= 0xFFFFFFFE;
    int i2 = ind >> 1;
    it = m_levels.end() - 1;
    int least = ind + 1 >= curSize ? 0 :
            (m_comparisons++, m_bottomLevel[ind] < m_bottomLevel[ind + 1]) ? 0 : 1;
//    (*it)[i2].m_least = least;
    (*it)[i2].m_elem = m_bottomLevel + (ind + least);
    for (; it != m_topLevel;) {
        curSize = (curSize + 1) >> 1;
        i2 &= 0xFFFFFFFE;
        least = i2 + 1 >= curSize ? 0 :
            (m_comparisons++, *((*it)[i2].m_elem) < *((*it)[i2 + 1].m_elem)) ? 0 : 1;
        C* leastElem = (*it)[i2 + least].m_elem;
        it--;
        i2 >>= 1;
//        (*it)[i2].m_least = least;
        (*it)[i2].m_elem = leastElem;
    }
    return true;
}

template<class C, class F>
class CASortTree {
public:
    typedef F Source;

public:
    CSortTree<C, F> m_st;
    F m_src;
    C m_srcelem;
    C m_stelem;
//    int m_stsource;
    F* m_stf;
    int m_source;
    ui32 m_srccount;
    ui32 m_stcount;
    bool m_srceof;
    bool m_steof;
    bool m_bsrc;

public:
    void Init() {
        m_st.Init();
        m_src.SeekToBegin();
        m_srceof = !m_src.Next(&m_srcelem);
//        m_steof = !m_st.Peek(m_stelem, m_stsource);
        m_steof = !m_st.Peek(m_stelem, m_stf);
        m_bsrc = m_steof || m_srceof ? false : m_srcelem < m_stelem;
        m_srccount = 0;
        m_stcount = 0;
    }

    bool Peek(C& elem, F*& source) {
        if (m_steof && m_srceof)
            return false;
        if (m_steof || m_bsrc) {
            elem = m_srcelem;
            source = &m_src;
            return true;
        }
        if (m_srceof || !m_bsrc) {
            elem = m_stelem;
//            source = m_stsource < m_source ? m_stsource : m_stsource + 1;
            source = m_stf;
            return true;
        }
        return true;
    }
    void AdvanceEof() {
        if (m_srceof && m_steof)
            return;
        if (m_steof || m_bsrc) {
            m_srceof = !m_src.Next(&m_srcelem);
            m_bsrc = m_steof || m_srceof ? false : m_srcelem < m_stelem;
            m_srccount++;
            return;
        }
        if (m_srceof || !m_bsrc) {
            m_st.AdvanceEof();
//            m_steof = !m_st.Peek(m_stelem, m_stsource);
            m_steof = !m_st.Peek(m_stelem, m_stf);
            m_bsrc = m_steof || m_srceof ? false : m_srcelem < m_stelem;
            m_stcount++;
        }
    }
    ui32 Size() {
        return m_st.size() + 1;
    }
    F& operator[](int i) {
        return i == m_source ? m_src : i < m_source ?  m_st[i] : m_st[i - 1];
    }
    void Finish() {
        m_st.Finish();
        m_src.Finish();
    }
};
