#pragma once

#include "utility.h"

#include <stlport/algorithm>

template <class T>
static inline void QuickSort(T f, T l) {
    NStl::sort(f, l);
}

template <class T, class C>
static inline void QuickSort(T f, T l, C c) {
    NStl::sort(f, l, c);
}

template <class T>
static inline void Sort(T f, T l) {
    NStl::sort(f, l);
}

template <class T, class C>
static inline void Sort(T f, T l, C c) {
    NStl::sort(f, l, c);
}

template <class T>
static inline void MergeSort(T f, T l) {
    NStl::stable_sort(f, l);
}

template <class T, class C>
static inline void MergeSort(T f, T l, C c) {
    NStl::stable_sort(f, l, c);
}

template <class T>
static inline void StableSort(T f, T l) {
    NStl::stable_sort(f, l);
}

template <class T, class C>
static inline void StableSort(T f, T l, C c) {
    NStl::stable_sort(f, l, c);
}

template <class T>
static inline void PartialSort(T f, T m, T l) {
    NStl::partial_sort(f, m, l);
}

template <class T, class C>
static inline void PartialSort(T f,T m, T l, C c) {
    NStl::partial_sort(f, m, l, c);
}

template <class T, class R>
static inline R PartialSortCopy(T f, T l, R of, R ol) {
    return NStl::partial_sort_copy(f, l, of, ol);
}

template <class T, class R, class C>
static inline R PartialSortCopy(T f, T l, R of, R ol, C c) {
    return NStl::partial_sort_copy(f, l, of, ol, c);
}

template <class I, class T>
static inline I Find(I f, I l, const T& v) {
    return NStl::find(f, l, v);
}

template <class I, class P>
static inline I FindIf(I f, I l, P p) {
    return NStl::find_if(f, l, p);
}

template<class C, class T>
inline bool IsIn(const C &c, const T &e) {
    return NStl::find(c.begin(), c.end(), e) != c.end();
}

template <class I>
static inline void PushHeap(I f, I l) {
    NStl::push_heap(f, l);
}

template <class I, class C>
static inline void PushHeap(I f, I l, C c) {
    NStl::push_heap(f, l, c);
}

template <class I>
static inline void PopHeap(I f, I l) {
    NStl::pop_heap(f, l);
}

template <class I, class C>
static inline void PopHeap(I f, I l, C c) {
    NStl::pop_heap(f, l, c);
}

template <class I>
static inline void MakeHeap(I f, I l) {
    NStl::make_heap(f, l);
}

template <class I, class C>
static inline void MakeHeap(I f, I l, C c) {
    NStl::make_heap(f, l, c);
}

template <class I, class T>
static inline I LowerBound(I f, I l, const T& v) {
    return NStl::lower_bound(f, l, v);
}

template <class I, class T, class C>
static inline I LowerBound(I f, I l, const T& v, C c) {
    return NStl::lower_bound(f, l, v, c);
}

template <class I, class T>
static inline I UpperBound(I f, I l, const T& v) {
    return NStl::upper_bound(f, l, v);
}

template <class I, class T, class C>
static inline I UpperBound(I f, I l, const T& v, C c) {
    return NStl::upper_bound(f, l, v, c);
}

template <class T>
static inline T Unique(T f, T l) {
    return NStl::unique(f, l);
}

template <class T, class P>
static inline T Unique(T f, T l, P p) {
    return NStl::unique(f, l, p);
}

template <class T1, class T2>
static inline bool Equal(T1 f1, T1 l1, T2 f2) {
    return NStl::equal(f1, l1, f2);
}

template <class T1, class T2, class P>
static inline bool Equal(T1 f1, T1 l1, T2 f2, P p) {
    return NStl::equal(f1, l1, f2, p);
}

template <class TI, class TO>
static inline TO Copy(TI f, TI l, TO t) {
    return NStl::copy(f, l, t);
}

template <class TI, class TO>
static inline TO UniqueCopy(TI f, TI l, TO t) {
    return NStl::unique_copy(f, l, t);
}

template <class TI, class TO, class TP>
static inline TO UniqueCopy(TI f, TI l, TO t, TP p) {
    return NStl::unique_copy(f, l, t, p);
}

template <class TI, class TO, class TP>
static inline TO RemoveCopyIf(TI f, TI l, TO t, TP p) {
    return NStl::remove_copy_if(f, l, t, p);
}

template <class TI1, class TI2, class TO>
static inline TO SetUnion(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p) {
    return NStl::set_union(f1, l1, f2, l2, p);
}

template <class TI1, class TI2, class TO, class TC>
static inline TO SetUnion(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p, TC c) {
    return NStl::set_union(f1, l1, f2, l2, p, c);
}

template <class TI1, class TI2, class TO>
static inline TO SetDifference(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p) {
    return NStl::set_difference(f1, l1, f2, l2, p);
}

template <class TI1, class TI2, class TO, class TC>
static inline TO SetDifference(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p, TC c) {
    return NStl::set_difference(f1, l1, f2, l2, p, c);
}

template <class TI1, class TI2, class TO>
static inline TO SetSymmetricDifference(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p) {
    return NStl::set_symmetric_difference(f1, l1, f2, l2, p);
}

template <class TI1, class TI2, class TO, class TC>
static inline TO SetSymmetricDifference(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p, TC c) {
    return NStl::set_symmetric_difference(f1, l1, f2, l2, p, c);
}

template <class TI1, class TI2, class TO>
static inline TO SetIntersection(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p) {
    return NStl::set_intersection(f1, l1, f2, l2, p);
}

template <class TI1, class TI2, class TO, class TC>
static inline TO SetIntersection(TI1 f1, TI1 l1, TI2 f2, TI2 l2, TO p, TC c) {
    return NStl::set_intersection(f1, l1, f2, l2, p, c);
}

template <class I, class T>
static inline void Fill(I f, I l, const T& v) {
    NStl::fill(f, l, v);
}
