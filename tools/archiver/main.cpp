#include <util/stream/file.h>
#include <util/generic/vector.h>
#include <util/yarchive.h>
#include <util/folder/filelist.h>
#include <util/folder/dirut.h>
#include <util/generic/yexception.h>
#include <util/string/hex.h>
#include <util/config/opt2.h>

#define COLS 10

class THexOutput: public TOutputStream {
    public:
        inline THexOutput(TOutputStream* slave)
            : mC(0)
            , mSlave(slave)
        {
        }

        virtual ~THexOutput() throw () {
        }

    private:
        virtual void DoFinish() {
            mSlave->Write('\n');
            mSlave->Finish();
        }

        virtual void DoWrite(const void* data, size_t len) {
            const char* b = (const char*)data;

            while (len) {
                const unsigned char c = *b;
                char buf[12];
                char* tmp = buf;

                if (mC % COLS == 0) {
                    *tmp++ = ' ';
                    *tmp++ = ' ';
                    *tmp++ = ' ';
                    *tmp++ = ' ';
                }

                if (mC && mC % COLS != 0) {
                    *tmp++ = ',';
                    *tmp++ = ' ';
                }

                *tmp++ = '0';
                *tmp++ = 'x';
                tmp = HexEncode(&c, 1, tmp);

                if ((mC % COLS) == (COLS - 1)) {
                    *tmp++ = ',';
                    *tmp++ = ' ';
                    *tmp++ = '\n';
                }

                mSlave->Write(buf, tmp - buf);

                --len;
                ++b;
                ++mC;
            }
        }

    private:
        ui64 mC;
        TOutputStream* mSlave;
};

static inline TAutoPtr<TOutputStream> OpenOutput(const Stroka& url) {
    if (url.empty()) {
        return new TBuffered<TFileOutput>(8192, Duplicate(1));
    } else {
        return new TBuffered<TFileOutput>(8192, url);
    }
}

static inline bool IsDelim(char ch) throw () {
    return ch == '/' || ch == '\\';
}

static inline Stroka GetFile(const Stroka& s) {
    const char* e = s.end();
    const char* b = s.begin();
    const char* c = e - 1;

    while (c != b && !IsDelim(*c)) {
        --c;
    }

    if (c != e && IsDelim(*c)) {
        ++c;
    }

    return Stroka(c, e - c);
}

static inline Stroka Fix(Stroka f) {
    if (!f.empty() && IsDelim(f[+f - 1])) {
        f.pop_back();
    }

    return f;
}

static bool Quiet = false;

static inline void Append(TArchiveWriter& w, const Stroka& fname, const Stroka& rname) {
    TMappedFileInput in(fname);
    const Stroka key = rname;

    if (!Quiet)
        Cerr << "--> " << key << Endl;

    w.Add(key, &in);
}

struct TRec {
    bool Recursive;
    Stroka Path;
    Stroka Prefix;

    TRec() : Recursive(false), Path(), Prefix()
    {}

    inline void Fix() {
        ::Fix(Path);
        ::Fix(Prefix);
    }

    inline void Recurse(TArchiveWriter& w) const {
        DoRecurse(w, "/");
    }

    inline void DoRecurse(TArchiveWriter& w, Stroka off) const {
        {
            TFileList fl;

            const char* name;
            const Stroka p = Path + off;

            fl.Fill(~p);

            while ((name = fl.Next())) {
                const Stroka fname = p + name;
                const Stroka rname = Prefix + off + name;

                Append(w, fname, rname);
            }
        }

        if (Recursive) {
            TDirsList dl;

            const char* name;
            const Stroka p = Path + off;

            dl.Fill(~p);

            while ((name = dl.Next())) {
                if (strcmp(name, ".") && strcmp(name, "..")) {
                    DoRecurse(w, off + name + "/");
                }
            }
        }
    }
};

int main(int argc, char** argv) {
    Opt2 opt(argc, argv, "rxa:z:q", IntRange(1, 999999), "hexdump=x,recursive=r,quiet=q,prepend=z,append=a");
    bool hexdump         = opt.Has('x', "- produce hexdump");
    const bool recursive = opt.Has('r', "- read all files under each directory, recursively");
    Quiet                = opt.Has('q', "- do not output progress on stderr.");
    const Stroka prepend = opt.Arg('z', "<PREFIX> - prepend string to output verbatim", 0);
    const Stroka append  = opt.Arg('a', "<SUFFIX> - append string to output verbatim", 0);
    opt.AutoUsageErr("<file>..."); // "Files or directories to archive."

    yvector<TRec> recs;
    for (size_t i = 0; i != opt.Pos.size(); ++i) {
        Stroka path = opt.Pos[i];
        size_t off = 0;
#ifdef _win_
        if (path[0] > 0 && isalpha(path[0]) && path[1] == ':')
            off = 2; // skip drive letter ("d:")
#endif // _win_
        const size_t pos = path.find(':', off);
        TRec cur;
        cur.Path = path.substr(0, pos);
        if (pos != Stroka::npos)
            cur.Prefix = path.substr(pos + 1);
        cur.Recursive = recursive;
        cur.Fix();
        recs.push_back(cur);
    }

    try {
        TAutoPtr<TOutputStream> outf(OpenOutput(""));
        TOutputStream* out = outf.Get();
        THolder<TOutputStream> hexout;

        if (hexdump) {
            hexout.Reset(new THexOutput(out));
            out = hexout.Get();
        }

        outf->Write(~prepend, +prepend);

        TArchiveWriter w(out);

        for (yvector<TRec>::const_iterator it = recs.begin(); it != recs.end(); ++it) {
            const TRec& rec = *it;

            if (IsDir(rec.Path)) {
                it->Recurse(w);
            } else {
                Append(w, it->Path, it->Prefix + "/" + GetFile(it->Path));
            }
        }

        w.Finish();
        outf->Write(~append, +append);
        out->Finish();
    } catch (...) {
        Cerr << CurrentExceptionMessage() << Endl;

        return 1;
    }

    return 0;
}
