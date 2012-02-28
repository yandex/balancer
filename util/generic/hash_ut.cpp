#include "hash.h"
#include "vector.h"
#include "hash_set.h"

#include <library/unittest/registar.h>

#include <util/str_stl.h>

static const char star = 42;

class THashTest: public TTestBase {
        UNIT_TEST_SUITE(THashTest);
            UNIT_TEST(TestHMap1);
            UNIT_TEST(TestHMMap1);
            UNIT_TEST(TestHMSet1);
            UNIT_TEST(TestHSet2);
            UNIT_TEST(TestInsertErase);
        UNIT_TEST_SUITE_END();

        typedef yhash_multiset<char, hash<char>, TEqualTo<char> > hmset;

    protected:
        void TestHMap1();
        void TestHMMap1();
        void TestHMSet1();
        void TestHSet2();
        void TestInsertErase();
};

UNIT_TEST_SUITE_REGISTRATION(THashTest);

void THashTest::TestHMap1() {
    typedef yhash_map<char, Stroka, THash<char>, TEqualTo<char> > maptype;
    maptype m;
    // Store mappings between roman numerals and decimals.
    m['l'] = "50";
    m['x'] = "20"; // Deliberate mistake.
    m['v'] = "5";
    m['i'] = "1";
    UNIT_ASSERT(!strcmp(m['x'].c_str(),"20"));
    m['x'] = "10"; // Correct mistake.
    UNIT_ASSERT(!strcmp(m['x'].c_str(),"10"));

    UNIT_ASSERT(!m.has('z'));
    UNIT_ASSERT(!strcmp(m['z'].c_str(),""));
    UNIT_ASSERT(m.has('z'));

    UNIT_ASSERT(m.count('z') == 1);
    TPair<maptype::iterator, bool> p = m.insert(TPair<const char, Stroka>('c', Stroka("100")));

    UNIT_ASSERT(p.second);

    p = m.insert(TPair<const char, Stroka>('c', Stroka("100")));
    UNIT_ASSERT(!p.second);

    //Some iterators compare check, really compile time checks
    maptype::iterator ite(m.begin());
    maptype::const_iterator cite(m.begin());
    cite = m.begin();
    maptype const& cm = m;
    cite = cm.begin();

    UNIT_ASSERT((maptype::const_iterator)ite == cite);
    UNIT_ASSERT(!((maptype::const_iterator)ite != cite));
    UNIT_ASSERT(cite == (maptype::const_iterator)ite);
    UNIT_ASSERT(!(cite != (maptype::const_iterator)ite));
}

void THashTest::TestHMMap1() {
    typedef yhash_multimap<char, int, THash<char>, TEqualTo<char> > mmap;
    mmap m;

    UNIT_ASSERT(m.count('X') == 0);
    m.insert(TPair<const char, int>('X', 10)); // Standard way.
    UNIT_ASSERT(m.count('X') == 1);

    m.insert(TPair<const char, int>('X', 20));  // jbuck: standard way
    UNIT_ASSERT(m.count('X') == 2);

    m.insert(TPair<const char, int>('Y', 32));  // jbuck: standard way
    mmap::iterator i = m.find('X'); // Find first match.

    UNIT_ASSERT((*i).first == 'X');
    UNIT_ASSERT((*i).second == 10);
    i++;
    UNIT_ASSERT((*i).first == 'X');
    UNIT_ASSERT((*i).second == 20);

    i = m.find('Y');
    UNIT_ASSERT((*i).first == 'Y');
    UNIT_ASSERT((*i).second == 32);

    i = m.find('Z');
    UNIT_ASSERT(i == m.end());

    size_t count = m.erase('X');
    UNIT_ASSERT(count == 2);

    //Some iterators compare check, really compile time checks
    mmap::iterator ite(m.begin());
    mmap::const_iterator cite(m.begin());

    UNIT_ASSERT((mmap::const_iterator)ite == cite);
    UNIT_ASSERT(!((mmap::const_iterator)ite != cite));
    UNIT_ASSERT(cite == (mmap::const_iterator)ite);
    UNIT_ASSERT(!(cite != (mmap::const_iterator)ite));

    typedef yhash_multimap<size_t, size_t> HMapType;
    HMapType hmap;

    //We fill the map to implicitely start a rehash.
    for (size_t counter = 0; counter < 3077; ++counter) {
        hmap.insert(HMapType::value_type(1, counter));
    }

    hmap.insert(HMapType::value_type(12325, 1));
    hmap.insert(HMapType::value_type(12325, 2));

    UNIT_ASSERT(hmap.count(12325) == 2);

    //At this point 23 goes to the same bucket as 12325, it used to reveal a bug.
    hmap.insert(HMapType::value_type(23, 0));

    UNIT_ASSERT(hmap.count(12325) == 2);
}

void THashTest::TestHMSet1() {
    hmset s;
    UNIT_ASSERT(s.count(star) == 0);
    s.insert(star);
    UNIT_ASSERT(s.count(star) == 1);
    s.insert(star);
    UNIT_ASSERT(s.count(star) == 2);
    hmset::iterator i = s.find(char(40));
    UNIT_ASSERT(i == s.end());

    i = s.find(star);
    UNIT_ASSERT(i != s.end());
    UNIT_ASSERT(*i == '*');
    UNIT_ASSERT(s.erase(star) == 2);
}

void THashTest::TestHSet2() {
    yhash_set<int, THash<int>, TEqualTo<int> > s;
    TPair<yhash_set<int, THash<int>, TEqualTo<int> >::iterator, bool> p = s.insert(42);
    UNIT_ASSERT(p.second);
    UNIT_ASSERT(*(p.first) == 42);

    p = s.insert(42);
    UNIT_ASSERT(!p.second);
}

void THashTest::TestInsertErase() {
    typedef yhash_map<Stroka, size_t, THash<Stroka>, TEqualTo<Stroka> > hmap;
    typedef hmap::value_type val_type;

    {
        hmap values;

        UNIT_ASSERT(values.insert(val_type("foo", 0)).second);
        UNIT_ASSERT(values.insert(val_type("bar", 0)).second);
        UNIT_ASSERT(values.insert(val_type("abc", 0)).second);

        UNIT_ASSERT(values.erase("foo") == 1);
        UNIT_ASSERT(values.erase("bar") == 1);
        UNIT_ASSERT(values.erase("abc") == 1);
    }

    {
        hmap values;

        UNIT_ASSERT(values.insert(val_type("foo", 0)).second);
        UNIT_ASSERT(values.insert(val_type("bar", 0)).second);
        UNIT_ASSERT(values.insert(val_type("abc", 0)).second);

        UNIT_ASSERT(values.erase("abc") == 1);
        UNIT_ASSERT(values.erase("bar") == 1);
        UNIT_ASSERT(values.erase("foo") == 1);
    }
}
