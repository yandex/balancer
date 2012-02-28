#include "map.h"

#include <algorithm>

#include <library/unittest/registar.h>

class TYMapTest: public TTestBase {
        UNIT_TEST_SUITE(TYMapTest);
            UNIT_TEST(TestMap1);
            UNIT_TEST(TestMMap1);
            UNIT_TEST(TestMMap2);
            UNIT_TEST(TestIterators);
            UNIT_TEST(TestEqualRange);
            UNIT_TEST(TestTemplateMethods);
        UNIT_TEST_SUITE_END();

    protected:
        void TestMap1();
        void TestMMap1();
        void TestMMap2();
        void TestIterators();
        void TestEqualRange();
        void TestTemplateMethods();
};

UNIT_TEST_SUITE_REGISTRATION(TYMapTest);

void TYMapTest::TestMap1() {
    typedef ymap<char, int, TLess<char> > maptype;
    maptype m;
    // Store mappings between roman numerals and decimals.
    m['l'] = 50;
    m['x'] = 20; // Deliberate mistake.
    m['v'] = 5;
    m['i'] = 1;

    UNIT_ASSERT(m['x'] == 20);
    m['x'] = 10; // Correct mistake.
    UNIT_ASSERT(m['x'] == 10);
    UNIT_ASSERT(m['z'] == 0);

    UNIT_ASSERT(m.count('z') == 1);

    TPair<maptype::iterator, bool> p = m.insert(TPair<const char, int>('c', 100));

    UNIT_ASSERT(p.second);
    UNIT_ASSERT(p.first != m.end());
    UNIT_ASSERT((*p.first).first == 'c');
    UNIT_ASSERT((*p.first).second == 100);

    p = m.insert(TPair<const char, int>('c', 100));

    UNIT_ASSERT(!p.second); // already existing pair
    UNIT_ASSERT(p.first != m.end());
    UNIT_ASSERT((*p.first).first == 'c');
    UNIT_ASSERT((*p.first).second == 100);
}

void TYMapTest::TestMMap1() {
    typedef ymultimap<char, int, TLess<char> > mmap;
    mmap m;
    UNIT_ASSERT(m.count('X') == 0);

    m.insert(TPair<const char, int>('X', 10)); // Standard way.
    UNIT_ASSERT(m.count('X') == 1);

    m.insert(TPair<const char, int>('X', 20)); // jbuck: standard way
    UNIT_ASSERT(m.count('X') == 2);

    m.insert(TPair<const char, int>('Y', 32)); // jbuck: standard way
    mmap::iterator i = m.find('X'); // Find first match.
    TPair<const char, int> p('X', 10);
    //UNIT_ASSERT(*i == p);
    //UNIT_ASSERT((*i).first == 'X');
    //UNIT_ASSERT((*i).second == 10);
    i++;
    UNIT_ASSERT((*i).first == 'X');
    UNIT_ASSERT((*i).second == 20);
    i++;
    UNIT_ASSERT((*i).first == 'Y');
    UNIT_ASSERT((*i).second == 32);
    i++;
    UNIT_ASSERT(i == m.end());

    size_t count = m.erase('X');
    UNIT_ASSERT(count == 2);
}

void TYMapTest::TestMMap2() {
    typedef TPair<const int, char> pair_type;

    pair_type p1(3, 'c');
    pair_type p2(6, 'f');
    pair_type p3(1, 'a');
    pair_type p4(2, 'b');
    pair_type p5(3, 'x');
    pair_type p6(6, 'f');

    typedef ymultimap<int, char, TLess<int> > mmap;

    pair_type array [] = {
        p1,
        p2,
        p3,
        p4,
        p5,
        p6
    };

    mmap m(array + 0, array + 6);
    mmap::iterator i;
    i = m.lower_bound(3);
    UNIT_ASSERT((*i).first == 3);
    UNIT_ASSERT((*i).second == 'c');

    i = m.upper_bound(3);
    UNIT_ASSERT((*i).first == 6);
    UNIT_ASSERT((*i).second == 'f');
}

void TYMapTest::TestIterators() {
    typedef ymap<int, char, TLess<int> > int_map;
    int_map imap;

    {
        int_map::iterator ite(imap.begin());
        int_map::const_iterator cite(imap.begin());

        UNIT_ASSERT(ite == cite);
        UNIT_ASSERT(!(ite != cite));
        UNIT_ASSERT(cite == ite);
        UNIT_ASSERT(!(cite != ite));
    }

    typedef ymultimap<int, char, TLess<int> > mmap;
    typedef mmap::value_type pair_type;

    pair_type p1(3, 'c');
    pair_type p2(6, 'f');
    pair_type p3(1, 'a');
    pair_type p4(2, 'b');
    pair_type p5(3, 'x');
    pair_type p6(6, 'f');

    pair_type array [] = {
        p1,
        p2,
        p3,
        p4,
        p5,
        p6
    };

    mmap m(array + 0, array + 6);

    {
        mmap::iterator ite(m.begin());
        mmap::const_iterator cite(m.begin());
        //test compare between const_iterator and iterator
        UNIT_ASSERT(ite == cite);
        UNIT_ASSERT(!(ite != cite));
        UNIT_ASSERT(cite == ite);
        UNIT_ASSERT(!(cite != ite));
    }

    mmap::reverse_iterator ri = m.rbegin();

    UNIT_ASSERT(ri != m.rend());
    UNIT_ASSERT(ri == m.rbegin());
    UNIT_ASSERT((*ri).first == 6);
    UNIT_ASSERT((*ri++).second == 'f');
    UNIT_ASSERT((*ri).first == 6);
    UNIT_ASSERT((*ri).second == 'f');

    mmap const& cm = m;
    mmap::const_reverse_iterator rci = cm.rbegin();

    UNIT_ASSERT(rci != cm.rend());
    UNIT_ASSERT((*rci).first == 6);
    UNIT_ASSERT((*rci++).second == 'f');
    UNIT_ASSERT((*rci).first == 6);
    UNIT_ASSERT((*rci).second == 'f');
}

void TYMapTest::TestEqualRange() {
    typedef ymap<char, int, TLess<char> > maptype;

    {
        maptype m;
        m['x'] = 10;

        TPair<maptype::iterator, maptype::iterator> ret;
        ret = m.equal_range('x');
        UNIT_ASSERT(ret.first != ret.second);
        UNIT_ASSERT((*(ret.first)).first == 'x');
        UNIT_ASSERT((*(ret.first)).second == 10);
        UNIT_ASSERT(++(ret.first) == ret.second);
    }

    {
        {
            maptype m;

            maptype::iterator i = m.lower_bound('x');
            UNIT_ASSERT(i == m.end());

            i = m.upper_bound('x');
            UNIT_ASSERT(i == m.end());

            TPair<maptype::iterator, maptype::iterator> ret;
            ret = m.equal_range('x');
            UNIT_ASSERT(ret.first == ret.second);
            UNIT_ASSERT(ret.first == m.end());
        }

        {
            const maptype m;
            TPair<maptype::const_iterator, maptype::const_iterator> ret;
            ret = m.equal_range('x');
            UNIT_ASSERT(ret.first == ret.second);
            UNIT_ASSERT(ret.first == m.end());
        }
    }
}

struct TKey {
    TKey()
        : m_data(0)
    {
    }

    explicit TKey(int data)
        : m_data(data)
    {
    }

    int m_data;
};

struct TKeyCmp {
    bool operator () (TKey lhs, TKey rhs) const {
        return lhs.m_data < rhs.m_data;
    }

    bool operator () (TKey lhs, int rhs) const {
        return lhs.m_data < rhs;
    }

    bool operator () (int lhs, TKey rhs) const {
        return lhs < rhs.m_data;
    }
};

struct TKeyCmpPtr {
    bool operator () (TKey const volatile* lhs, TKey const volatile* rhs) const {
        return (*lhs).m_data < (*rhs).m_data;
    }

    bool operator () (TKey const volatile* lhs, int rhs) const {
        return (*lhs).m_data < rhs;
    }

    bool operator () (int lhs, TKey const volatile* rhs) const {
        return lhs < (*rhs).m_data;
    }
};

void TYMapTest::TestTemplateMethods() {
    {
        typedef ymap<TKey, int, TKeyCmp> Container;
        typedef Container::value_type value;

        Container cont;

        cont.insert(value(TKey(1), 1));
        cont.insert(value(TKey(2), 2));
        cont.insert(value(TKey(3), 3));
        cont.insert(value(TKey(4), 4));

        UNIT_ASSERT(cont.count(TKey(1)) == 1);
        UNIT_ASSERT(cont.count(1) == 1);
        UNIT_ASSERT(cont.count(5) == 0);

        UNIT_ASSERT(cont.find(2) != cont.end());
        UNIT_ASSERT(cont.lower_bound(2) != cont.end());
        UNIT_ASSERT(cont.upper_bound(2) != cont.end());
        UNIT_ASSERT(cont.equal_range(2) != NStl::make_pair(cont.begin(), cont.end()));

        Container const& ccont = cont;

        UNIT_ASSERT(ccont.find(2) != ccont.end());
        UNIT_ASSERT(ccont.lower_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.upper_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.equal_range(2) != NStl::make_pair(ccont.end(), ccont.end()));
    }

    {
        typedef ymap<TKey*, int, TKeyCmpPtr> Container;
        typedef Container::value_type value;

        Container cont;

        TKey key1(1), key2(2), key3(3), key4(4);

        cont.insert(value(&key1, 1));
        cont.insert(value(&key2, 2));
        cont.insert(value(&key3, 3));
        cont.insert(value(&key4, 4));

        UNIT_ASSERT(cont.count(1) == 1);
        UNIT_ASSERT(cont.count(5) == 0);

        UNIT_ASSERT(cont.find(2) != cont.end());
        UNIT_ASSERT(cont.lower_bound(2) != cont.end());
        UNIT_ASSERT(cont.upper_bound(2) != cont.end());
        UNIT_ASSERT(cont.equal_range(2) != NStl::make_pair(cont.begin(), cont.end()));

        Container const& ccont = cont;

        UNIT_ASSERT(ccont.find(2) != ccont.end());
        UNIT_ASSERT(ccont.lower_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.upper_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.equal_range(2) != NStl::make_pair(ccont.begin(), ccont.end()));
    }

    {
        typedef ymultimap<TKey, int, TKeyCmp> Container;
        typedef Container::value_type value;

        Container cont;

        cont.insert(value(TKey(1), 1));
        cont.insert(value(TKey(2), 2));
        cont.insert(value(TKey(3), 3));
        cont.insert(value(TKey(4), 4));

        UNIT_ASSERT(cont.count(TKey(1)) == 1);
        UNIT_ASSERT(cont.count(1) == 1);
        UNIT_ASSERT(cont.count(5) == 0);

        UNIT_ASSERT(cont.find(2) != cont.end());
        UNIT_ASSERT(cont.lower_bound(2) != cont.end());
        UNIT_ASSERT(cont.upper_bound(2) != cont.end());
        UNIT_ASSERT(cont.equal_range(2) != NStl::make_pair(cont.begin(), cont.end()));

        Container const& ccont = cont;

        UNIT_ASSERT(ccont.find(2) != ccont.end());
        UNIT_ASSERT(ccont.lower_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.upper_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.equal_range(2) != make_pair(ccont.end(), ccont.end()));
    }

    {
        typedef ymultimap<TKey const volatile*, int, TKeyCmpPtr> Container;
        typedef Container::value_type value;

        Container cont;

        TKey key1(1), key2(2), key3(3), key4(4);

        cont.insert(value(&key1, 1));
        cont.insert(value(&key2, 2));
        cont.insert(value(&key3, 3));
        cont.insert(value(&key4, 4));

        UNIT_ASSERT(cont.count(1) == 1);
        UNIT_ASSERT(cont.count(5) == 0);

        UNIT_ASSERT(cont.find(2) != cont.end());
        UNIT_ASSERT(cont.lower_bound(2) != cont.end());
        UNIT_ASSERT(cont.upper_bound(2) != cont.end());
        UNIT_ASSERT(cont.equal_range(2) != NStl::make_pair(cont.begin(), cont.end()));

        Container const& ccont = cont;

        UNIT_ASSERT(ccont.find(2) != ccont.end());
        UNIT_ASSERT(ccont.lower_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.upper_bound(2) != ccont.end());
        UNIT_ASSERT(ccont.equal_range(2) != NStl::make_pair(ccont.begin(), ccont.end()));
    }
}
