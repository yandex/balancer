#pragma once

namespace NStl {
    template <class C, class T, class A>
    class basic_string;

    template <class C, class T>
    class basic_istream;

    template <class C, class T>
    class basic_ostream;

    template <class C>
    class char_traits;

    template <class C, class T, class A>
    class basic_string;

    template <class F, class S>
    struct pair;
}

typedef NStl::basic_istream<char, NStl::char_traits<char> > TIStream;
typedef NStl::basic_ostream<char, NStl::char_traits<char> > TOStream;
