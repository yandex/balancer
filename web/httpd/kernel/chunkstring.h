#pragma once

#include <web/httpd/kernel/chunks.h>

#include <util/generic/chartraits.h>

namespace NSrvKernel {
    class TChunkString {
        // Empty chunks are not copied. End chunk must have IndexWithinChunk_ equal to 0.
        private:
            template<class TChunkIter>
            class TBaseIterator {
                public:
                    TBaseIterator(TChunkIter chunk, size_t index)
                        : Chunk_(chunk.Item())
                        , IndexWithinChunk_(index) {
                    }

                    TBaseIterator(const TBaseIterator<TChunkList::TIterator>& other)
                        : Chunk_(other.Chunk_)
                        , IndexWithinChunk_(other.IndexWithinChunk_) {
                    }

                    void Next(size_t cnt) throw () {
                        while (cnt > 0) {
                            size_t chunkLeft = Chunk_->Length() - IndexWithinChunk_;
                            if (chunkLeft <= cnt) {
                                ++Chunk_;
                                IndexWithinChunk_ = 0;
                                cnt -= chunkLeft;
                            } else {
                                IndexWithinChunk_ += cnt;
                                cnt = 0;
                            }
                        }
                    }

                    void Prev(size_t cnt) throw () {
                        size_t chunkLeft = IndexWithinChunk_ + 1;
                        while (cnt >= chunkLeft) {
                            --Chunk_;
                            cnt -= chunkLeft;
                            chunkLeft = Chunk_->Length();
                        }

                        if (cnt > chunkLeft) {
                            IndexWithinChunk_ = 0;
                            cnt -= chunkLeft - 1;
                        } else {
                            IndexWithinChunk_ = chunkLeft - 1 - cnt;
                            cnt = 0;
                        }
                    }

                    void Next() throw () {
                        ++IndexWithinChunk_;
                        if (IndexWithinChunk_ >= Chunk_->Length()) {
                            ++Chunk_;
                            IndexWithinChunk_ = 0;
                        }
                    }

                    void Prev() throw () {
                        if (IndexWithinChunk_ == 0) {
                            --Chunk_;
                            IndexWithinChunk_ = Chunk_->Length() - 1;
                        } else {
                            --IndexWithinChunk_;
                        }
                    }

                    template <class TIter>
                    bool operator== (const TBaseIterator<TIter>& r) const throw () {
                        return Chunk_ == r.Chunk_ && IndexWithinChunk_ == r.IndexWithinChunk_;
                    }

                    template <class TIter>
                    bool operator!= (const TBaseIterator<TIter>& r) const throw () {
                        return !(*this == r);
                    }

                    TBaseIterator<TChunkIter>& operator++ () throw () {
                        Next();

                        return *this;
                    }

                    TBaseIterator<TChunkIter> operator++ (int) throw () {
                        TBaseIterator<TChunkIter> ret(*this);

                        Next();

                        return ret;
                    }

                    TBaseIterator<TChunkIter>& operator-- () throw () {
                        Prev();

                        return *this;
                    }

                    TBaseIterator<TChunkIter> operator-- (int) throw () {
                        TBaseIterator<TChunkIter> ret(*this);

                        Prev();

                        return ret;
                    }

                    TBaseIterator<TChunkIter>& operator+= (size_t off) throw () {
                        Next(off);

                        return *this;
                    }

                    TBaseIterator<TChunkIter>& operator-= (size_t off) throw () {
                        Prev(off);

                        return *this;
                    }

                    TBaseIterator<TChunkIter>& operator+= (int off) throw () {
                        off > 0 ? Next(off) : Prev(-off);

                        return *this;
                    }

                    TBaseIterator<TChunkIter>& operator-= (int off) throw () {
                        off > 0 ? Prev(off) : Next(-off);

                        return *this;
                    }

                    template<class TOff>
                    TBaseIterator<TChunkIter> operator+ (TOff off) const throw() {
                        TBaseIterator<TChunkIter> ret(*this);
                        ret += off;
                        return ret;
                    }

                    template<class TOff>
                    TBaseIterator<TChunkIter> operator- (TOff off) const throw() {
                        TBaseIterator<TChunkIter> ret(*this);
                        ret -= off;
                        return ret;
                    }

                    char operator* () const throw () {
                        return Chunk_->Data()[IndexWithinChunk_];
                    }

                private:
                    friend class TChunkString;

                    TChunkIter Chunk_;
                    size_t IndexWithinChunk_;
            };

        public:
            typedef TBaseIterator<TChunkList::TConstIterator> TConstIterator;
            typedef TBaseIterator<TChunkList::TIterator> TIterator;
            //////////////////////////////////////////////////////////////////////////

            TChunkString() {}

            TChunkString(const TChunkList& chunkList) {
                Append(chunkList);
            }

            TChunkString(const TChunkString& chunkString) {
                Append(chunkString.ChunkList_);
            }

            TChunkString& operator=(const TChunkString& chunkString) {
                ChunkList_.Clear();
                Append(chunkString);
                return *this;
            }

            void Append(const TChunkList& chunkList) {
                for (TChunkList::TConstIterator chunk = chunkList.Begin(); chunk != chunkList.End(); ++chunk) {
                    if (chunk->Length() > 0) {
                        ChunkList_.Push(chunk->Copy());
                    }
                }
            }

            void Append(const TChunkString& chunkString) {
                Append(chunkString.GetChunks());
            }

            TChunkList& GetChunks() {
                return ChunkList_;
            }

            const TChunkList& GetChunks() const {
                return ChunkList_;
            }

            size_t Length() const {
                return ChunkList_.FullLength();
            }

            TConstIterator Begin() const {
                return TConstIterator(ChunkList_.Begin(), 0);
            }

            TIterator Begin() {
                return TIterator(ChunkList_.Begin(), 0);
            }

            TConstIterator End() const {
                return TConstIterator(ChunkList_.End(), 0);
            }

            TIterator End() {
                return TIterator(ChunkList_.End(), 0);
            }

            TConstIterator Find(char c, TConstIterator begin, TConstIterator end) const {
                return FindImpl(c, begin, end);
            }

            TIterator Find(char c, TIterator begin, TIterator end) {
                return FindImpl(c, begin, end);
            }

            TConstIterator Find(char c, TConstIterator begin) const {
                return FindImpl(c, begin, End());
            }

            TIterator Find(char c, TIterator begin) {
                return FindImpl(c, begin, End());
            }

            TConstIterator Find(char c) const {
                return FindImpl(c, Begin(), End());
            }

            TIterator Find(char c) {
                return FindImpl(c, Begin(), End());
            }

            void CutFront(TIterator cutEnd) {
                if (cutEnd.IndexWithinChunk_ == 0) {
                    TChunkList::Cut(ChunkList_.Begin(), cutEnd.Chunk_);
                } else {
                    TChunkPtr newBegin = cutEnd.Chunk_->SubChunk(cutEnd.IndexWithinChunk_, cutEnd.Chunk_->Length());
                    ++cutEnd.Chunk_;
                    TChunkList::Cut(ChunkList_.Begin(), cutEnd.Chunk_);
                    if (newBegin->Length() > 0) {
                        ChunkList_.PushFront(newBegin.Release());
                    }
                }
            }

            void CutBack(TIterator cutBegin) {
                if (cutBegin.IndexWithinChunk_ == 0) {
                    TChunkList::Cut(cutBegin.Chunk_, ChunkList_.End());
                } else {
                    TChunkPtr newEnd = cutBegin.Chunk_->SubChunk(0, cutBegin.IndexWithinChunk_);
                    TChunkList::Cut(cutBegin.Chunk_, ChunkList_.End());
                    if (newEnd->Length() > 0) {
                        ChunkList_.PushBack(newEnd.Release());
                    }
                }
            }

            void GetData(char* dest) const {
                for (TChunkList::TConstIterator it = ChunkList_.Begin(); it != ChunkList_.End(); ++it) {
                    TCharTraits<char>::Copy(dest, it->Data(), it->Length());
                    dest += it->Length();
                }
            }

            template<class TStroka>
            int Compare(const TStringBase<TStroka, char>& other) const {
                const char* otherData = other.Data();
                size_t otherLen = other.Size();

                for (TChunkList::TConstIterator chunk = ChunkList_.Begin(); chunk != ChunkList_.End(); ++chunk) {
                    size_t chunkLen = chunk->Length();
                    if (otherLen < chunkLen) {
                        int res = TCharTraits<char>::Compare(chunk->Data(), otherData, otherLen);
                        return res == 0 ? 1 : res;
                    }
                    int res = TCharTraits<char>::Compare(chunk->Data(), otherData, chunkLen);
                    if (res != 0) {
                        return res;
                    }
                    otherData += chunkLen;
                    otherLen -= chunkLen;
                }
                return otherLen > 0 ? -1 : 0;
            }

            template<class TStroka>
            bool StartsWith(const TStringBase<TStroka, char>& prefix) const {
                const char* prefixData = prefix.Data();
                size_t prefixLen = prefix.Size();

                for (TChunkList::TConstIterator chunk = ChunkList_.Begin(); prefixLen != 0 && chunk != ChunkList_.End(); ++chunk) {
                    size_t len = Min(chunk->Length(), prefixLen);
                    if (TCharTraits<char>::Compare(chunk->Data(), prefixData, len) != 0) {
                        return false;
                    }
                    prefixData += len;
                    prefixLen -= len;
                }
                return prefixLen == 0;
            }

            template<class TStroka>
            bool operator==(const TStringBase<TStroka, char>& other) const {
                return Compare(other) == 0;
            }

            template<class TStroka>
            bool operator!=(const TStringBase<TStroka, char>& other) const {
                return Compare(other) != 0;
            }

            template<class TStroka>
            bool operator<(const TStringBase<TStroka, char>& other) const {
                return Compare(other) < 0;
            }

            template<class TStroka>
            bool operator<=(const TStringBase<TStroka, char>& other) const {
                return Compare(other) <= 0;
            }

            template<class TStroka>
            bool operator>(const TStringBase<TStroka, char>& other) const {
                return Compare(other) > 0;
            }

            template<class TStroka>
            bool operator>=(const TStringBase<TStroka, char>& other) const {
                return Compare(other) >= 0;
            }

        private:
            template<class TIter>
            static TIter FindImpl(char c, TIter begin, TIter end) {
                const char* found = NULL;

                if (begin.Chunk_ == end.Chunk_) {
                    found = TCharTraits<char>::Find(begin.Chunk_->Data() + begin.IndexWithinChunk_, c,
                        end.IndexWithinChunk_ - begin.IndexWithinChunk_);
                } else {
                    found = TCharTraits<char>::Find(begin.Chunk_->Data() + begin.IndexWithinChunk_, c,
                        begin.Chunk_->Length() - begin.IndexWithinChunk_);

                    while (found == NULL && ++begin.Chunk_ != end.Chunk_) {
                        found = TCharTraits<char>::Find(begin.Chunk_->Data(), c, begin.Chunk_->Length());
                    }

                    if (found == NULL) {
                        found = TCharTraits<char>::Find(end.Chunk_->Data(), c, end.IndexWithinChunk_);
                    }
                }

                if (found == NULL) {
                    return end;
                }

                begin.IndexWithinChunk_ = found - begin.Chunk_->Data();
                return begin;
            }

            TChunkList ChunkList_;
    };
}
