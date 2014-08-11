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
#include "dhorn\type_traits.h"

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

                return *this;
            }

            node_test_class &operator=(_In_ const node_test_class &&other)
            {
                (void)other;
                moves++;

                return *this;
            }

            int doit(void) {}
            double doit(void) const {}




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
        public:
            using tree_type = dhorn::tree<node_test_class>;
            using test_type = _dhorn_tree_node<dhorn::_dhorn_tree_types<node_test_class, std::allocator<node_test_class>>>;
            using sentinel_type = _dhorn_tree_node<dhorn::_dhorn_tree_types<node_test_class, std::allocator<node_test_class>>, true>;

            /*
             * Constructor Tests
             */
            TEST_METHOD(DefaultConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);
                    Assert::IsTrue(x.size() == 0);

                    test_type *y;
                    node_test_class::check(1, 1, 0, 0);

                    y = new test_type;
                    node_test_class::check(2, 2, 0, 0);
                    Assert::IsTrue(y->size() == 0);

                    delete y;
                    node_test_class::check(1, 2, 0, 0);
                });

                // Sentinel nodes should not affect counts at all
                node_test_class::reset();
                sentinel_type x;
                node_test_class::check(0, 0, 0, 0);
            }

            TEST_METHOD(ValueConstructorTest)
            {
                node_test_class::test([]()
                {
                    node_test_class val;
                    node_test_class::check(1, 1, 0, 0);

                    test_type x(val);
                    node_test_class::check(2, 2, 1, 0);
                    Assert::IsTrue(x.size() == 0);

                    test_type y(val);
                    node_test_class::check(3, 3, 2, 0);
                    Assert::IsTrue(y.size() == 0);

                    test_type *z;
                    node_test_class::check(3, 3, 2, 0);

                    z = new test_type(val);
                    node_test_class::check(4, 4, 3, 0);
                    Assert::IsTrue(z->size() == 0);

                    delete z;
                    node_test_class::check(3, 4, 3, 0);
                });

                // Sentinel node should not be value-constructable
                Assert::IsFalse(std::is_constructible<sentinel_type, node_test_class>::value);
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
                    Assert::IsTrue(x.size() == 0);

                    test_type *y;
                    node_test_class::check(2, 2, 0, 1);

                    y = new test_type(std::move(val2));
                    node_test_class::check(2, 2, 0, 2);
                    Assert::IsTrue(y->size() == 0);

                    delete y;
                    node_test_class::check(1, 2, 0, 2);
                });

                // Sentinel node should not be rvalue-constructable
                Assert::IsFalse(std::is_constructible<sentinel_type, node_test_class &&>::value);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);
                    Assert::IsTrue(x.size() == 0);

                    test_type y(x);
                    node_test_class::check(2, 2, 1, 0);
                    Assert::IsTrue(y.size() == 0);

                    test_type *z;
                    node_test_class::check(2, 2, 1, 0);

                    z = new test_type(x);
                    node_test_class::check(3, 3, 2, 0);
                    Assert::IsTrue(z->size() == 0);

                    delete z;
                    node_test_class::check(2, 3, 2, 0);
                });

                // Copying sentinel nodes should have no effect on the value(s)
                node_test_class::reset();
                sentinel_type x;
                sentinel_type y(x);
                node_test_class::check(0, 0, 0, 0);

                // For the most part, you should not be able to convert between sentinel and
                // non-sentinel nodes. That being said, it is not clear whether or not converting
                // from a non-sentinel node to a sentinel node would make sense (e.g. if children
                // become orphans), so just leave that situation supported for now.
                Assert::IsFalse(std::is_constructible<test_type, sentinel_type>::value);
                Assert::IsTrue(std::is_constructible<sentinel_type, test_type>::value);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(1, 1, 0, 0);
                    Assert::IsTrue(x.size() == 0);

                    test_type y(std::move(x));
                    node_test_class::check(1, 1, 0, 1);
                    Assert::IsTrue(y.size() == 0);

                    test_type *z;
                    node_test_class::check(1, 1, 0, 1);

                    z = new test_type(std::move(y));
                    node_test_class::check(1, 1, 0, 2);
                    Assert::IsTrue(z->size() == 0);

                    delete z;
                    node_test_class::check(0, 1, 0, 2);
                });

                // Move-constructing sentinel nodes should have no effect on value types
                node_test_class::reset();
                sentinel_type x;
                sentinel_type y(std::move(x));
                node_test_class::check(0, 0, 0, 0);

                // As with before, ensure proper behavior for converting
                Assert::IsFalse(std::is_constructible<test_type, sentinel_type &&>::value);
                Assert::IsTrue(std::is_constructible<sentinel_type, test_type &&>::value);
            }



            /*
             * Assignment Tests
             */
            TEST_METHOD(AssignmentTest)
            {
                node_test_class::test([](void)
                {
                    test_type x, y;
                    node_test_class::check(2, 2, 0, 0);
                    Assert::IsTrue(x.size() == 0);
                    Assert::IsTrue(y.size() == 0);

                    y = x;
                    node_test_class::check(2, 2, 1, 0);
                    Assert::IsTrue(y.size() == 0);

                    test_type *z = new test_type;
                    node_test_class::check(3, 3, 1, 0);

                    *z = x;
                    node_test_class::check(3, 3, 2, 0);
                    Assert::IsTrue(z->size() == 0);

                    delete z;
                    node_test_class::check(2, 3, 2, 0);
                });

                // Assignment to sentinel node should not affect the value type
                node_test_class::reset();
                sentinel_type x, y, z;
                y = x;
                z = x;
                node_test_class::check(0, 0, 0, 0);

                // Assignment follows the same rules as copy constructing
                Assert::IsFalse(std::is_assignable<test_type, sentinel_type>::value);
                Assert::IsTrue(std::is_assignable<sentinel_type, test_type>::value);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                node_test_class::test([]()
                {
                    test_type x, y;
                    node_test_class::check(2, 2, 0, 0);
                    Assert::IsTrue(x.size() == 0);
                    Assert::IsTrue(y.size() == 0);

                    y = std::move(x);
                    node_test_class::check(2, 2, 0, 1);
                    Assert::IsTrue(y.size() == 0);

                    test_type *z = new test_type;
                    node_test_class::check(3, 3, 0, 1);

                    *z = std::move(x);
                    node_test_class::check(3, 3, 0, 2);
                    Assert::IsTrue(z->size() == 0);

                    delete z;
                    node_test_class::check(2, 3, 0, 2);
                });

                // Assignment to sentinel node should not affect the value type
                node_test_class::reset();
                sentinel_type x, y, z;
                y = std::move(x);
                z = std::move(z);
                node_test_class::check(0, 0, 0, 0);

                // Assignment follows the same rules as copy constructing
                Assert::IsFalse(std::is_assignable<test_type, sentinel_type &&>::value);
                Assert::IsTrue(std::is_assignable<sentinel_type, test_type &&>::value);
            }



            /*
             * Value Tests
             */
            TEST_METHOD(ValueAccessorTest)
            {
                using node_type = _dhorn_tree_node<dhorn::tree<std::string>>;

                node_type x;
                Assert::IsTrue(x.value() == "");

                x.value() = "hello, world";
                Assert::IsTrue(x.value() == "hello, world");
                Assert::IsTrue(std::is_assignable<decltype(x.value()), std::string>::value);

                node_type y("test");
                Assert::IsTrue(y.value() == "test");

                const node_type z("const");
                Assert::IsTrue(z.value() == "const");

                Assert::IsFalse(std::is_assignable<decltype(z.value()), std::string>::value);
            }
        };



        /*
         * Mostly just tests to make sure that conversion between iterator types works as expected,
         * and accessability is correct as well
         */
        TEST_CLASS(TreeIteratorTests)
        {
            using test_type = dhorn::tree<node_test_class>;

            TEST_METHOD(EqualityTest)
            {
                // Should be able to compare both iterators and const iterators. Same thing with
                // reverse iterators. These two types should not be comparable, though
                test_type x;
                auto itr = std::begin(x);
                auto citr = std::cbegin(x);
                auto ritr = std::rbegin(x);
                auto critr = std::crbegin(x);

                Assert::IsTrue(itr == citr);
                Assert::IsTrue(citr == itr);
                Assert::IsFalse(itr != citr);
                Assert::IsFalse(citr != itr);

                Assert::IsTrue(ritr == critr);
                Assert::IsTrue(critr == ritr);
                Assert::IsFalse(ritr != critr);
                Assert::IsFalse(critr != ritr);

                Assert::IsTrue(dhorn::is_comparable<decltype(itr), decltype(citr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(itr), decltype(ritr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(itr), decltype(critr)>::value);

                Assert::IsTrue(dhorn::is_comparable<decltype(citr), decltype(itr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(citr), decltype(ritr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(citr), decltype(critr)>::value);

                Assert::IsFalse(dhorn::is_comparable<decltype(ritr), decltype(itr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(ritr), decltype(citr)>::value);
                Assert::IsTrue(dhorn::is_comparable<decltype(ritr), decltype(critr)>::value);

                Assert::IsFalse(dhorn::is_comparable<decltype(critr), decltype(itr)>::value);
                Assert::IsFalse(dhorn::is_comparable<decltype(critr), decltype(citr)>::value);
                Assert::IsTrue(dhorn::is_comparable<decltype(critr), decltype(ritr)>::value);

                // Insert a value and assert that begin() != end
                x.insert(std::begin(x), node_test_class());

                Assert::IsTrue(std::begin(x) != std::cend(x));
                Assert::IsTrue(std::end(x) != std::cbegin(x));
                Assert::IsFalse(std::begin(x) == std::cend(x));
                Assert::IsFalse(std::end(x) == std::cbegin(x));

                Assert::IsTrue(std::rbegin(x) != std::crend(x));
                Assert::IsTrue(std::rend(x) != std::crbegin(x));
                Assert::IsFalse(std::rbegin(x) == std::crend(x));
                Assert::IsFalse(std::rend(x) == std::crbegin(x));
            }

            template <bool _IsEqual, typename _Less, typename _Greater>
            void _TestComparisons(_In_ _Less less, _In_ _Greater greater)
            {
                Assert::IsTrue(less <= greater);
                Assert::IsFalse(less > greater);
                Assert::IsTrue((less < greater) == !_IsEqual);
                Assert::IsTrue((less >= greater) == _IsEqual);

                Assert::IsTrue(greater >= less);
                Assert::IsFalse(greater < less);
                Assert::IsTrue((greater > less) == !_IsEqual);
                Assert::IsTrue((greater <= less) == _IsEqual);
            }

            TEST_METHOD(InequalityTest)
            {
                test_type x;
                x.insert(std::begin(x), node_test_class());

                auto front = std::begin(x);
                auto back = std::end(x);
                auto cfront = std::cbegin(x);
                auto cback = std::cend(x);

                _TestComparisons<false>(front, back);
                _TestComparisons<true>(front, front);
                _TestComparisons<true>(back, back);
                _TestComparisons<false>(cfront, cback);
                _TestComparisons<true>(cfront, cfront);
                _TestComparisons<true>(cback, cback);

                _TestComparisons<false>(front, cback);
                _TestComparisons<false>(cfront, back);
                _TestComparisons<true>(front, cfront);
                _TestComparisons<true>(cfront, front);
                _TestComparisons<true>(back, cback);
                _TestComparisons<true>(cback, back);

                // Now, same thing with the reverse iterators
                auto rfront = std::rbegin(x);
                auto rback = std::rend(x);
                auto crfront = std::crbegin(x);
                auto crback = std::crend(x);

                _TestComparisons<false>(rfront, rback);
                _TestComparisons<true>(rfront, rfront);
                _TestComparisons<true>(rback, rback);
                _TestComparisons<false>(crfront, crback);
                _TestComparisons<true>(crfront, crfront);
                _TestComparisons<true>(crback, crback);

                _TestComparisons<false>(rfront, crback);
                _TestComparisons<false>(crfront, rback);
                _TestComparisons<true>(rfront, crfront);
                _TestComparisons<true>(crfront, rfront);
                _TestComparisons<true>(rback, crback);
                _TestComparisons<true>(crback, rback);
            }

            TEST_METHOD(DereferenceTest)
            {
                dhorn::tree<int> x;
                x.insert(std::end(x), 8);
                x.insert(std::end(x), 42);

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                Assert::IsTrue(*front == 8);
                Assert::IsTrue(*cfront == 8);
                Assert::IsTrue(*rfront == 42);
                Assert::IsTrue(*crfront == 42);

                // Make sure you can assign to non-const iterators and you can't to const iterators
                Assert::IsTrue(std::is_assignable<decltype(*front), int>::value);
                Assert::IsTrue(std::is_assignable<decltype(*rfront), int>::value);

                // There is a bug with Visual Studio's implementation of std::is_assignable that
                // causes the following to fail even though it should pass.
                //Assert::IsFalse(std::is_assignable<decltype(*cfront), int>::value);
                //Assert::IsFalse(std::is_assignable<decltype(*crfront), int>::value);

                *front = 42;
                *rfront = 8;
                Assert::IsTrue(*front == 42);
                Assert::IsTrue(*rfront == 8);
            }

            TEST_METHOD(ArrowOperatorTest)
            {
                test_type x;
                x.insert(std::end(x), node_test_class());

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                Assert::IsTrue(std::is_same<decltype(front->doit()), int>::value);
                Assert::IsTrue(std::is_same<decltype(rfront->doit()), int>::value);
                Assert::IsTrue(std::is_same<decltype(cfront->doit()), double>::value);
                Assert::IsTrue(std::is_same<decltype(crfront->doit()), double>::value);
            }

            template <bool _IsConst, typename _It>
            void _IncrementTest(_In_ _It itr)
            {
                // Can only assign if non-const iterator. Again, Visual Studio's bug with
                // is_assignable causes us to be unable to run this test
                //Assert::IsTrue(std::is_assignable<decltype(*(++itr)), int>::value == !_IsConst);
                //Assert::IsTrue(std::is_assignable<decltype(*(itr++)), int>::value == !_IsConst);

                auto val = *itr;
                auto x = itr;
                auto y = itr;

                Assert::IsTrue(*(++x) != val);
                Assert::IsTrue(*(y++) == val);
            }

            TEST_METHOD(IncrementTest)
            {
                dhorn::tree<int> x;
                x.insert(std::end(x), 8);
                x.insert(std::end(x), 42);

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                _IncrementTest<false>(front);
                _IncrementTest<true>(cfront);
                _IncrementTest<false>(rfront);
                _IncrementTest<true>(crfront);
            }
        };



        TEST_CLASS(TreeTests)
        {
            using test_type = dhorn::tree<node_test_class>;

            TEST_METHOD(DefaultConstructorTest)
            {
                // Constructing an empty tree should have no effect on the value type as it is only
                // initializing a sentinel node
                node_test_class::test([]()
                {
                    test_type x;
                    node_test_class::check(0, 0, 0, 0);
                });
            }

            TEST_METHOD(CopyConstructorTest)
            {
                // TODO
            }

            TEST_METHOD(MoveConstructorTest)
            {
                // TODO
            }

            TEST_METHOD(AssignmentTest)
            {
                // TODO
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                // TODO
            }

            TEST_METHOD(IteratorBeginEndTest)
            {
                // Very simple test. Make sure begin() and end() are equal to or are not equal to
                // each other
                dhorn::tree<int> test;

                Assert::IsTrue(test.begin() == test.end());
            }
        };
    }
}
