#pragma once

#include "str_map.h"

#include <cstdio>

#include <util/generic/pair.h>
#include <util/generic/vector.h>
#include <util/generic/utility.h>

template <class HashFcn, class EqualTo>
class atomizer : public string_hash<ui32, HashFcn, EqualTo>
{
private:
    yvector<const char*> order;
public:
    typedef typename string_hash<ui32, HashFcn, EqualTo>::iterator iterator;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::const_iterator const_iterator;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::value_type value_type;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::size_type size_type;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::pool_size_type pool_size_type;

    using string_hash<ui32, HashFcn, EqualTo>::pool;
    using string_hash<ui32, HashFcn, EqualTo>::size;
    using string_hash<ui32, HashFcn, EqualTo>::find;
    using string_hash<ui32, HashFcn, EqualTo>::end;
    using string_hash<ui32, HashFcn, EqualTo>::insert_copy;
    using string_hash<ui32, HashFcn, EqualTo>::clear_hash;

    atomizer()
    {
        order.reserve(HASH_SIZE_DEFAULT);
    }
    atomizer(size_type hash_size, pool_size_type pool_size)
      : string_hash<ui32, HashFcn, EqualTo>(hash_size, pool_size)
    {
        order.reserve(hash_size);
    }
    ~atomizer()
    {
    }
    ui32 string_to_atom(const char* key)
    {
        const char *old_begin = pool.Begin();
        const char *old_end = pool.End();
        TPair<iterator,bool> ins = insert_copy(key, ui32(size()+1));
        if (ins.second) { // new?
            if (pool.Begin() != old_begin) // repoint?
                for (yvector<const char*>::iterator ptr = order.begin(); ptr != order.end(); ++ptr)
                    if (old_begin <= *ptr && *ptr < old_end) // from old pool?
                        *ptr += pool.Begin() - old_begin;
            order.push_back((*ins.first).first); // copy of 'key'
        }
        return (ui32)(*ins.first).second;
    }

    ui32 perm_string_to_atom(const char* key)
    {
        value_type val (key, ui32(size()+1));
        TPair<iterator,bool> ins = this->insert(val);
        if (ins.second)
            order.push_back((*ins.first).first); // == copy of 'key'
        return (ui32)(*ins.first).second; // == size()+1
    }
    ui32 find_atom(const char *key) const
    {
        const_iterator it = find(key);
        if (it == end())
            return 0; // INVALID_ATOM
        else
            return (ui32)(*it).second;
    }
    const char *get_atom_name(ui32 atom) const
    {
        if (atom && atom <= size())
            return order[atom-1];
        return NULL;
    }
    void clear_atomizer()
    {
        clear_hash();
        order.clear();
    }
    void SaveC2N(FILE* f) const {                   // we write sorted file
        for (ui32 i = 0; i < order.size(); i++)
            if (order[i])
                fprintf(f, "%d\t%s\n", i+1, order[i]);
    }
    void LoadC2N(FILE* f) {                         // but can read unsorted one
        long k, km = 0;
        char buf[1000];
        char *s;
        while (fgets(buf, 1000, f)) {
            k = strtol(buf, &s, 10);
            char *endl = strchr(s, '\n');
            if (endl)
                *endl = 0;
            if (k > 0 && k != LONG_MAX) {
                km = Max(km, k);
                insert_copy(++s, ui32(k));
            }
        }
        order.resize(km);
        memset(&order[0], 0, order.size());         // if some atoms are absent
        for (const_iterator I = this->begin(); I != end(); ++I)
            order[(*I).second - 1] = (*I).first;
    }
};

template <class T, class HashFcn, class EqualTo>
class super_atomizer : public string_hash<ui32, HashFcn, EqualTo>
{
private:
    typedef yvector<TPair<const char*, T> > TOrder;
    TOrder order;
public:
    typedef typename string_hash<ui32, HashFcn, EqualTo>::iterator iterator;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::const_iterator const_iterator;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::value_type value_type;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::size_type size_type;
    typedef typename string_hash<ui32, HashFcn, EqualTo>::pool_size_type pool_size_type;

    typedef typename TOrder::iterator       o_iterator;
    typedef typename TOrder::const_iterator o_const_iterator;
    typedef typename TOrder::value_type     o_value_type;

    using string_hash<ui32, HashFcn, EqualTo>::pool;
    using string_hash<ui32, HashFcn, EqualTo>::size;
    using string_hash<ui32, HashFcn, EqualTo>::find;
    using string_hash<ui32, HashFcn, EqualTo>::end;
    using string_hash<ui32, HashFcn, EqualTo>::insert_copy;
    using string_hash<ui32, HashFcn, EqualTo>::clear_hash;

    super_atomizer()
    {
        order.reserve(HASH_SIZE_DEFAULT);
    }
    super_atomizer(size_type hash_size, pool_size_type pool_size)
      : string_hash<ui32, HashFcn, EqualTo>(hash_size, pool_size)
    {
        order.reserve(hash_size);
    }
    ~super_atomizer()
    {
    }
    ui32 string_to_atom(const char* key, const T* atom_data = NULL)
    {
        const char *old_begin = pool.Begin();
        const char *old_end = pool.End();
        TPair<iterator,bool> ins = insert_copy(key, ui32(size()+1));
        if (ins.second) { // new?
            if (pool.Begin() != old_begin) // repoint?
                for (typename TOrder::iterator ptr = order.begin(); ptr != order.end(); ++ptr)
                    if (old_begin <= (*ptr).first && (*ptr).first < old_end) // from old pool?
                        (*ptr).first += pool.Begin() - old_begin;
            order.push_back(TPair<const char*, T>((*ins.first).first, atom_data?*atom_data:T()));
        }
        return (*ins.first).second;
    }

    ui32 perm_string_to_atom(const char* key, const T* atom_data = NULL)
    {
        value_type val (key, ui32(size()+1));
        TPair<iterator,bool> ins = insert(val);
        if (ins.second)
            order.push_back(TPair<const char*, T>((*ins.first).first, atom_data?*atom_data:T()));
        return (*ins.first).second; // == size()+1
    }
    ui32 find_atom(const char *key) const
    {
        const_iterator it = find(key);
        if (it == end())
            return 0; // INVALID_ATOM
        else
            return (*it).second;
    }
    const char *get_atom_name(ui32 atom) const {
        if (atom && atom <= size())
            return order[atom-1].first;
        return NULL;
    }
    const T* get_atom_data(ui32 atom) const {
        if (atom && atom <= size())
            return &order[atom-1].second;
        return NULL;
    }
    T* get_atom_data(ui32 atom) {
        if (atom && atom <= size())
            return &order[atom-1].second;
        return NULL;
    }
    o_iterator o_begin() {
        return order.begin();
    }
    o_iterator o_end() {
        return order.end();
    }
    o_const_iterator o_begin() const {
        return order.begin();
    }
    o_const_iterator o_end() const {
        return order.end();
    }
    void clear_atomizer()
    {
        clear_hash();
        order.clear();
    }

};
