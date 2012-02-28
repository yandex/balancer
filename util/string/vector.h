#pragma once

#include <util/string/cast.h>
#include <util/system/yassert.h>
#include <util/generic/map.h>
#include <util/generic/stroka.h>
#include <util/generic/vector.h>

typedef yvector<Stroka> VectorStrok;
typedef yvector<Wtroka> VectorWtrok;
typedef ymap<Stroka, Stroka> HashStrok;
typedef ymap<Wtroka, Wtroka> HashWtrok;

#define KEEP_EMPTY_TOKENS  0x01
#define KEEP_DELIMITERS    0x02

void SplitStroku(VectorStrok* res, const char* ptr, const char* delimiter, size_t maxFields = 0, int options = 0);
void SplitStrokuBySet(VectorStrok* res, const char* ptr, const char* delimiters, size_t maxFields = 0, int options = 0);
void SplitStrokuBySet(VectorWtrok* res, const wchar16* ptr, const wchar16* delimiters, size_t maxFields = 0, int options = 0);

inline void SplitStroku(VectorStrok* res, const Stroka& str, const char* delimiter, size_t maxFields = 0, int options = 0) {
    SplitStroku(res, ~str, delimiter, maxFields, options);
}

void SplitStroku(VectorWtrok* res, const wchar16* ptr, const wchar16* delimiter, size_t maxFields = 0, int options = 0);

inline void SplitStroku(VectorWtrok* res, const Wtroka& str, const wchar16* delimiter, size_t maxFields = 0, int options = 0) {
    SplitStroku(res, ~str, delimiter, maxFields, options);
}

inline VectorStrok splitStroku(const char* ptr, const char* delimiter, size_t maxFields = 0, int options = 0) {
    VectorStrok res;
    SplitStroku(&res, ptr, delimiter, maxFields, options);
    return res;
}

inline VectorStrok splitStroku(const Stroka& str, const char* delimiter, size_t maxFields = 0, int options = 0) {
    return splitStroku(~str, delimiter, maxFields, options);
}

inline VectorStrok splitStrokuBySet(const char* ptr, const char* delimiters, size_t maxFields = 0, int options = 0) {
    VectorStrok res;
    SplitStrokuBySet(&res, ptr, delimiters, maxFields, options);
    return res;
}

/// Splits input string by given delimiter character.
/*! @param[in, out] str input string
        (will be modified: delimiter will be replaced by NULL character)
    @param[in] delim delimiter character
    @param[out] arr output array of substrings
    @param[in] maxCount max number of substrings to return
    @return count of substrings
*/
size_t SplitStroku(char* str, char delim, char* arr[], size_t maxCount);

template<class TIter>
inline Stroka JoinStroku(TIter begin, TIter end, const Stroka& delim)
{
    if (begin == end)
        return Stroka();

    Stroka result = ToString(*begin);
    for (++begin; begin != end; ++begin) {
        result.append(delim);
        result.append(ToString(*begin));
    }
    return result;
}

/// Concatenates elements of given VectorStrok.
inline Stroka JoinStroku(const VectorStrok& v, const Stroka& delim)
{
    return JoinStroku(v.begin(), v.end(), delim);
}

inline Stroka JoinStroku(const VectorStrok& v, size_t index, size_t count, const Stroka& delim)
{
    YASSERT(index + count <= v.size() && "JoinStroku(): index or count out of range");
    return JoinStroku(v.begin() + index, v.begin() + index + count, delim);
}

Wtroka JoinStroku(const VectorWtrok& v, const Wtroka& delim);
Wtroka JoinStroku(const VectorWtrok& v, size_t index, size_t count, const Wtroka& delim);
