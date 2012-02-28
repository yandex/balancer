#pragma once

#include <util/system/defaults.h>
#include <util/string/cast.h>
#include <util/stream/ios.h>
#include <util/logger/all.h>
#include <util/generic/map.h>
#include <util/generic/ptr.h>
#include <util/generic/stack.h>
#include <util/generic/stroka.h>
#include <util/generic/vector.h>
#include <util/generic/yexception.h>
#include <util/str_stl.h>

class TSectionDesc;

class TYandexConfig: public TRefCounted<TYandexConfig>
{
public:
    class Directives;
    typedef ymap<const char*, const char*, ci_less> SectionAttrs;
    struct Section;
    typedef ymultimap<stroka, Section*> TSectionsMap;

    struct Section {
        const char* Name;
        SectionAttrs Attrs;
        Directives* Cookie;
        Section* Parent;
        Section* Next;
        Section* Child;
        bool Owner;
        TSectionDesc* Desc;

        Section()
            : Name("")
            , Cookie(0)
            , Parent(0)
            , Next(0)
            , Child(0)
            , Owner(false)
            , Desc(0)
        {
        }

        Directives& GetDirectives() {
            assert(Cookie);
            return *Cookie;
        }

        const Directives& GetDirectives() const {
            assert(Cookie);
            return *Cookie;
        }

        bool Parsed() const {
            return Cookie != 0;
        }

        TSectionsMap GetAllChildren() const;
    };
public:
    TYandexConfig() : FileData(0) {
        Clear();
    }
    virtual ~TYandexConfig() {
        Clear();
    }
    bool Read(const char* path);
    bool ReadMemory(const char* buffer);
    bool Parse(const char* path, bool process_directives = true);
    bool ParseMemory(const char* buffer, bool process_directives = true);
    bool ParseSection(const char* SecName, const char* idname = 0, const char* idvalue = 0);
    void AddSection(Section* sec);
    void Clear();
    void ReportError(const char* ptr, const char* err, bool warning = false);
    void ReportError(const char* ptr, bool warning, const char* format, ...);
    void PrintErrors(TLog* Log);
    void PrintErrors(Stroka& Err);
    Section* GetFirstChild(const char* Name, Section* CurSection = NULL);
    const char* GetConfigPath() const {
        return ~ConfigPath;
    }
    Section* GetRootSection() {
        assert(!AllSections.empty());
        return AllSections[0];
    }
    const Section* GetRootSection() const {
        assert(!AllSections.empty());
        return AllSections[0];
    }
    void PrintConfig(TOutputStream& os) const;
protected:
    //the followind three functions return 'false' only for fatal errors to break the parsing
    virtual bool AddKeyValue(Section& sec, const char* key, const char* value);
    virtual bool OnBeginSection(Section& sec); //keep sec.Cookie==0 to skip the section
    virtual bool OnEndSection(Section& sec);
private:
    bool PrepareLines();
    void ProcessComments();
    bool ProcessRoot(bool process_directives);
    bool ProcessAll(bool process_directives);
    bool ProcessBeginSection();
    bool ProcessEndSection();
    bool ProcessDirective();
    void ProcessLineBreak(char*& LineBreak, char toChange);
    bool FindEndOfSection(const char* SecName, const char* begin, char*& endsec, char*& endptr);
private:
    char* FileData;
    ui32 Len;
    char* CurrentMemoryPtr;
    ystack<Section*> CurSections;
    yvector<Section*> AllSections;
    yvector<Stroka> Errors;
    yvector<const char*> EndLines;
    Stroka ConfigPath;
    DECLARE_NOCOPY(TYandexConfig);
};

class TYandexConfig::Directives: public ymap<stroka, const char*>
{
protected:
    Directives(bool isStrict)
        : strict(isStrict)
    {
    }

public:
    Directives()
        : strict(true)
    {
    }

    virtual ~Directives() {
    }

    bool IsStrict() const
    {
        return strict;
    }

    bool AddKeyValue(const char* key, const char* value);

    bool GetValue(const char* key, Stroka& value) const;
    bool GetValue(const char* key, bool& value) const;

    template <class T>
    inline bool GetValue(const char* key, T& value) const {
        Stroka tmp;

        if (GetValue(key, tmp)) {
            value = FromString<T>(tmp);

            return true;
        }

        return false;
    }

    template <class T>
    inline T Value(const char* key, T def) const {
        GetValue(key, def);
        return def;
    }

    void FillArray (const char* key, yvector<Stroka>& values) const;
    void Clear();

    void declare(const char* directive_name) {
        insert(value_type(directive_name, NULL));
    }

    virtual bool CheckOnEnd(TYandexConfig& yc, TYandexConfig::Section& sec);

protected:
    bool strict;
};

#define DECLARE_CONFIG(ConfigClass)                \
    class ConfigClass : public TYandexConfig {     \
      public:                                      \
        ConfigClass() : TYandexConfig() {}         \
      protected:                                   \
        virtual bool OnBeginSection(Section& sec); \
      private:                                     \
        ConfigClass(const ConfigClass&);           \
        ConfigClass& operator=(const ConfigClass&);\
    };

#define DECLARE_SECTION(SectionClass)                                            \
    class SectionClass : public TYandexConfig::Directives {                      \
      public:                                                                    \
        SectionClass();                                                          \
    };                                                                           \

#define DECLARE_SECTION_CHECK(SectionClass)                                      \
    class SectionClass : public TYandexConfig::Directives {                      \
      public:                                                                    \
        SectionClass();                                                          \
        bool CheckOnEnd(TYandexConfig& yc, TYandexConfig::Section& sec);         \
    };                                                                           \

#define BEGIN_CONFIG(ConfigClass)                                                \
    bool ConfigClass::OnBeginSection(Section& sec) {                             \
        if (sec.Parent == &sec) /* it's root */ {                                \
            assert (*sec.Name == 0);                                             \
            /* do not allow any directives at root */                            \
            sec.Cookie = new TYandexConfig::Directives;                          \
            sec.Owner = true;                                                    \
            return true;                                                         \
        }

#define BEGIN_TOPSECTION2(SectionName, DirectivesClass)                          \
        if (*sec.Parent->Name == 0) { /* it's placed at root */                  \
            if (stricmp(sec.Name, #SectionName) == 0) {                          \
                sec.Cookie = new DirectivesClass;                                \
                sec.Owner = true;                                                \
                return true;                                                     \
            }                                                                    \
        } else if (stricmp(sec.Parent->Name, #SectionName) == 0) {

#define BEGIN_SUBSECTION(SectionName, SubSectionName)                            \
        if (stricmp(sec.Parent->Name, #SubSectionName) == 0                      \
            && stricmp(sec.Parent->Parent->Name, #SectionName) == 0)             \
        {

#define SUBSECTION2(SubSectionName, DirectivesClass)                             \
            if (stricmp(sec.Name, #SubSectionName) == 0) {                       \
                sec.Cookie = new DirectivesClass;                                \
                sec.Owner = true;                                                \
                return true;                                                     \
            }

#define FAKESECTION(SubSectionName)                                              \
            if (stricmp(sec.Name, #SubSectionName) == 0) {                       \
                assert(sec.Cookie == 0);                                         \
                return true;                                                     \
            }

#define END_SECTION()                                                            \
        }

#define END_CONFIG()                                                             \
        if (!sec.Parent->Parsed())                                               \
            return true;                                                         \
        ReportError(sec.Name, true, "section \'%s\' not allowed here and will be ignored", sec.Name); \
        return true;                                                             \
    }

#define SUBSECTION(SectionName)       SUBSECTION2(SectionName, SectionName)
#define BEGIN_TOPSECTION(SectionName) BEGIN_TOPSECTION2(SectionName, SectionName)

#define BEGIN_SECTION(SectionClass)                          \
    SectionClass::SectionClass() {
#define DEFINE_SECTION(SectionClass)                         \
    class SectionClass : public TYandexConfig::Directives {  \
        public:                                              \
            SectionClass() {
#define DIRECTIVE(DirectiveName) declare(#DirectiveName);
#define END_DEFINE_SECTION }};
#define END_DEFINE_SECTION_CHECK } bool CheckOnEnd(TYandexConfig& yc, TYandexConfig::Section& sec); };

#define DEFINE_INDEFINITE_SECTION(SectionClass)              \
    class SectionClass : public TYandexConfig::Directives { public: SectionClass() { strict = false; } };

#define BEGIN_SECTION_CHECK(SectionClass)                                                      \
    bool SectionClass::CheckOnEnd(TYandexConfig& yc, TYandexConfig::Section& sec) {            \
        (void)yc;                                                                              \
        (void)sec;                                                                             \
        SectionClass& type = *this;                                                            \
        (void)type;

#   define DIR_ABSENT(DirectiveName)       (type[#DirectiveName] == 0)
#   define DIR_ARG_ABSENT(DirectiveName)   (type[#DirectiveName] == 0 || *(type[#DirectiveName]) == 0)
#   define DIR_PRESENT(DirectiveName)      (type[#DirectiveName] != 0)
#   define DIR_ARG_PRESENT(DirectiveName)  (type[#DirectiveName] != 0 && *(type[#DirectiveName]) != 0)

#define DIRECTIVE_MUSTBE(DirectiveName)                                                         \
    if (DIR_ARG_ABSENT(DirectiveName)) {                                                        \
        yc.ReportError(sec.Name, true,                                                          \
            "Section \'%s\' must include directive \'%s\'. Section will be ignored",            \
            sec.Name, #DirectiveName);                                                          \
        return false;                                                                           \
    }

#define DIRECTIVE_ATLEAST(DirectiveName1, DirectiveName2)                                       \
    if (DIR_ARG_ABSENT(DirectiveName1) && DIR_ARG_ABSENT(DirectiveName2)) {                     \
        yc.ReportError(sec.Name, true,                                                          \
            "Section \'%s\' must include directives \'%s\' or \'%s\'. Section will be ignored", \
            sec.Name, #DirectiveName1, #DirectiveName2);                                        \
        return false;                                                                           \
    }

#define DIRECTIVE_BOTH(DirectiveName1, DirectiveName2)                                          \
    if (DIR_ARG_ABSENT(DirectiveName1) && DIR_ARG_PRESENT(DirectiveName2)                       \
     || DIR_ARG_ABSENT(DirectiveName2) && DIR_ARG_PRESENT(DirectiveName1)) {                    \
        yc.ReportError(sec.Name, true,                                                          \
            "Section \'%s\' must include directives \'%s\' and \'%s\' simultaneously. Section will be ignored", \
            sec.Name, #DirectiveName1, #DirectiveName2);                                        \
        return false;                                                                           \
    }

#define END_SECTION_CHECK()                                                                       \
        return true;                                                                            \
    }

class config_exception : public yexception
{
public:
    config_exception(const char* fp) {
        filepoint = fp;
    }
    const char *where() const throw() {
        return filepoint;
    }
private:
    const char* filepoint;
};

#define DEFINE_UNSTRICT_SECTION(SectionClasse)\
class SectionClasse \
    : public TYandexConfig::Directives { \
public : \
    SectionClasse(const TYandexConfig::Directives &obj) \
        : TYandexConfig::Directives(obj) { strict = false; } \
    SectionClasse() { \
        strict = false;

DEFINE_UNSTRICT_SECTION(AnyDirectives)
    END_DEFINE_SECTION;

#define EMBEDDED_CONFIG(SectionName)\
    if (sec.Parent != &sec) /* not root not placed at root */{\
        Section* parent=sec.Parent; \
        while (*parent->Name != 0) { /* any child of SectionName */\
            if (stricmp(parent->Name, #SectionName) == 0) {\
                sec.Cookie = new AnyDirectives();\
                sec.Owner = true; \
                return true;\
            }\
            parent = parent->Parent;\
        }\
    }

#define ONE_EMBEDDED_CONFIG(SectionName)\
    if (sec.Parent != &sec) /* not root not placed at root */{\
        Section* parent=sec.Parent; \
        while (*parent->Name != 0) { /* any child of SectionName */\
            if (stricmp(parent->Name, #SectionName) == 0) {\
                if (!parent->Child->Next) {\
                    sec.Cookie = new AnyDirectives();\
                    sec.Owner = true; \
                    return true;\
                } else {\
                    break;\
                }\
            }\
            parent = parent->Parent;\
        }\
    }
