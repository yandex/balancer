#pragma once

#include "pire.h"

namespace NRegExp {
    class TMatcher;

    class TFsm {
            friend class TMatcher;
        public:
            struct TOptions {
                inline TOptions()
                    : CaseInsensitive(false)
                    , Surround(false)
                {
                }

                inline TOptions& SetCaseInsensitive(bool v) {
                    CaseInsensitive = v;
                    return *this;
                }

                inline TOptions& SetSurround(bool v) {
                    Surround = v;
                    return *this;
                }

                bool CaseInsensitive;
                bool Surround;
            };

            inline TFsm(const Stroka& regexp, const TOptions& opts = TOptions())
                : Scanner_(Parse(regexp, opts).Compile<NPire::TNonrelocScanner>())
            {
            }

            static inline TFsm False() {
                return TFsm(NPire::TFsm::MakeFalse().Compile<NPire::TNonrelocScanner>());
            }

            static inline TFsm Glue(const TFsm&l, const TFsm& r) {
                return NPire::TNonrelocScanner::Glue(l.Scanner_, r.Scanner_);
            }

        private:
            inline TFsm(const NPire::TNonrelocScanner& compiled)
                : Scanner_(compiled)
            {
                if (Scanner_.Empty())
                    ythrow yexception() << "Can't create fsm with empty scanner";
            }

            static inline NPire::TFsm Parse(const Stroka& regexp, const TOptions& opts) {
                NPire::TLexer lexer;
                lexer.Assign(regexp.begin(), regexp.end());

                if (opts.CaseInsensitive) {
                    lexer.AddFeature(NPire::NFeatures::CaseInsensitive());
                }

                NPire::TFsm ret = lexer.Parse();

                if (opts.Surround) {
                    ret.Surround();
                }

                ret.Determine();

                return ret;
            }

        private:
            NPire::TNonrelocScanner Scanner_;
    };

    static inline TFsm operator | (const TFsm& l, const TFsm& r) {
        return TFsm::Glue(l, r);
    }

    class TMatcher {
        public:
            inline TMatcher(const TFsm& fsm)
                : Fsm_(fsm)
            {
                Fsm_.Scanner_.Initialize(State_);
            }

            inline bool Final() const throw () {
                return Fsm_.Scanner_.Final(State_);
            }

            inline TMatcher& Match(const char* data, size_t len) throw () {
                NPire::Run(Fsm_.Scanner_, State_, data, data + len);

                return *this;
            }

            inline const char* Find(const char* b, const char* e) throw () {
                return NPire::ShortestPrefix(Fsm_.Scanner_, b, e);
            }

            inline TMatcher& Match(const Stroka& s) throw () {
                return Match(~s, +s);
            }

            typedef TPair<const size_t*, const size_t*> TMatchedRegexps;

            inline TMatchedRegexps MatchedRegexps() const throw () {
                return Fsm_.Scanner_.AcceptedRegexps(State_);
            }

        private:
            const TFsm& Fsm_;
            NPire::TNonrelocScanner::State State_;
    };
}
