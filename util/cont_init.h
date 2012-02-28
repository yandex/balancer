#pragma once

#include <util/generic/vector.h>
#include <util/generic/hash_set.h>
#include    <util/generic/set.h>

struct TVectorType
{

};

struct TSetType
{

};

template<typename T>
struct TContAssignerTraits
{
    typedef TVectorType TTraits;
};

template<class Value, class HashFcn, class EqualKey, class Alloc>
struct TContAssignerTraits< yhash_set<Value, HashFcn, EqualKey, Alloc> >
{
    typedef TSetType TTraits;
};

template <class Key, class Less, class Alloc>
struct TContAssignerTraits< yset<Key, Less, Alloc> >
{
    typedef TSetType TTraits;
};

template <class Key, class Less, class Alloc>
struct TContAssignerTraits< ymultiset<Key, Less, Alloc> >
{
    typedef TSetType TTraits;
};

template<typename T>
class TContAssigner
{
private:
    typedef yvector<T> TData;
    typedef typename TData::const_iterator TDataConstIterator;
    TData Data;

    template<class TCont>
    void To(TCont& result, TVectorType) const
    {
        result.insert(result.end(), Data.begin(), Data.end());
    }

    template<class TCont>
    void To(TCont& result, TSetType) const
    {
        for (TDataConstIterator toData = Data.begin(); toData != Data.end(); ++toData)
            result.insert(*toData);
    }

public:
    inline TContAssigner& operator()(const T& value)
    {
        Data.push_back(value);
        return *this;
    }

    template<class TCont>
    void ToContainer(TCont& cont) const
    {
        typedef typename TContAssignerTraits<TCont>::TTraits TTraits;
        To(cont, TTraits());
    }

    template<class TCont>
    operator TCont() const
    {
        TCont result;
        ToContainer(result);
        return result;
    }

    TContAssigner<T> operator+(const TContAssigner<T>& b) const {
        TContAssigner<T> result;
        for (size_t i = 0; i < Data.size(); ++i)
            result.Data.push_back(Data[i]);
        for (size_t i = 0; i < b.Data.size(); ++i)
            result.Data.push_back(b.Data[i]);
        return result;
    }

    /*
    operator const yvector<T>&() const
    {
        return Data;
    }
    */
};

template<typename T>
TContAssigner<T> ContOf()
{
    return TContAssigner<T>();
}

template<typename T>
TContAssigner<T> ContOf(const T& value)
{
    return TContAssigner<T>()(value);
}
