#include "rbtree.h"

#include <library/unittest/registar.h>

#include <util/random/randcpp.h>

class TRedBlackTreeTest: public TTestBase {
        struct TCmp {
            template <class T>
            static inline bool Compare(const T& l, const T& r) {
                return l.N < r.N;
            }

            template <class T>
            static inline bool Compare(const T& l, int r) {
                return l.N < r;
            }

            template <class T>
            static inline bool Compare(int l, const T& r) {
                return l < r.N;
            }
        };

        class TNode: public TRbTreeItem<TNode, TCmp> {
            public:
                inline TNode(int n) throw ()
                    : N(n)
                {
                }

                int N;
        };

        typedef TRbTree<TNode, TCmp> TTree;

        UNIT_TEST_SUITE(TRedBlackTreeTest);
            UNIT_TEST(TestEmpty)
            UNIT_TEST(TestInsert)
            UNIT_TEST(TestErase)
            UNIT_TEST(TestFind)
            UNIT_TEST(TestStress)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestStress() {
            yvector<TSharedPtr<TNode> > nodes;

            for (int i = 0; i < 1000; ++i) {
                nodes.push_back(new TNode(i));
            }

            TTree tree;
            TRand rnd;

            for (size_t i = 0; i < 1000000; ++i) {
                tree.Insert(nodes[rnd.random() % nodes.size()].Get());
            }

            for (TTree::TConstIterator it = tree.Begin(); it != tree.End();) {
                const int v1 = it->N;

                if (++it == tree.End()) {
                    break;
                }

                const int v2 = it->N;

                UNIT_ASSERT(v1 < v2);
            }
        }

        inline void TestFind() {
            TTree tree;

            {
                TNode n1(1);
                TNode n2(2);
                TNode n3(3);

                tree.Insert(n1);
                tree.Insert(n2);
                tree.Insert(n3);

                UNIT_ASSERT_EQUAL(tree.Find(1)->N, 1);
                UNIT_ASSERT_EQUAL(tree.Find(2)->N, 2);
                UNIT_ASSERT_EQUAL(tree.Find(3)->N, 3);

                UNIT_ASSERT(!tree.Find(0));
                UNIT_ASSERT(!tree.Find(4));
                UNIT_ASSERT(!tree.Find(1234567));
            }

            UNIT_ASSERT(tree.Empty());
        }

        inline void TestEmpty() {
            TTree tree;

            UNIT_ASSERT(tree.Empty());
            UNIT_ASSERT_EQUAL(tree.Begin(), tree.End());
        }

        inline void TestInsert() {
            TTree tree;

            {
                TNode n1(1);
                TNode n2(2);
                TNode n3(3);

                tree.Insert(n1);
                tree.Insert(n2);
                tree.Insert(n3);

                TTree::TConstIterator it = tree.Begin();

                UNIT_ASSERT_EQUAL((it++)->N, 1);
                UNIT_ASSERT_EQUAL((it++)->N, 2);
                UNIT_ASSERT_EQUAL((it++)->N, 3);
                UNIT_ASSERT_EQUAL(it, tree.End());
            }

            UNIT_ASSERT(tree.Empty());
        }

        inline void TestErase() {
            TTree tree;

            {
                TNode n1(1);
                TNode n2(2);
                TNode n3(3);

                tree.Insert(n1);
                tree.Insert(n2);
                tree.Insert(n3);

                TTree::TIterator it = tree.Begin();

                tree.Erase(it++);

                UNIT_ASSERT_EQUAL(it, tree.Begin());
                UNIT_ASSERT_EQUAL(it->N, 2);

                tree.Erase(it++);

                UNIT_ASSERT_EQUAL(it, tree.Begin());
                UNIT_ASSERT_EQUAL(it->N, 3);

                tree.Erase(it++);

                UNIT_ASSERT_EQUAL(it, tree.Begin());
                UNIT_ASSERT_EQUAL(it, tree.End());
            }

            UNIT_ASSERT(tree.Empty());
        }
};

UNIT_TEST_SUITE_REGISTRATION(TRedBlackTreeTest);
