#pragma once

#include "last_getopt_support.h"

#include <stdarg.h>

#include <util/stream/output.h>
#include <util/generic/ptr.h>
#include <util/generic/stroka.h>
#include <util/generic/list.h>
#include <util/generic/vector.h>
#include <util/generic/hash_set.h>
#include <util/generic/yexception.h>
#include <util/string/split.h>

/// http://wiki.yandex-team.ru/development/poisk/arcadia/util/lastgetopt

namespace NLastGetopt {

enum EHasArg {
    NO_ARGUMENT,
    REQUIRED_ARGUMENT,
    OPTIONAL_ARGUMENT,
    DEFAULT_HAS_ARG = REQUIRED_ARGUMENT
};

enum EArgPermutation {
    REQUIRE_ORDER,
    PERMUTE,
    RETURN_IN_ORDER,
    DEFAULT_ARG_PERMUTATION = PERMUTE
};

void PrintUsageAndExit(const TOptsParser* parser);
void PrintVersionAndExit(const TOptsParser* parser);

class TOpt {
public:
    typedef yvector<char> TShortNames;
    typedef yvector<Stroka> TLongNames;

public:
    TShortNames Chars_;
    TLongNames LongNames_;
    EHasArg HasArg_;
    Stroka ArgTitle_; // used in help
    Stroka Help_;

private:
    typedef TPair<Stroka, bool> TdOptVal;
    typedef yvector<TSharedPtr<IOptHandler> > TOptHandlers;

private:
    bool Required_; // must be specified at least once
    bool Hidden_;
    const void* UserValue_;
    // different from default: used when the option is present without an arg
    TdOptVal OptionalValue_;
    TdOptVal DefaultValue_;
    TOptHandlers Handlers_;

public:
    TOpt()
        : HasArg_(DEFAULT_HAS_ARG)
        , Required_(false)
        , Hidden_(false)
        , UserValue_(NULL)
    {
    }

    Stroka ToShortString() const;

    bool NameIs(const Stroka& name) const;
    bool CharIs(char c) const;
    Stroka GetName() const; // throws
    const TLongNames& GetLongNames() const {
        return LongNames_;
    }
    char GetChar() const; // throws
    char GetCharOr0() const;

    TOpt& HasArg(EHasArg hasArg) {
        HasArg_ = hasArg;
        return *this;
    }

    // make this option required
    TOpt& RequiredArgument(const Stroka& title = "") {
        ArgTitle_ = title;
        return HasArg(REQUIRED_ARGUMENT);
    }

    TOpt& NoArgument() {
        return HasArg(NO_ARGUMENT);
    }

    TOpt& OptionalArgument(const Stroka& title = "") {
        ArgTitle_ = title;
        return HasArg(OPTIONAL_ARGUMENT);
    }


    TOpt& OptionalValue(const Stroka& val, const Stroka& title = "") {
        OptionalValue_ = TdOptVal(val, true);
        return OptionalArgument(title);
    }

    bool HasOptionalValue() const {
        return OPTIONAL_ARGUMENT == HasArg_ && OptionalValue_.second;
    }

    const Stroka& GetOptionalValue() const {
        return OptionalValue_.first;
    }


    TOpt& DefaultValue(const Stroka& val) {
        DefaultValue_ = TdOptVal(val, true);
        return *this;
    }

    bool HasDefaultValue() const {
        return DefaultValue_.second;
    }

    const Stroka& GetDefaultValue() const {
        return DefaultValue_.first;
    }


    TOpt& Required() {
        Required_ = true;
        return *this;
    }

    TOpt& Optional() {
        Required_ = false;
        return *this;
    }

    bool IsRequired() const {
        return Required_;
    }

    TOpt& Hidden() {
        Hidden_ = true;
        return *this;
    }

    bool IsHidden() const {
        return Hidden_;
    }

    TOpt& UserValue(const void* userval) {
        UserValue_ = userval;
        return *this;
    }

    const void* UserValue() const {
        return UserValue_;
    }

private:
    TOpt& HandlerImpl(IOptHandler* handler) {
        Handlers_.push_back(handler);
        return *this;
    }

public:
    template <typename TpFunc>
    TOpt& Handler0(TpFunc func) { // functor taking no parameters
        return HandlerImpl(new NPrivate::THandlerFunctor0<TpFunc>(func));
    }

    template <typename TpFunc>
    TOpt& Handler1(TpFunc func) { // functor taking one parameter
        return HandlerImpl(new NPrivate::THandlerFunctor1<TpFunc>(func));
    }
    template <typename TpArg, typename TpFunc>
    TOpt& Handler1T(TpFunc func) {
        return HandlerImpl(new NPrivate::THandlerFunctor1<TpFunc, TpArg>(func));
    }
    template <typename TpArg, typename TpFunc>
    TOpt& Handler1T(const TpArg &def, TpFunc func) {
        return HandlerImpl(new NPrivate::THandlerFunctor1<TpFunc, TpArg>(func, def));
    }
    template <typename TpArg, typename TpArg2, typename TpFunc>
    TOpt& Handler1T(const TpArg2 &def, TpFunc func) {
        return HandlerImpl(new NPrivate::THandlerFunctor1<TpFunc, TpArg>(func, def));
    }

    TOpt& Handler(void (*f)()) {
        return Handler0(f);
    }
    TOpt& Handler(void (*f)(const TOptsParser*)) {
        return Handler1(f);
    }

    TOpt& Handler(TAutoPtr<IOptHandler> handler) {
        return HandlerImpl(handler.Release());
    }

    template <typename T> // T extends IOptHandler
    TOpt& Handler(TAutoPtr<T> handler) {
        return HandlerImpl(handler.Release());
    }

    // Stores FromString<T>(arg) in *target
    // T maybe anything with FromString<T>(const TStringBuf&) defined
    template <typename T>
    TOpt& StoreResult(T* target) {
        return Handler1T<T>(NPrivate::TStoreResultFunctor<T>(target));
    }

    template <typename T, typename TpDef>
    TOpt& StoreResult(T* target, const TpDef& def) {
        return Handler1T<T>(def, NPrivate::TStoreResultFunctor<T>(target));
    }

    // Stores given value in *target if the option is present.
    // T must be a copyable type, assignable from TValue.
    template <typename T, typename TValue>
    TOpt& StoreValue(T* target, const TValue& value) {
        return Handler1(NPrivate::TStoreValueFunctor<T>(target, value));
    }

    // Appends FromString<T>(arg) to *target for each argument
    template <typename T>
    TOpt& AppendTo(yvector<T>* target) {
        return Handler1T<T>(NPrivate::TAppendToFunctor<T>(target));
    }

    TOpt& Help(const Stroka& help) {
        Help_ = help;
        return *this;
    }

    void FireHandlers(const TOptsParser*) const;
};

class TOptsParseResult;
class TOptsParser;

/// description of options
class TOpts {
    friend class TOptsParseResult;
    friend class TOptsParser;

public:
    typedef yvector<TSharedPtr<TOpt> > TOptsVector;
    TOptsVector Opts_;
    EArgPermutation ArgPermutation_;
    bool AllowSingleDashForLong_; // ignored (= true) if no short options, false by default
    bool AllowPlusForLong_; // for compatibility with Opt
    bool AllowUnknownCharOptions_;
    bool AllowUnknownLongOptions_;
    // XXX: support arg names (as in Opt2)
    ui32 FreeArgsMin_;
    ui32 FreeArgsMax_;
    yvector<Stroka> FreeArgTitles_;

    // as in getopt(3)
    TOpts(const TStringBuf& optstring = TStringBuf());

    static TOpts Default() {
        TOpts opts;
        opts.AddHelpOption();
        opts.AddVersionOption();
        return opts;
    }

    void Validate() const;

    const TOpt* FindLongOption(const TStringBuf& name) const;
    const TOpt* FindCharOption(char c) const;
    TOpt* FindLongOption(const TStringBuf& name);
    TOpt* FindCharOption(char c);
private:
    Stroka GetFreeArgTitle(size_t pos) const;
public:
    bool HasLongOption(const Stroka& name) const {
        return FindLongOption(name) != 0;
    }
    bool HasCharOption(char c) const {
        return FindCharOption(c) != 0;
    }
    const TOpt& GetLongOption(const TStringBuf& name) const;
    TOpt& GetLongOption(const TStringBuf& name);
    const TOpt& GetCharOption(char c) const;
    TOpt& GetCharOption(char c);

    bool HasAnyShortOption() const;
    bool HasAnyLongOption() const;

    TOpt& AddOption(const TOpt& option);

    void AddCharOptions(const TStringBuf& optstring);

    TOpt& AddCharOption(char c, const Stroka& help = "") {
        return AddCharOption(c, DEFAULT_HAS_ARG, help);
    }

    TOpt& AddCharOption(char c, EHasArg hasArg, const Stroka& help = "") {
        TOpt option;
        option.Chars_.push_back(c);
        option.Help_ = help;
        option.HasArg_ = hasArg;
        return AddOption(option);
    }

    TOpt& AddLongOption(const Stroka& name, const Stroka& help = "") {
        return AddLongOption(0, name, help);
    }

    TOpt& AddLongOption(char c, const Stroka& name, const Stroka& help = "") {
        TOpt option;
        if (c != 0)
            option.Chars_.push_back(c);
        option.LongNames_.push_back(name);
        option.Help_ = help;
        return AddOption(option);
    }

    // standard --help option, support multiple chars
    TOpt& AddHelpOption(char c = '?') {
        if (TOpt* o = FindLongOption("help")) {
            if (!o->CharIs(c))
                o->Chars_.push_back(c);
            return *o;
        }
        return AddLongOption(c, "help", "print usage")
            .HasArg(NO_ARGUMENT)
            .Handler(&PrintUsageAndExit);
    }
    // --svnrevision
    TOpt& AddVersionOption(char c = 'V') {
        if (TOpt* o = FindLongOption("svnrevision")) {
            if (!o->CharIs(c))
                o->Chars_.push_back(c);
            return *o;
        }
        return AddLongOption(c, "svnrevision", "print svn version")
            .HasArg(NO_ARGUMENT)
            .Handler(&PrintVersionAndExit);
    }
public:

    // get or create
    TOpt& CharOption(char c) {
        const TOpt* opt = FindCharOption(c);
        if (opt != 0) {
            return const_cast<TOpt&>(*opt);
        } else {
            AddCharOption(c);
            return const_cast<TOpt&>(GetCharOption(c));
        }
    }

    size_t IndexOf(const TOpt* opt) const;

    void SetFreeArgsMin(size_t n) {
        FreeArgsMin_ = (ui32)n;
    }
    void SetFreeArgsMax(size_t n) {
        FreeArgsMax_ = (ui32)n;
    }
    void SetFreeArgsNum(size_t n) {
        FreeArgsMin_ = (ui32)n;
        FreeArgsMax_ = (ui32)n;
    }
    void SetFreeArgTitle(size_t pos, const Stroka& title);

    void SetAllowSingleDashForLong(bool value) {
        AllowSingleDashForLong_ = value;
    }

    void PrintUsage(const TStringBuf& program, TOutputStream& os = Cerr) const;
};

class TOptsParser {
    friend class TOpt;

    enum EIsOpt {
        EIO_NONE,
        EIO_SDASH,
        EIO_DDASH,
        EIO_PLUS,
    };

public: // XXX: make private
    const TOpts* Opts_;

    size_t Argc_;
    const char** Argv_;

private:
    TCopyPtr<TOpt> TempCurrentOpt_;

public:
    Stroka ProgramName_;

    size_t Pos_; // current element withing argv
    size_t Sop_; // current char within arg
    bool Stopped_;
    bool GotMinusMinus_;

protected:
    const TOpt* CurrentOpt_;
    TStringBuf CurrentValue_;

private:
    typedef yhash_set<const TOpt *> TdOptSet;
    TdOptSet OptsSeen_;
    ylist<const TOpt *> OptsDefault_;

private:
    void Init(const TOpts* options, int argc, const char* argv[]);
    void Init(const TOpts* options, int argc, char* argv[]);

    bool CommitEndOfOptions(size_t pos);
    bool Commit(const TOpt* currentOption, const TStringBuf& currentValue, size_t pos, size_t sop);

    bool ParseShortOptArg(size_t pos);
    bool ParseOptArg(size_t pos);
    bool ParseOptParam(const TOpt* opt, size_t pos);
    bool ParseUnknownShortOptWithinArg(size_t pos, size_t sop);
    bool ParseShortOptWithinArg(size_t pos, size_t sop);
    bool ParseWithPermutation();

    bool DoNext();
    void Finish();

    EIsOpt IsOpt(const TStringBuf& arg) const;

    void Swap(TOptsParser& that);

public:
    TOptsParser(const TOpts* options, int argc, const char* argv[]) {
        Init(options, argc, argv);
    }

    TOptsParser(const TOpts* options, int argc, char* argv[]) {
        Init(options, argc, argv);
    }

    /// fetch next argument, false if no more arguments left
    bool Next();

public:
    const TOpt* CurOpt() const {
        return CurrentOpt_;
    }
    const char* CurVal() const {
        return CurrentValue_.data();
    }
    const TStringBuf& CurValStr() const {
        return CurrentValue_;
    }
    TStringBuf CurValOrOpt() const {
        TStringBuf val(CurValStr());
        if (!val.IsInited() && CurOpt()->HasOptionalValue())
            val = CurOpt()->GetOptionalValue();
        return val;
    }
    TStringBuf CurValOrDef(bool useDef = true) const {
        TStringBuf val(CurValOrOpt());
        if (!val.IsInited() && useDef && CurOpt()->HasDefaultValue())
            val = CurOpt()->GetDefaultValue();
        return val;
    }

    // true if this option was actually specified by the user
    bool IsExplicit() const {
        return NULL == CurrentOpt_ || !OptsSeen_.empty();
    }

public:
    bool CurrentIs(const Stroka& name) const {
        return CurOpt()->NameIs(name);
    }

public:
    const Stroka& ProgramName() const {
        return ProgramName_;
    }
    void PrintUsage(TOutputStream& os = Cerr) const {
        Opts_->PrintUsage(ProgramName(), os);
    }
};

class TOptParseResult {
public:
    typedef yvector<const char *> TValues;

public:
    TOptParseResult(const TOpt* opt = NULL)
        : Opt_(opt)
    {
    }
public:
    const TOpt& Opt() const {
        return *Opt_;
    }
    const TOpt* OptPtr() const {
        return Opt_;
    }
    const TValues& Values() const {
        return Values_;
    }
    bool Empty() const {
        return Values().empty();
    }
    size_t Count() const {
        return Values_.size();
    }
    void AddValue(const char* val) {
        if (NULL != val)
            Values_.push_back(val);
    }
    const char *DefVal(const char *def = NULL) const {
        return Opt().HasDefaultValue() ? Opt().GetDefaultValue().c_str() : def;
    }
    const char *Front(const char *def = NULL) const {
        return Empty() ? DefVal(def) : Values().front();
    }
    const char *Back(const char *def = NULL) const {
        return Empty() ? DefVal(def) : Values().back();
    }

private:
    const TOpt* Opt_;
    TValues Values_;
};

class TOptsParseResult {
private:
    THolder<TOptsParser> Parser_;
    // XXX: make argc, argv
    typedef yvector<TOptParseResult> TdVec;
    TdVec Opts_;
    TdVec OptsDef_;

private:
    TOptParseResult& OptParseResult();
    static const TOptParseResult* FindParseResult(const TdVec& vec, const TOpt* opt);

public:
    const TOptParseResult* FindOptParseResult(const TOpt* opt, bool includeDefault = false) const;
    const TOptParseResult* FindLongOptParseResult(const Stroka& name, bool includeDefault = false) const;
    const TOptParseResult* FindCharOptParseResult(char c, bool includeDefault = false) const;

protected:
    void Init(const TOpts* options, int argc, const char** argv);

    TOptsParseResult() { }

public:
    virtual void HandleError() const;

public:
    TOptsParseResult(const TOpts* options, int argc, const char* argv[]) {
        Init(options, argc, argv);
    }
    TOptsParseResult(const TOpts* options, int argc, char* argv[]) {
        Init(options, argc, const_cast<const char**>(argv));
    }

    virtual ~TOptsParseResult() { }
public:
    Stroka GetProgramName() const;
    size_t GetFreeArgsPos() const;
    yvector<Stroka> GetFreeArgs() const;

    bool Has(const TOpt* opt, bool includeDefault = false) const;
    const char* Get(const TOpt* opt, bool includeDefault = true) const;
    const char* GetOrElse(const TOpt* opt, const char* defaultValue) const;

    bool Has(const Stroka& name, bool includeDefault = false) const;
    const char* Get(const Stroka& name, bool includeDefault = true) const;
    const char* GetOrElse(const Stroka& name, const char* defaultValue) const;

    bool Has(char name, bool includeDefault = false) const;
    const char* Get(char name, bool includeDefault = true) const;
    const char* GetOrElse(char name, const char* defaultValue) const;

    template <typename T, typename TKey>
    T Get(const TKey opt) const {
        const char* value = Get(opt);
        try {
            return NPrivate::OptFromString<T>(value, opt);
        } catch (...) {
            HandleError();
            throw;
        }
    }

    template <typename T, typename TKey>
    T GetOrElse(const TKey opt, const T& defaultValue) const {
        if (Has(opt))
            return Get<T>(opt);
        else
            return defaultValue;
    }
};

/// parse result of options, throws exception on error
class TOptsParseResultException: public TOptsParseResult {
    yvector<const char*> Argv_;
public:
    TOptsParseResultException(const TOpts* opts, int argc, const char* argv[]) {
        Init(opts, argc, argv);
    }

    TOptsParseResultException(const TOpts* opts, yvector<const char*> argv)
        : Argv_(argv)
    {
        Init(opts, (int)+Argv_, ~Argv_);
    }

    // override
    void HandleError() const;

};

/// Handler to split option value by delimiter to a vector.
template<class T>
struct TOptSplitHandler: public IOptHandler {
private:
    yvector<T>* Target;
    char Delim;
public:
    explicit TOptSplitHandler(yvector<T>* target, const char delim)
        : Target(target)
        , Delim(delim)
    {
    }

    virtual void HandleOpt(const TOptsParser* parser) {
        const TStringBuf curval(parser->CurValOrDef());
        if (curval.IsInited()) {
            TConsumer cons = { parser->CurOpt(), *Target };
            SplitStringTo(curval, Delim, &cons);
        }
    }

private:
    struct TConsumer {
        const TOpt *CurOpt;
        yvector<T>& Target;
        typedef TStringBuf value_type;
        void push_back(const TStringBuf& val) {
            Target.push_back(NPrivate::OptFromString<T>(val, CurOpt));
        }
    };
};

namespace NPrivate {

    template <typename TpFunc, typename TpArg>
    void THandlerFunctor1<TpFunc, TpArg>::HandleOpt(const TOptsParser* parser) {
        const TStringBuf curval = parser->CurValOrDef(!HasDef_);
        Func_(curval.IsInited()
            ? OptFromString<TpArg>(curval, parser->CurOpt()) : Def_);
    }

    template <typename T>
    static inline T OptFromStringImpl(const TStringBuf& value) {
        return FromString<T>(value);
    }

    template <>
    inline TStringBuf OptFromStringImpl<TStringBuf>(const TStringBuf& value) {
        return value;
    }

    template <>
    inline const char* OptFromStringImpl<const char*>(const TStringBuf& value) {
        return value.data();
    }

    template <typename T, typename TSomeOpt>
    T OptFromString(const TStringBuf& value, const TSomeOpt opt) {
        try {
            return OptFromStringImpl<T>(value);
        } catch (...) {
            ythrow TUsageException() << "failed to parse opt " << OptToString(opt) << " value " << Stroka(value).Quote() << ": " << CurrentExceptionMessage();
        }
    }

} // NPrivate

} // NLastGetopt
