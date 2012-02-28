#pragma once

#include <util/generic/hash.h>

#include <cstdio>

class TStlSaver {
protected:
    FILE *File;
    int State;
    const char *Offset;
public:
    TStlSaver()
        : File(0)
        , State(0)
        , Offset(0)
    {}
    TStlSaver(const char *fname, const char *base);
    TStlSaver(FILE *file, const char *base = 0)
        : File(file)
        , State(0)
        , Offset(base)
    {}
    virtual int Save(const void *buffer, size_t length) {
        Offset += length;
        if (fwrite(buffer, 1, length, File) != length) {
            State |= 1;
        }
        return State;
    }
    virtual ~TStlSaver() {}
    virtual int Align(uintptr_t align) { //give reasonable values (1,2,4,8,16,32) only!
        const char *const alignBuffer = "alignalignalignalignalignalignal";
        const char *oldOffset = Offset;
        assert(align <= 32 && !(align & (align - 1)));
        Offset = (char*)(((uintptr_t)Offset + align - 1) & ~(align - 1));
        align = Offset - oldOffset;
        if (fwrite(alignBuffer, 1, align, File) != align) {
            State |= 1;
        }
        return State;
    }
    virtual const char *GetActiveOffset() const {
        return Offset;
    }
};

// this structure might be replaced with sthashtable class
template <class HF, class Eq, class size_type>
struct sthashtable_nvm_sv {
    HF hf;
    Eq eq;
    size_type num_buckets;
    size_type num_elements;
    size_type data_end_off;
};

template <class V, class K, class HF, class Ex, class Eq, class A>
template <class KS>
inline int yhashtable<V, K, HF, Ex, Eq, A>::save_for_st(TStlSaver &saver, KS &ks) const {
    typedef sthashtable_nvm_sv<HF, Eq, typename KS::TSizeType> sv_type;
    sv_type sv = {hash, equals, static_cast<typename KS::TSizeType>(buckets.size() - 1), static_cast<typename KS::TSizeType>(num_elements), 0};
    // to do: m.b. use just the size of corresponding object?
    typename KS::TSizeType cur_off = sizeof(sv_type) +
            (sv.num_buckets + 1) * sizeof(typename KS::TSizeType);
    sv.data_end_off = cur_off;
    const_iterator n;
    for (n = begin(); n != end(); ++n) {
        sv.data_end_off += static_cast<typename KS::TSizeType>(ks.GetRecordSize(*n));
    }
    //saver.Align(sizeof(char*));
    saver.Save(&sv, sizeof(sv));

    size_type i;
    //save vector
    for (i = 0; i < buckets.size() - 1; ++i) {
        node* cur = buckets[i];
        saver.Save(&cur_off, sizeof(cur_off));
        if (cur) {
            while (!((uintptr_t)cur & 1)) {
                cur_off += static_cast<typename KS::TSizeType>(ks.GetRecordSize(cur->val));
                cur = cur->next;
            }
        }
    }
    saver.Save(&cur_off, sizeof(cur_off)); // end mark
    for (i = 0; i < buckets.size() - 1; ++i) {
        node* cur = buckets[i];
        if (cur) {
            while (!((uintptr_t)cur & 1)) {
                ks.SaveRecord(saver, cur->val);
                cur = cur->next;
            }
        }
    }
    return 0;
}
