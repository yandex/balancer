#pragma once

#include "save_stl.h"
#include "sthash_iterators.h"

#include <stlport/algorithm>
#include <stlport/functional>
#include <stlport/memory>

#include <util/generic/pair.h>
#include <util/generic/hash.h>
#include <util/generic/vector.h>
#include <util/generic/yexception.h> // for save/load only

#include <cstdlib>
#include <cstddef>
#include <cassert>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4624) // 'destructor could not be generated because a base class destructor is inaccessible'
#endif

template<class HashType, class KeySaver>
inline void SaveHashToFileEx(HashType &hash, const char *fileName) {
    FILE *f = fopen(fileName, "wb");
    if (!f)
        ythrow yexception() <<  fileName << ": " << LastSystemErrorText();
    TStlSaver saver(f, 0);
    KeySaver ks;
    if (hash.save_for_st(saver, ks))
        ythrow yexception() << "Could not save hash to " <<  fileName;
    if (fclose(f))
        ythrow yexception() <<  fileName << ": " << LastSystemErrorText();
}

template<class HashType>
inline void SaveHashToFile(HashType &hash, const char *fileName) {
    typedef TSthashWriter<typename HashType::key_type, typename HashType::mapped_type, ui64> KeySaver;
    return SaveHashToFileEx<HashType, KeySaver>(hash, fileName);
}

template<class HashType>
inline void SaveHashSetToFile(HashType &hash, const char *fileName) {
    typedef TSthashSetWriter<typename HashType::key_type, ui64> KeySaver;
    return SaveHashToFileEx<HashType, KeySaver>(hash, fileName);
}

template<class HashType>
inline void SaveHashToFile32(HashType &hash, const char *fileName) {
    typedef TSthashWriter<typename HashType::key_type, typename HashType::mapped_type, ui32> KeySaver;
    return SaveHashToFileEx<HashType, KeySaver>(hash, fileName);
}

template <class Iter, typename size_type_f = ui64>
class sthashtable {
public:
  typedef typename Iter::TKeyType key_type;
  typedef typename Iter::TValueType value_type;
  typedef typename Iter::THasherType hasher;
  typedef typename Iter::TKeyEqualType key_equal;

  typedef size_type_f size_type;
  typedef ptrdiff_t         difference_type;
  typedef const value_type* const_pointer;
  typedef const value_type& const_reference;

  typedef Iter const_iterator;

  const hasher hash_funct() const { return hash; }
  const key_equal key_eq() const { return equals; }

private:
  const hasher hash;
  const key_equal equals;

private:
//  const size_type *buckets;
  const size_type *buckets() const { return (size_type*)((char*)this + sizeof(*this)); }
  const size_type buckets(size_type n) const { return buckets()[n]; }

  const_iterator iter_at_bucket(size_type bucket) const {
    return (const_iterator)(((char*)this + buckets()[bucket]));
  }

  const_iterator iter_at_bucket_or_end(size_type bucket) const {
    if (bucket < num_buckets)
      return (const_iterator)(((char*)this + buckets()[bucket]));
    else
      return end();
  }

  const size_type num_buckets;
  const size_type num_elements;
  const size_type data_end_off;

protected: //shut up gcc warning
  // we can't construct/destroy this object at all!
  sthashtable();
  sthashtable(const sthashtable& ht);
  ~sthashtable();

public:

  size_type size() const { return num_elements; }
  size_type max_size() const { return size_type(-1); }
  bool empty() const { return size() == 0; }

  const_iterator begin() const { return num_buckets? iter_at_bucket(0) : end(); }

  const_iterator end() const { return (const_iterator)(((char*)this + data_end_off)); }

public:

  size_type bucket_count() const { return num_buckets; }

  size_type elems_in_bucket(size_type bucket) const
  {
    size_type result = 0;
    const_iterator first = iter_at_bucket(bucket);
    const_iterator last  = iter_at_bucket_or_end(bucket + 1);

    for (; first != last; ++first)
      ++result;
    return result;
  }

  template<class TheKey>
  const_iterator find(const TheKey& key) const
  {
    size_type n = bkt_num_key(key);
    const_iterator first(iter_at_bucket(n)), last(iter_at_bucket_or_end(n + 1));
    for (;
        first != last && !first.KeyEquals(equals, key);
        ++first)
    {}
    if (first != last) return first;
    return end();
  }

  size_type count(const key_type& key) const
  {
    const size_type n = bkt_num_key(key);
    size_type result = 0;
    const_iterator first = iter_at_bucket(n);
    const_iterator last  = iter_at_bucket_or_end(n + 1);

    for (; first != last; ++first)
      if (first.KeyEquals(equals, key))
        ++result;
    return result;
  }

  TPair<const_iterator, const_iterator> equal_range(const key_type& key) const;

private:

  template<class TheKey>
  size_type bkt_num_key(const TheKey& key) const
  {
    return hash(key) % num_buckets;
  }
};

template <class I, class size_type_f>
TPair<I, I> sthashtable<I, size_type_f>::equal_range(const key_type& key) const
{
  typedef TPair<const_iterator, const_iterator> pii;
  const size_type n = bkt_num_key(key);
  const_iterator first = iter_at_bucket(n);
  const_iterator last  = iter_at_bucket_or_end(n + 1);

  for (; first != last; ++first) {
    if (first.KeyEquals(equals, key)) {
      for (const_iterator cur = first + 1; cur != last; ++cur)
        if (!cur.KeyEquals(equals, key))
          return pii(const_iterator(first),
                     const_iterator(cur));
      return pii(const_iterator(first),
                 const_iterator(last));
    }
  }
  return pii(end(), end());
}

/* end __SGI_STL_HASHTABLE_H */

template <class Key, class T, class HashFcn /*= hash<Key>*/,
          class EqualKey = TEqualTo<Key>, typename size_type_f = ui64>
class sthash
{
private:
  typedef sthashtable<TSthashIterator<const Key, const T, HashFcn, EqualKey>, size_type_f> ht;
  ht rep;

public:
  typedef typename ht::key_type key_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;
  typedef T mapped_type;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::const_iterator const_iterator;

  const hasher hash_funct() const { return rep.hash_funct(); }
  const key_equal key_eq() const { return rep.key_eq(); }

public:
  size_type size() const { return rep.size(); }
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }

  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }

public:
  template<class TheKey>
  const_iterator find(const TheKey& key) const { return rep.find(key); }
  template<class TheKey>
  bool has(const TheKey& key) const { return rep.find(key) != rep.end(); }

  size_type count(const key_type& key) const { return rep.count(key); }

  TPair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return rep.equal_range(key); }

  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }
  size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};

template <class Key, class HashFcn,
          class EqualKey = TEqualTo<Key>, typename size_type_f = ui64>
class sthash_set : public sthash<Key, TEmptyValue, HashFcn, EqualKey, size_type_f> {

  typedef sthash<Key, TEmptyValue, HashFcn, EqualKey, size_type_f> Base;
public:
  using Base::key_type;
  using Base::value_type;
  using Base::hasher;
  using Base::key_equal;
  using Base::size_type;
  using Base::const_iterator;
};

template <class Key, class T, class HashFcn /*= hash<Key>*/,
          class EqualKey = TEqualTo<Key>, typename size_type_f = ui64>
class sthash_mm
{
private:
  typedef sthashtable<TSthashIterator<const Key, T, HashFcn, EqualKey>, size_type_f> ht;
  ht rep;

public:
  typedef typename ht::key_type key_type;
  typedef typename ht::value_type value_type;
  typedef typename ht::hasher hasher;
  typedef typename ht::key_equal key_equal;
  typedef T mapped_type;
  typedef typename ht::key_saver_type key_saver_type;

  typedef typename ht::size_type size_type;
  typedef typename ht::difference_type difference_type;
  typedef typename ht::pointer pointer;
  typedef typename ht::const_pointer const_pointer;
  typedef typename ht::reference reference;
  typedef typename ht::const_reference const_reference;

  typedef typename ht::iterator iterator;
  typedef typename ht::const_iterator const_iterator;

  const hasher hash_funct() const { return rep.hash_funct(); }
  const key_equal key_eq() const { return rep.key_eq(); }

public:
  size_type size() const { return rep.size(); }
  size_type max_size() const { return rep.max_size(); }
  bool empty() const { return rep.empty(); }

  const_iterator begin() const { return rep.begin(); }
  const_iterator end() const { return rep.end(); }

  const_iterator find(const key_type& key) const { return rep.find(key); }

  size_type count(const key_type& key) const { return rep.count(key); }

  TPair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return rep.equal_range(key); }

  size_type bucket_count() const { return rep.bucket_count(); }
  size_type max_bucket_count() const { return rep.max_bucket_count(); }
  size_type elems_in_bucket(size_type n) const
    { return rep.elems_in_bucket(n); }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif
