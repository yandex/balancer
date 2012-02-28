#include "set.h"

#include <algorithm>

#include <library/unittest/registar.h>

#include <util/generic/pair.h>

class TYSetTest: public TTestBase {
        UNIT_TEST_SUITE(TYSetTest);
            UNIT_TEST(TestSet1);
            UNIT_TEST(TestSet2);
            UNIT_TEST(TestErase);
            UNIT_TEST(TestInsert);
            UNIT_TEST(TestFind);
            UNIT_TEST(TestBounds);
            UNIT_TEST(TestImplementationCheck);
            UNIT_TEST(TestReverseIteratorTest);
            UNIT_TEST(TestTemplateMethods);
        UNIT_TEST_SUITE_END();

    protected:
        void TestSet1();
        void TestSet2();
        void TestErase();
        void TestInsert();
        void TestFind();
        void TestBounds();
        void TestImplementationCheck();
        void TestReverseIteratorTest();
        void TestTemplateMethods();
};

UNIT_TEST_SUITE_REGISTRATION(TYSetTest);

void TYSetTest::TestSet1() {
    yset<int, TLess<int> > s;
    UNIT_ASSERT(s.count(42) == 0);
    s.insert(42);
    UNIT_ASSERT(s.count(42) == 1);
    s.insert(42);
    UNIT_ASSERT(s.count(42) == 1);
    size_t count = s.erase(42);
    UNIT_ASSERT(count == 1);
}

void TYSetTest::TestSet2() {
    typedef yset<int, TLess<int> > int_set;
    int_set s;
    TPair<int_set::iterator, bool> p = s.insert(42);
    UNIT_ASSERT(p.second == true);
    p = s.insert(42);
    UNIT_ASSERT(p.second == false);

    int array1 [] = { 1, 3, 6, 7 };
    s.insert(array1, array1 + 4);
    UNIT_ASSERT(distance(s.begin(), s.end()) == 5);

    int_set s2;
    s2.swap(s);
    UNIT_ASSERT(distance(s2.begin(), s2.end()) == 5);
    UNIT_ASSERT(distance(s.begin(), s.end()) == 0);

    int_set s3;
    s3.swap(s);
    s3.swap(s2);
    UNIT_ASSERT(distance(s.begin(), s.end()) == 0);
    UNIT_ASSERT(distance(s2.begin(), s2.end()) == 0);
    UNIT_ASSERT(distance(s3.begin(), s3.end()) == 5);
}

void TYSetTest::TestErase() {
    yset<int, TLess<int> > s;
    s.insert(1);
    s.erase(s.begin());
    UNIT_ASSERT(s.empty());

    size_t nb = s.erase(1);
    UNIT_ASSERT(nb == 0);
}

void TYSetTest::TestInsert() {
    yset<int> s;
    yset<int>::iterator i = s.insert(s.end(), 0);
    UNIT_ASSERT(*i == 0);
}

void TYSetTest::TestFind() {
    yset<int> s;

    UNIT_ASSERT(s.find(0) == s.end());

    yset<int> const& crs = s;

    UNIT_ASSERT(crs.find(0) == crs.end());
}

void TYSetTest::TestBounds() {
    int array1 [] = { 1, 3, 6, 7 };
    yset<int> s(array1, array1 + sizeof(array1) / sizeof(array1[0]));
    yset<int> const& crs = s;

    yset<int>::iterator sit;
    yset<int>::const_iterator scit;
    TPair<yset<int>::iterator, yset<int>::iterator> pit;
    TPair<yset<int>::const_iterator, yset<int>::const_iterator> pcit;

    //Check iterator on mutable set
    sit = s.lower_bound(2);
    UNIT_ASSERT(sit != s.end());
    UNIT_ASSERT(*sit == 3);

    sit = s.upper_bound(5);
    UNIT_ASSERT(sit != s.end());
    UNIT_ASSERT(*sit == 6);

    pit = s.equal_range(6);
    UNIT_ASSERT(pit.first != pit.second);
    UNIT_ASSERT(pit.first != s.end());
    UNIT_ASSERT(*pit.first == 6);
    UNIT_ASSERT(pit.second != s.end());
    UNIT_ASSERT(*pit.second == 7);

    pit = s.equal_range(4);
    UNIT_ASSERT(pit.first == pit.second);
    UNIT_ASSERT(pit.first != s.end());
    UNIT_ASSERT(*pit.first == 6);
    UNIT_ASSERT(pit.second != s.end());
    UNIT_ASSERT(*pit.second == 6);

    //Check const_iterator on mutable set
    scit = s.lower_bound(2);
    UNIT_ASSERT(scit != s.end());
    UNIT_ASSERT(*scit == 3);

    scit = s.upper_bound(5);
    UNIT_ASSERT(scit != s.end());
    UNIT_ASSERT(*scit == 6);

    pcit = s.equal_range(6);
    UNIT_ASSERT(pcit.first != pcit.second);
    UNIT_ASSERT(pcit.first != s.end());
    UNIT_ASSERT(*pcit.first == 6);
    UNIT_ASSERT(pcit.second != s.end());
    UNIT_ASSERT(*pcit.second == 7);

    //Check const_iterator on const set
    scit = crs.lower_bound(2);
    UNIT_ASSERT(scit != crs.end());
    UNIT_ASSERT(*scit == 3);

    scit = crs.upper_bound(5);
    UNIT_ASSERT(scit != crs.end());
    UNIT_ASSERT(*scit == 6);

    pcit = crs.equal_range(6);
    UNIT_ASSERT(pcit.first != pcit.second);
    UNIT_ASSERT(pcit.first != crs.end());
    UNIT_ASSERT(*pcit.first == 6);
    UNIT_ASSERT(pcit.second != crs.end());
    UNIT_ASSERT(*pcit.second == 7);
}

void TYSetTest::TestImplementationCheck() {
    yset<int> tree;
    tree.insert(1);
    yset<int>::iterator it = tree.begin();
    int const& int_ref = *it++;
    UNIT_ASSERT(int_ref == 1);

    UNIT_ASSERT(it == tree.end());
    UNIT_ASSERT(it != tree.begin());

    yset<int>::const_iterator cit = tree.begin();
    int const& int_cref = *cit++;
    UNIT_ASSERT(int_cref == 1);
}

void TYSetTest::TestReverseIteratorTest() {
    yset<int> tree;
    tree.insert(1);
    tree.insert(2);

    {
        yset<int>::reverse_iterator rit(tree.rbegin());
        UNIT_ASSERT(*(rit++) == 2);
        UNIT_ASSERT(*(rit++) == 1);
        UNIT_ASSERT(rit == tree.rend());
    }

    {
        yset<int> const& ctree = tree;
        yset<int>::const_reverse_iterator rit(ctree.rbegin());
        UNIT_ASSERT(*(rit++) == 2);
        UNIT_ASSERT(*(rit++) == 1);
        UNIT_ASSERT(rit == ctree.rend());
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
    bool operator () (TKey const volatile *lhs, TKey const volatile *rhs) const {
        return (*lhs).m_data < (*rhs).m_data;
    }

    bool operator () (TKey const volatile *lhs, int rhs) const {
        return (*lhs).m_data < rhs;
    }

    bool operator () (int lhs, TKey const volatile *rhs) const {
        return lhs < (*rhs).m_data;
    }
};

void TYSetTest::TestTemplateMethods() {
    {
        typedef yset<TKey, TKeyCmp> KeySet;
        KeySet keySet;
        keySet.insert(TKey(1));
        keySet.insert(TKey(2));
        keySet.insert(TKey(3));
        keySet.insert(TKey(4));

        UNIT_ASSERT(keySet.count(TKey(1)) == 1);
        UNIT_ASSERT(keySet.count(1) == 1);
        UNIT_ASSERT(keySet.count(5) == 0);

        UNIT_ASSERT(keySet.find(2) != keySet.end());
        UNIT_ASSERT(keySet.lower_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.upper_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.equal_range(2) != NStl::make_pair(keySet.begin(), keySet.end()));

        KeySet const& ckeySet = keySet;
        UNIT_ASSERT(ckeySet.find(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.lower_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.upper_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.equal_range(2) != NStl::make_pair(ckeySet.begin(), ckeySet.end()));
    }

    {
        typedef yset<TKey*, TKeyCmpPtr> KeySet;
        KeySet keySet;
        TKey key1(1), key2(2), key3(3), key4(4);
        keySet.insert(&key1);
        keySet.insert(&key2);
        keySet.insert(&key3);
        keySet.insert(&key4);

        UNIT_ASSERT(keySet.count(1) == 1);
        UNIT_ASSERT(keySet.count(5) == 0);

        UNIT_ASSERT(keySet.find(2) != keySet.end());
        UNIT_ASSERT(keySet.lower_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.upper_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.equal_range(2) != NStl::make_pair(keySet.begin(), keySet.end()));

        KeySet const& ckeySet = keySet;
        UNIT_ASSERT(ckeySet.find(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.lower_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.upper_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.equal_range(2) != NStl::make_pair(ckeySet.begin(), ckeySet.end()));
    }
    {
        typedef ymultiset<TKey, TKeyCmp> KeySet;
        KeySet keySet;
        keySet.insert(TKey(1));
        keySet.insert(TKey(2));
        keySet.insert(TKey(3));
        keySet.insert(TKey(4));

        UNIT_ASSERT(keySet.count(TKey(1)) == 1);
        UNIT_ASSERT(keySet.count(1) == 1);
        UNIT_ASSERT(keySet.count(5) == 0);

        UNIT_ASSERT(keySet.find(2) != keySet.end());
        UNIT_ASSERT(keySet.lower_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.upper_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.equal_range(2) != NStl::make_pair(keySet.begin(), keySet.end()));

        KeySet const& ckeySet = keySet;
        UNIT_ASSERT(ckeySet.find(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.lower_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.upper_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.equal_range(2) != NStl::make_pair(ckeySet.begin(), ckeySet.end()));
    }

    {
        typedef ymultiset<TKey const volatile*, TKeyCmpPtr> KeySet;
        KeySet keySet;
        TKey key1(1), key2(2), key3(3), key4(4);
        keySet.insert(&key1);
        keySet.insert(&key2);
        keySet.insert(&key3);
        keySet.insert(&key4);

        UNIT_ASSERT(keySet.count(1) == 1);
        UNIT_ASSERT(keySet.count(5) == 0);

        UNIT_ASSERT(keySet.find(2) != keySet.end());
        UNIT_ASSERT(keySet.lower_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.upper_bound(2) != keySet.end());
        UNIT_ASSERT(keySet.equal_range(2) != NStl::make_pair(keySet.begin(), keySet.end()));

        KeySet const& ckeySet = keySet;
        UNIT_ASSERT(ckeySet.find(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.lower_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.upper_bound(2) != ckeySet.end());
        UNIT_ASSERT(ckeySet.equal_range(2) != NStl::make_pair(ckeySet.begin(), ckeySet.end()));
    }
}
