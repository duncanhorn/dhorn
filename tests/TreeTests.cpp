/*
 * SocketsTest.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn sockets classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn\tree.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class node_test_class
        {
        public:
            node_test_class(void) :
                _moved(false)
            {
                instances++;
                constructed++;
            }

            node_test_class(_In_ const node_test_class &other) :
                _moved(false)
            {
                Assert::IsTrue(!other._moved);

                instances++;
                constructed++;
                copies++;
            }

            node_test_class(_In_ node_test_class &&other) :
                _moved(false)
            {
                Assert::IsTrue(!other._moved);
                other._moved = true;

                moves++;
            }

            ~node_test_class(void)
            {
                if (!this->_moved)
                {
                    instances--;
                }
            }



            node_test_class &operator=(_In_ const node_test_class &other)
            {
                (void)other;
                copies++;
            }

            node_test_class &operator=(_In_ const node_test_class &&other)
            {
                (void)other;
                moves++;
            }



            static int instances;
            static int constructed;
            static int copies;
            static int moves;

            static void reset(void)
            {
                instances = 0;
                constructed = 0;
                copies = 0;
                moves = 0;
            }

            static void check(int inst, int con, int cop, int mov)
            {
                // Should only be called when all instances should have been destroyed
                Assert::AreEqual(instances, inst);
                Assert::AreEqual(constructed, con);
                Assert::AreEqual(copies, cop);
                Assert::AreEqual(moves, mov);
            }

            template <typename _Func>
            static void test(_In_ _Func func)
            {
                reset();
                {
                    func();
                }
                Assert::AreEqual(instances, 0);
            }

        private:

            bool _moved;
        };

        int node_test_class::instances = 0;
        int node_test_class::constructed = 0;
        int node_test_class::copies = 0;
        int node_test_class::moves = 0;

        TEST_CLASS(TreeNodeTests)
        {
            using tree_type = dhorn::tree<node_test_class>;
            using test_type = _dhorn_tree_node<tree_type>;

        public:

            /*
             * Constructor Tests
             */
            TEST_METHOD(DefaultConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);

                    test_type *y;
                    node_test_class::check(1, 1, 0, 0);

                    y = new test_type;
                    node_test_class::check(2, 2, 0, 0);

                    delete y;
                    node_test_class::check(1, 2, 0, 0);
                });
            }

            TEST_METHOD(ValueConstructorTest)
            {
                node_test_class::test([]()
                {
                    node_test_class val;
                    node_test_class::check(1, 1, 0, 0);

                    test_type x(val);
                    node_test_class::check(2, 2, 1, 0);

                    test_type y(val);
                    node_test_class::check(3, 3, 2, 0);

                    test_type *z;
                    node_test_class::check(3, 3, 2, 0);

                    z = new test_type(val);
                    node_test_class::check(4, 4, 3, 0);

                    delete z;
                    node_test_class::check(3, 4, 3, 0);
                });
            }

            TEST_METHOD(ValueMoveConstructorTest)
            {
                node_test_class::test([]()
                {
                    node_test_class val1;
                    node_test_class val2;
                    node_test_class::check(2, 2, 0, 0);

                    test_type x(std::move(val1));
                    node_test_class::check(2, 2, 0, 1);

                    test_type *y;
                    node_test_class::check(2, 2, 0, 1);

                    y = new test_type(std::move(val2));
                    node_test_class::check(2, 2, 0, 2);

                    delete y;
                    node_test_class::check(1, 2, 0, 2);
                });
            }

            TEST_METHOD(CopyConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);

                    test_type y(x);
                    node_test_class::check(2, 2, 1, 0);

                    test_type *z;
                    node_test_class::check(2, 2, 1, 0);

                    z = new test_type(x);
                    node_test_class::check(3, 3, 2, 0);

                    delete z;
                    node_test_class::check(2, 3, 2, 0);
                });
            }

            TEST_METHOD(MoveConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);

                    test_type y(std::move(x));
                    node_test_class::check(1, 1, 0, 1);

                    test_type *z;
                    node_test_class::check(1, 1, 0, 1);

                    z = new test_type(std::move(y));
                    node_test_class::check(1, 1, 0, 2);

                    delete z;
                    node_test_class::check(0, 1, 0, 2);
                });
            }
        };
    }
}
