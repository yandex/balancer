#pragma once

#include "base.h"

#include <util/generic/stroka.h>

/// @addtogroup Streams_Strings
/// @{

/// Поток ввода для чтения данных из строки.
class TStringInput: public TInputStream {
    public:
        inline TStringInput(const Stroka& s) throw ()
            : S_(s)
            , Pos_(0)
        {
        }

        virtual ~TStringInput() throw ();

    private:
        virtual size_t DoRead(void* buf, size_t len);

    private:
        const Stroka& S_;
        size_t Pos_;
};

/// Поток вывода для записи данных в строку.
class TStringOutput: public TOutputStream {
    public:
        inline TStringOutput(Stroka& s) throw ()
            : S_(s)
        {
        }

        virtual ~TStringOutput() throw ();

    private:
        virtual void DoWrite(const void* buf, size_t len);

    private:
        Stroka& S_;
};

/// Поток ввода/вывода для чтения/записи данных в строку.
class TStringStream: public Stroka
                   , public TStringInput
                   , public TStringOutput {
    public:
        inline TStringStream()
            : TStringInput(Str())
            , TStringOutput(Str())
        {
        }

        virtual ~TStringStream() throw ();

        /*
         * compat(with STL) methods
         */
        inline Stroka& Str() throw () {
            return *this;
        }

        inline const Stroka& Str() const throw () {
            return *this;
        }
};

/// @}
