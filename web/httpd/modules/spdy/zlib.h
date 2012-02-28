#pragma once

#include <web/httpd/kernel/chunks.h>

#include <util/generic/hash.h>
#include <util/generic/strbuf.h>
#include <util/generic/singleton.h>
#include <util/generic/yexception.h>

#include <contrib/libs/zlib/zlib.h>

namespace NSrvKernel {
    struct TDicts {
        struct TDict: public TStringBuf {
            inline TDict(const TStringBuf& str)
                : TStringBuf(str)
            {
            }

            inline uLong Id() const throw () {
                return adler32(adler32(0L, Z_NULL, 0), (const Bytef*)c_str(), size());
            }
        };

        inline TDicts() {
            const TStringBuf dicts[] = {
                STRINGBUF(
                    "optionsgetheadpostputdeletetraceacceptaccept-charsetaccept-encodingaccept-"
                    "languageauthorizationexpectfromhostif-modified-sinceif-matchif-none-matchi"
                    "f-rangeif-unmodifiedsincemax-forwardsproxy-authorizationrangerefererteuser"
                    "-agent10010120020120220320420520630030130230330430530630740040140240340440"
                    "5406407408409410411412413414415416417500501502503504505accept-rangesageeta"
                    "glocationproxy-authenticatepublicretry-afterservervarywarningwww-authentic"
                    "ateallowcontent-basecontent-encodingcache-controlconnectiondatetrailertran"
                    "sfer-encodingupgradeviawarningcontent-languagecontent-lengthcontent-locati"
                    "oncontent-md5content-rangecontent-typeetagexpireslast-modifiedset-cookieMo"
                    "ndayTuesdayWednesdayThursdayFridaySaturdaySundayJanFebMarAprMayJunJulAugSe"
                    "pOctNovDecchunkedtext/htmlimage/pngimage/jpgimage/gifapplication/xmlapplic"
                    "ation/xhtmltext/plainpublicmax-agecharset=iso-8859-1utf-8gzipdeflateHTTP/1"
                    ".1statusversionurl\0")
            };

            for (size_t i = 0; i < ARRAY_SIZE(dicts); ++i) {
                const TDict dict(dicts[i]);

                Dicts.insert(MakePair(dict.Id(), dict));
            }
        }

        inline const TStringBuf* Find(uLong id) const throw () {
            TDictsMap::const_iterator it = Dicts.find(id);

            if (it == Dicts.end()) {
                return 0;
            }

            return &it->second;
        }

        inline const TStringBuf& Default() const throw () {
            return Dicts.begin()->second;
        }

        static inline const TDicts& Instance() throw () {
            return *Singleton<TDicts>();
        }

        typedef yhash<uLong, TDict> TDictsMap;
        TDictsMap Dicts;
    };

    struct TInflate {
        inline TInflate() {
            Zero(Ctx);

            const int succ = inflateInit(&Ctx);

            if (succ != Z_OK) {
                ythrow yexception() << "can not init inflate engine(" << succ << ")";
            }
        }

        inline ~TInflate() throw () {
            inflateEnd(&Ctx);
        }

        inline void Decompress(TChunkList& lst) {
            TChunkList tmp;

            Decompress(lst, tmp);
            tmp.Swap(lst);
        }

        inline void Decompress(const TChunkList& from, TChunkList& to) {
            TChunkPtr chunk(NewChunk(from.FullLength() * 4));

            Ctx.next_out = (Bytef*)chunk->Data();
            Ctx.avail_out = chunk->Length();

            for (TChunkList::TConstIterator it = from.Begin(); it != from.End(); ++it) {
                Ctx.next_in = (Bytef*)it->Data();
                Ctx.avail_in = it->Length();

                int rv = inflate(&Ctx, Z_SYNC_FLUSH);

                if (rv == Z_NEED_DICT) {
                    const TStringBuf* dict = TDicts::Instance().Find(Ctx.adler);

                    if (dict) {
                        rv = inflateSetDictionary(&Ctx, (const Bytef*)dict->c_str(), dict->size());

                        if (rv == Z_OK) {
                            rv = inflate(&Ctx, Z_SYNC_FLUSH);
                        }
                    }
                }

                if (rv != Z_OK) {
                    ythrow yexception() << "inflate error(" << rv << ")";
                }
            }

            chunk->Shrink((char*)Ctx.next_out - chunk->Data());

            to.Push(chunk);
        }

        z_stream Ctx;
    };

    struct TDeflate {
        inline TDeflate() {
            Zero(Ctx);

            int success = deflateInit2(&Ctx, 9, Z_DEFLATED, 11, 1, Z_DEFAULT_STRATEGY);

            if (success == Z_OK) {
                const TStringBuf& dict = TDicts::Instance().Default();

                success = deflateSetDictionary(&Ctx, (const Bytef*)~dict, +dict);
            }

            if (success != Z_OK) {
                ythrow yexception() << "deflate init error(" << success << ")";
            }
        }

        inline ~TDeflate() throw () {
            deflateEnd(&Ctx);
        }

        inline void Compress(TChunkList& from, TChunkList& to) {
            TChunkPtr chunk(NewChunk(deflateBound(&Ctx, from.FullLength())));

            Ctx.next_out = (Bytef*)chunk->Data();
            Ctx.avail_out = chunk->Length();

            for (TChunkList::TConstIterator it = from.Begin(); it != from.End(); ++it) {
                Ctx.next_in = (Bytef*)it->Data();
                Ctx.avail_in = it->Length();

                const int rv = deflate(&Ctx, Z_NO_FLUSH);

                if (rv != Z_OK) {
                    ythrow yexception() << "deflate error(" << rv << ")";
                }
            }

            {
                const int rv = deflate(&Ctx, Z_SYNC_FLUSH);

                if (rv != Z_OK) {
                    ythrow yexception() << "deflate flush error(" << rv << ")";
                }
            }

            chunk->Shrink((char*)Ctx.next_out - chunk->Data());

            to.Push(chunk);
        }

        inline void Compress(TChunkList& lst) {
            TChunkList tmp;

            Compress(lst, tmp);
            lst.Swap(tmp);
        }

        z_stream Ctx;
    };
}
