#pragma once

#include <util/string/cast.h>
#include <util/generic/stroka.h>
#include <util/generic/vector.h>
#include <util/generic/yexception.h>

class TInputStream;

namespace NSrvKernel {
    class TConfigParseError: public yexception {
    };

    class IConfig {
    public:
        class IValue {
        public:
            virtual Stroka AsString() = 0;
            virtual bool AsBool() = 0;
            virtual IConfig* AsSubConfig() = 0;
        };

        class IFunc {
        public:
            inline void Consume(const Stroka& key, IValue* value) {
                DoConsume(key, value);
            }

        private:
            virtual void DoConsume(const Stroka& key, IValue* value) {
                (void)key;
                (void)value;
            }
        };

        virtual ~IConfig() {
        }

        inline void ForEach(IFunc* func) {
            DoForEach(func);
        }

    private:
        virtual void DoForEach(IFunc* func) = 0;
    };

    template <class T>
    static inline bool ParseFromString(const Stroka& s, T& t) {
        t = FromString<T>(s);

        return true;
    }
}

#define START_PARSE virtual void DoConsume(const Stroka& key, IConfig::IValue* value) { (void)key; (void)value;
#define END_PARSE ythrow NSrvKernel::TConfigParseError() << "unsupported key(" << key.Quote() << ")"; }
#define ON_KEY(k, v) if (key == k && NSrvKernel::ParseFromString(value->AsString(), v))
