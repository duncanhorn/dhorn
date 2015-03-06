/*
 * SocketsTest.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn sockets classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/tree.h"
#include "dhorn/type_traits.h"
#include <type_traits>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        class node_test_class
        {
        public:
            node_test_class(void) :
                _moved(false),
                _val{}
            {
                instances++;
                constructed++;
            }

            node_test_class(int val) :
                _moved(false),
                _val(val)
            {
                instances++;
                constructed++;
            }

            node_test_class(_In_ const node_test_class &other) :
                _moved(false),
                _val(other._val)
            {
                Assert::IsTrue(!other._moved);

                instances++;
                constructed++;
                copies++;
            }

            node_test_class(_In_ node_test_class &&other) :
                _moved(false),
                _val(other._val)
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

            operator int(void) const
            {
                return this->_val;
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
            int _val;
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
                z = std::move(y);
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
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "foo");
                x.insert(std::end(x), "bar");

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                Assert::IsTrue(*front == "foo");
                Assert::IsTrue(*cfront == "foo");
                Assert::IsTrue(*rfront == "bar");
                Assert::IsTrue(*crfront == "bar");

                // Make sure you can assign to non-const iterators and you can't to const iterators
                Assert::IsTrue(std::is_assignable<decltype(*front), int>::value);
                Assert::IsTrue(std::is_assignable<decltype(*rfront), int>::value);
                Assert::IsFalse(std::is_assignable<decltype(*cfront), int>::value);
                Assert::IsFalse(std::is_assignable<decltype(*crfront), int>::value);

                *front = "bar";
                *rfront = "foo";
                Assert::IsTrue(*front == "bar");
                Assert::IsTrue(*rfront == "foo");
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
                Assert::IsTrue(std::is_assignable<decltype(*(++itr)), std::string>::value == !_IsConst);
                Assert::IsTrue(std::is_assignable<decltype(*(itr++)), std::string>::value == !_IsConst);

                auto val = *itr;
                auto x = itr;
                auto y = itr;

                Assert::IsTrue(*(++x) != val);
                Assert::IsTrue(*(y++) == val);
            }

            TEST_METHOD(IncrementTest)
            {
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "foo");
                x.insert(std::end(x), "bar");

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                _IncrementTest<false>(front);
                _IncrementTest<true>(cfront);
                _IncrementTest<false>(rfront);
                _IncrementTest<true>(crfront);
            }

            template <bool _IsConst, typename _It>
            void _DecrementTest(_In_ _It itr)
            {
                Assert::IsTrue(std::is_assignable<decltype(*(--itr)), std::string>::value == !_IsConst);
                Assert::IsTrue(std::is_assignable<decltype(*(itr--)), std::string>::value == !_IsConst);

                // Cannot assign past the end
                --itr;

                auto val = *itr;
                auto x = itr;
                auto y = itr;

                Assert::IsTrue(*(--x) != val);
                Assert::IsTrue(*(y--) == val);
            }

            TEST_METHOD(DecrementTest)
            {
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "foo");
                x.insert(std::end(x), "bar");

                auto back = std::end(x);
                auto cback = std::cend(x);
                auto rback = std::rend(x);
                auto crback = std::crend(x);

                _DecrementTest<false>(back);
                _DecrementTest<true>(cback);
                _DecrementTest<false>(rback);
                _DecrementTest<true>(crback);
            }

            template <bool _IsConst, typename _It>
            void _AdditionTest(_In_ _It itr)
            {
                Assert::IsTrue(std::is_assignable<decltype(*(itr + 4)), std::string>::value == !_IsConst);
                Assert::IsTrue(std::is_assignable<decltype(*(itr += 4)), std::string>::value == !_IsConst);

                auto val = (*itr)[0];

                Assert::IsTrue((*(itr + 4))[0] + val == '1' + '5');
                Assert::IsTrue((*(itr += 4))[0] + val == '1' + '5');
            }

            TEST_METHOD(AdditionTest)
            {
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "1");
                x.insert(std::end(x), "2");
                x.insert(std::end(x), "3");
                x.insert(std::end(x), "4");
                x.insert(std::end(x), "5");

                auto front = std::begin(x);
                auto cfront = std::cbegin(x);
                auto rfront = std::rbegin(x);
                auto crfront = std::crbegin(x);

                _AdditionTest<false>(front);
                _AdditionTest<true>(cfront);
                _AdditionTest<false>(rfront);
                _AdditionTest<true>(crfront);
            }

            template <bool _IsConst, typename _It>
            void _SubtractionTest(_In_ _It itr)
            {
                Assert::IsTrue(std::is_assignable<decltype(*(itr - 5)), std::string>::value == !_IsConst);
                Assert::IsTrue(std::is_assignable<decltype(*(itr -= 5)), std::string>::value == !_IsConst);

                itr -= 1;
                auto val = (*itr)[0];

                Assert::IsTrue((*(itr - 4))[0] + val == '1' + '5');
                Assert::IsTrue((*(itr -= 4))[0] + val == '1' + '5');
            }

            TEST_METHOD(SubtractionTest)
            {
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "1");
                x.insert(std::end(x), "2");
                x.insert(std::end(x), "3");
                x.insert(std::end(x), "4");
                x.insert(std::end(x), "5");

                auto back = std::end(x);
                auto cback = std::cend(x);
                auto rback = std::rend(x);
                auto crback = std::crend(x);

                _SubtractionTest<false>(back);
                _SubtractionTest<true>(cback);
                _SubtractionTest<false>(rback);
                _SubtractionTest<true>(crback);
            }

            template <bool _IsConst, bool _IsReverse, typename _It>
            void _IndexingTest(_In_ _It itr)
            {
                Assert::IsTrue(std::is_assignable<decltype(itr[0]), std::string>::value == !_IsConst);

                for (int i = 1; i <= 5; i++)
                {
                    Assert::IsTrue(itr[i - 3][0] - '0' == (_IsReverse ? (6 - i) : i));
                }
            }

            TEST_METHOD(IndexingTest)
            {
                dhorn::tree<std::string> x;
                x.insert(std::end(x), "1");
                x.insert(std::end(x), "2");
                x.insert(std::end(x), "3");
                x.insert(std::end(x), "4");
                x.insert(std::end(x), "5");

                _IndexingTest<false, false>(std::begin(x) + 2);
                _IndexingTest<true, false>(std::cbegin(x) + 2);
                _IndexingTest<false, true>(std::rbegin(x) + 2);
                _IndexingTest<true, true>(std::crbegin(x) + 2);
            }

            TEST_METHOD(BeginEndTest)
            {
                // Run the above tests, but with a second level child
                dhorn::tree<std::string> x;
                auto itr = x.insert(std::end(x), "0");
                x.insert(std::end(itr), "1");
                x.insert(std::end(itr), "2");
                x.insert(std::end(itr), "3");
                x.insert(std::end(itr), "4");
                x.insert(std::end(itr), "5");

                auto front = std::begin(itr);
                auto cfront = std::cbegin(itr);
                auto rfront = std::rbegin(itr);
                auto crfront = std::crbegin(itr);

                auto back = std::end(itr);
                auto cback = std::cend(itr);
                auto rback = std::rend(itr);
                auto crback = std::crend(itr);

                // Increment test
                this->_IncrementTest<false>(front);
                this->_IncrementTest<true>(cfront);
                this->_IncrementTest<false>(rfront);
                this->_IncrementTest<true>(crfront);

                // Decrement test
                this->_DecrementTest<false>(back);
                this->_DecrementTest<true>(cback);
                this->_DecrementTest<false>(rback);
                this->_DecrementTest<true>(crback);

                // Addition test
                this->_AdditionTest<false>(front);
                this->_AdditionTest<true>(cfront);
                this->_AdditionTest<false>(rfront);
                this->_AdditionTest<true>(crfront);

                // Subtraction test
                this->_SubtractionTest<false>(back);
                this->_SubtractionTest<true>(cback);
                this->_SubtractionTest<false>(rback);
                this->_SubtractionTest<true>(crback);

                // Indexing test
                this->_IndexingTest<false, false>(front + 2);
                this->_IndexingTest<true, false>(cfront + 2);
                this->_IndexingTest<false, true>(rfront + 2);
                this->_IndexingTest<true, true>(crfront + 2);
            }
        };



        TEST_CLASS(TreeTests)
        {
            using test_type = dhorn::tree<node_test_class>;

            /*
             * The test tree is generated with 10 children of the sentinel, each containing values
             * equal to their index (0, 1, ..., 9). The i'th child has i children with values equal
             * to (0, 1, ..., i - 1). Finally, the fourth child's (index 3) first child (index 0)
             * has three children with values (0, 1, 2). This gives a final size of 58.
             */
            static test_type get_test_tree(void)
            {
                test_type tree;

                for (int i = 0; i < 10; i++)
                {
                    auto itr = tree.insert(std::end(tree), i);

                    for (int j = 0; j < i; j++)
                    {
                        tree.insert(std::end(itr), j);
                    }
                }

                auto pos = std::begin(tree) + 3;
                pos = std::begin(pos);
                for (int i = 0; i < 3; i++)
                {
                    tree.insert(std::end(pos), i);
                }

                return tree;
            }

            static void verify_test_tree(_In_ const test_type &tree)
            {
                Assert::IsTrue(tree.size() == 58);

                Assert::IsTrue(std::distance(std::begin(tree), std::end(tree)) == 10);
                for (int i = 0; i < 10; i++)
                {
                    auto itr = std::begin(tree) + i;
                    Assert::IsTrue(*itr == i);

                    Assert::IsTrue(std::distance(std::begin(itr), std::end(itr)) == i);
                    for (int j = 0; j < i; j++)
                    {
                        auto pos = std::begin(itr) + j;
                        Assert::IsTrue(*pos == j);

                        if (i != 3 || j != 0)
                        {
                            Assert::IsTrue(std::distance(std::begin(pos), std::end(pos)) == 0);
                        }
                        else
                        {
                            Assert::IsTrue(std::distance(std::begin(pos), std::end(pos)) == 3);
                            for (int k = 0; k < 3; k++)
                            {
                                Assert::IsTrue(*(std::begin(pos) + k) == k);
                            }
                        }
                    }
                }
            }

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
                node_test_class::test([]()
                {
                    // Construct a test tree. Note that this will cause moves to occur, but that
                    // does not matter as this is a tree of ints
                    auto tree = get_test_tree();
                    verify_test_tree(tree);

                    test_type copy(tree);
                    verify_test_tree(copy);
                });

                node_test_class::test([]()
                {
                    // Construct a tree where the first node has 10 children with a height of
                    // three. This gives a total of thirty constructed, and 24 moved
                    test_type x;
                    auto itr = std::begin(x);
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            itr = x.insert(itr, node_test_class());
                            ++itr;
                        }
                        itr -= 9;
                    }
                    node_test_class::check(30, 30, 0, 30);

                    test_type copy2(x);
                    node_test_class::check(60, 60, 30, 30);
                });
            }

            TEST_METHOD(MoveConstructorTest)
            {
                node_test_class::test([]()
                {
                    // Construct a test tree. Note that this will cause moves to occur, but that
                    // does not matter as this is a tree of ints
                    auto tree = get_test_tree();
                    verify_test_tree(tree);

                    test_type copy(std::move(tree));
                    verify_test_tree(copy);
                });

                node_test_class::test([]()
                {
                    // Construct a tree where the first node has 10 children with a height of
                    // three. This gives a total of thirty constructed, and 24 moved
                    test_type x;
                    auto itr = std::begin(x);
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            itr = x.insert(itr, node_test_class());
                            ++itr;
                        }
                        itr -= 9;
                    }
                    node_test_class::check(30, 30, 0, 30);

                    test_type copy2(std::move(x));
                    node_test_class::check(30, 30, 0, 30);
                });
            }

            TEST_METHOD(AssignmentTest)
            {
                node_test_class::test([]()
                {
                    // Construct a test tree. Note that this will cause moves to occur, but that
                    // does not matter as this is a tree of ints
                    auto tree = get_test_tree();
                    verify_test_tree(tree);

                    // Make sure no optimizations are taking place
                    test_type copy;
                    Assert::IsTrue(copy.size() == 0);

                    copy = tree;
                    verify_test_tree(copy);
                });

                node_test_class::test([]()
                {
                    // Construct a tree where the first node has 10 children with a height of
                    // three. This gives a total of thirty constructed, and 24 moved
                    test_type x;
                    auto itr = std::begin(x);
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            itr = x.insert(itr, node_test_class());
                            ++itr;
                        }
                        itr -= 9;
                    }
                    node_test_class::check(30, 30, 0, 30);

                    // Make sure no optimizations take place
                    test_type copy2;
                    Assert::IsTrue(copy2.size() == 0);

                    copy2 = x;
                    node_test_class::check(60, 60, 30, 30);
                });
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                node_test_class::test([]()
                {
                    // Construct a test tree. Note that this will cause moves to occur, but that
                    // does not matter as this is a tree of ints
                    auto tree = get_test_tree();
                    verify_test_tree(tree);

                    // Make sure no optimizations are taking place
                    test_type copy;
                    Assert::IsTrue(copy.size() == 0);

                    copy = std::move(tree);
                    verify_test_tree(copy);
                });

                node_test_class::test([]()
                {
                    // Construct a tree where the first node has 10 children with a height of
                    // three. This gives a total of thirty constructed, and 24 moved
                    test_type x;
                    auto itr = std::begin(x);
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 10; j++)
                        {
                            itr = x.insert(itr, node_test_class());
                            ++itr;
                        }
                        itr -= 9;
                    }
                    node_test_class::check(30, 30, 0, 30);

                    // Make sure no optimizations take place
                    test_type copy2;
                    Assert::IsTrue(copy2.size() == 0);

                    copy2 = std::move(x);
                    node_test_class::check(30, 30, 0, 30);
                });
            }

            TEST_METHOD(IteratorBeginEndTest)
            {
                // Very simple test. Make sure begin() and end() are equal to or are not equal to
                // each other and that they have the correct values. Iterators were tested in depth in
                // a separate test
                dhorn::tree<std::string> test;
                Assert::IsTrue(test.begin() == test.end());

                test.insert(std::begin(test), "foo");
                Assert::IsTrue(*std::begin(test) == "foo");
                Assert::IsTrue(std::begin(test) != std::end(test));

                const auto copy = test;
                Assert::IsFalse(std::is_assignable<decltype(*std::begin(copy)), std::string>::value);
                Assert::IsFalse(std::is_assignable<decltype(*std::end(copy)), std::string>::value);
                Assert::IsTrue(std::begin(copy) != std::end(copy));

                Assert::IsTrue(*test.begin() == *copy.rbegin());
            }

            TEST_METHOD(SimpleTest)
            {
                auto tree = get_test_tree();
                verify_test_tree(tree);
            }

            TEST_METHOD(CapacityTest)
            {
                // Add breadth and depth and make sure the size updates correctly. Also test empty()
                dhorn::tree<int> tree;
                Assert::IsTrue(tree.empty());

                for (size_t i = 0; i < 100; i++)
                {
                    tree.insert(std::end(tree), i);
                    Assert::IsTrue(tree.size() == i + 1);
                    Assert::IsTrue(!tree.empty());
                }

                auto itr = std::begin(tree);
                for (size_t i = 0; i < 100; i++)
                {
                    itr = tree.insert(std::end(itr), i);
                    Assert::IsTrue(tree.size() == i + 101);
                    Assert::IsTrue(!tree.empty());
                }
            }

            TEST_METHOD(InsertTest)
            {
                // Insert has been heavily used up until this point. This test mostly makes sure that we can
                // properly insert at random places in the tree and that we can properly perform a move
                // insertion
                node_test_class::test([]()
                {
                    test_type x;

                    // Insert such that the tree is { 9, 8, ..., 0 }
                    for (int i = 0; i < 10; i++)
                    {
                        // Insert by copying val
                        node_test_class val(i);
                        x.insert(std::begin(x), val);
                    }
                    node_test_class::check(10, 20, 10, 0);
                    Assert::IsTrue(x.size() == 10);

                    // Make sure they are in the correct order
                    int expected = 9;
                    for (auto &val : x)
                    {
                        Assert::IsTrue(val == expected--);
                    }

                    // Now, move insert such that the order is { 9, ..., 5, 19, 18, ..., 10, 4, ..., 0 }
                    auto pos = std::begin(x) + 5;
                    for (int i = 10; i < 20; i++)
                    {
                        pos = x.insert(pos, i);
                    }
                    node_test_class::check(20, 30, 10, 10);
                    Assert::IsTrue(x.size() == 20);

                    // Make sure the correct order
                    int index = 0;
                    for (auto &val : x)
                    {
                        expected = (index < 5) ? (9 - index) : (index < 15) ? (24 - index) : (19 - index);
                        Assert::IsTrue(val == expected);
                        index++;
                    }
                });

                // Test inserting a range of values
                node_test_class::test([]()
                {
                    test_type x;
                    std::vector<node_test_class> v;

                    // We want to make sure the iterator returned is correct
                    x.insert(std::end(x), node_test_class(999));
                    x.insert(std::end(x), node_test_class(999));
                    Assert::IsTrue(x.size() == 2);

                    // populate the vector with values 0, ..., 9. Visual Studio's std::vector starts with a
                    // size of zero, so we will hit a total of 4 resizes (cost of 25 additional moves)
                    for (int i = 0; i < 10; i++)
                    {
                        v.push_back(node_test_class(i));
                    }
                    node_test_class::check(12, 12, 0, 37);

                    auto itr = x.insert(std::begin(x) + 1, std::begin(v), std::end(v));
                    node_test_class::check(22, 22, 10, 37);
                    Assert::IsTrue(x.size() == 12);
                    Assert::IsTrue(*itr == 0);

                    for (int i = 1; i <= 10; i++)
                    {
                        Assert::IsTrue(*(std::begin(x) + i) == i - 1);
                    }
                });

                // Test using an initializer list
                node_test_class::test([]()
                {
                    test_type x;
                    x.insert(std::end(x),
                    {
                        node_test_class(0), node_test_class(1), node_test_class(2),
                        node_test_class(3), node_test_class(4), node_test_class(5),
                        node_test_class(6), node_test_class(7), node_test_class(8),
                    });

                    node_test_class::check(9, 18, 9, 0);
                    Assert::IsTrue(x.size() == 9);

                    int next = 0;
                    for (auto &val : x)
                    {
                        Assert::IsTrue(val == next++);
                    }

                    node_test_class::check(9, 18, 9, 0);
                });
            }

            TEST_METHOD(ClearTest)
            {
                node_test_class::test([]()
                {
                    test_type x;

                    for (int i = 0; i < 10; i++)
                    {
                        auto itr = x.insert(std::end(x), i);
                        for (int j = 0; j < 10; j++)
                        {
                            x.insert(std::end(itr), j);
                        }
                    }

                    node_test_class::check(110, 110, 0, 110);
                    x.clear();
                    node_test_class::check(0, 110, 0, 110);
                    Assert::IsTrue(x.size() == 0);
                });
            }

            TEST_METHOD(EmplaceTest)
            {
                node_test_class::test([]()
                {
                    test_type x;

                    for (int i = 0; i < 10; i++)
                    {
                        auto itr = x.emplace(std::end(x), i);
                        for (int j = 0; j < 10; j++)
                        {
                            x.emplace(std::end(itr), j);
                        }
                    }

                    node_test_class::check(110, 110, 0, 0);
                    Assert::IsTrue(x.size() == 110);
                });
            }

            TEST_METHOD(SwapTest)
            {
                node_test_class::test([]()
                {
                    test_type left, right;
                    left = get_test_tree();

                    for (int i = 0; i < 10; i++)
                    {
                        auto pos = right.insert(std::end(right), i);
                        for (int j = 0; j < 10; j++)
                        {
                            right.insert(std::end(pos), j);
                        }
                    }

                    // We have:
                    //      { 58,  58,  0, 58  } for left and
                    //      { 110, 110, 0, 110 } for right
                    node_test_class::check(168, 168, 0, 168);

                    left.swap(right);

                    // Should not have caused any moves, copies, etc.
                    node_test_class::check(168, 168, 0, 168);

                    verify_test_tree(right);
                    for (int i = 0; i < 10; i++)
                    {
                        auto pos = std::begin(left) + i;
                        Assert::IsTrue(*pos == i);
                        for (int j = 0; j < 10; j++)
                        {
                            auto itr = std::begin(pos) + j;
                            Assert::IsTrue(*itr == j);
                        }
                    }

                    // std::swap should work as well
                    std::swap(left, right);
                    node_test_class::check(168, 168, 0, 168);

                    verify_test_tree(left);
                    for (int i = 0; i < 10; i++)
                    {
                        auto pos = std::begin(right) + i;
                        Assert::IsTrue(*pos == i);
                        for (int j = 0; j < 10; j++)
                        {
                            auto itr = std::begin(pos) + j;
                            Assert::IsTrue(*itr == j);
                        }
                    }
                });
            }

            TEST_METHOD(EraseTest)
            {
                // Single element erase
                node_test_class::test([]()
                {
                    test_type x;

                    for (int i = 0; i < 10; i++)
                    {
                        x.insert(std::end(x), i);
                    }

                    node_test_class::check(10, 10, 0, 10);
                    Assert::IsTrue(x.size() == 10);

                    // Erase the first element
                    auto itr = x.erase(std::begin(x));
                    node_test_class::check(9, 10, 0, 10);
                    Assert::IsTrue(x.size() == 9);
                    Assert::IsTrue(*itr == 1);
                    Assert::IsTrue(itr == std::begin(x));
                    Assert::IsTrue(std::distance(std::begin(x), std::end(x)) == 9);

                    // Erase the last element
                    itr = x.erase(std::end(x) - 1);
                    node_test_class::check(8, 10, 0, 10);
                    Assert::IsTrue(x.size() == 8);
                    Assert::IsTrue(itr == std::end(x));
                    Assert::IsTrue(std::distance(std::begin(x), std::end(x)) == 8);

                    // Check the rest
                    for (int i = 0; i < 8; i++)
                    {
                        itr = std::begin(x) + i;
                        Assert::IsTrue(*itr == i + 1);
                    }

                    // Remove from the "middle"
                    itr = x.erase(std::begin(x) + 2);       // Removing value 3
                    node_test_class::check(7, 10, 0, 10);
                    Assert::IsTrue(x.size() == 7);
                    Assert::IsTrue(*itr == 4);
                    Assert::IsTrue(itr == std::begin(x) + 2);
                    Assert::IsTrue(std::distance(std::begin(x), std::end(x)) == 7);


                    // Remove the rest
                    for (int i = 0; i < 7; i++)
                    {
                        itr = x.erase(std::begin(x));
                        node_test_class::check(6 - i, 10, 0, 10);
                        Assert::IsTrue(x.size() == (size_t)(6 - i));
                        Assert::IsTrue(itr == std::begin(x));
                        Assert::IsTrue(std::distance(std::begin(x), std::end(x)) == 6 - i);
                    }
                });

                // Remove a range
                node_test_class::test([]()
                {
                    auto x = get_test_tree();
                    node_test_class::check(58, 58, 0, 58);

                    // 9th index should have 9 children. Remove them all
                    auto itr = std::begin(x) + 9;
                    x.erase(std::begin(itr), std::end(itr));
                    node_test_class::check(49, 58, 0, 58);
                    Assert::IsTrue(std::begin(itr) == std::end(itr));
                    Assert::IsTrue(x.size() == 49);

                    // Remove all but the first and last (of 8) children of the 8th index
                    itr = std::begin(x) + 8;
                    x.erase(std::begin(itr) + 1, std::end(itr) - 1);
                    node_test_class::check(43, 58, 0, 58);
                    Assert::IsTrue(std::begin(itr) != std::end(itr));
                    Assert::IsTrue(std::distance(std::begin(itr), std::end(itr)) == 2);
                    Assert::IsTrue(x.size() == 43);

                    // 3rd index has 3 children, the first of which has 3 children
                    itr = std::begin(x) + 3;
                    x.erase(std::begin(itr), std::begin(itr) + 2);      // removes 5 nodes in total
                    node_test_class::check(38, 58, 0, 58);
                    Assert::IsTrue(std::begin(itr) != std::end(itr));
                    Assert::IsTrue(*std::begin(itr) == 2);
                    Assert::IsTrue(std::distance(std::begin(itr), std::end(itr)) == 1);
                    Assert::IsTrue(x.size() == 38);

                    // Remove the rest
                    x.erase(std::begin(x), std::end(x));
                    node_test_class::check(0, 58, 0, 58);
                    Assert::IsTrue(std::begin(x) == std::end(x));
                    Assert::IsTrue(x.size() == 0);
                });

                // Just make sure that erasing a range works with large-ish height
                node_test_class::test([]()
                {
                    auto x = get_test_tree();
                    node_test_class::check(58, 58, 0, 58);

                    // Remove all children
                    x.erase(std::begin(x), std::end(x));
                    node_test_class::check(0, 58, 0, 58);
                    Assert::IsTrue(std::begin(x) == std::end(x));
                    Assert::IsTrue(x.size() == 0);
                });
            }

            TEST_METHOD(MemoryLeakTest)
            {
                // All previous tests should give a good indication of whether or not there are
                // memory leaks, but don't explicitly test for it. It would be ideal to run all
                // previous tests without the validity checks, but I'm unsure if that is possible.
                // Instead, we replicate the situations that are likely to produce memory leaks and
                // just check those.
                _CrtMemState startState;
                (void)startState;
                _CrtMemCheckpoint(&startState);

                node_test_class::test([]()
                {
                    // Simple test. Create a tree, then destroy it
                    test_type x = get_test_tree();
                    verify_test_tree(x);
                });

                node_test_class::test([]()
                {
                    // Copy
                    auto x = get_test_tree();
                    test_type y(x);
                    verify_test_tree(x);
                    verify_test_tree(y);
                });

                node_test_class::test([]()
                {
                    // Move
                    auto x = get_test_tree();
                    test_type y(std::move(x));
                    verify_test_tree(y);
                });

                node_test_class::test([]()
                {
                    // Copy assignment
                    auto x = get_test_tree();
                    test_type y;

                    auto itr = y.insert(std::end(y), 0);
                    itr = y.insert(std::end(itr), 1);

                    Assert::IsTrue(y.size() == 2);
                    verify_test_tree(x);

                    y = x;
                    verify_test_tree(x);
                    verify_test_tree(y);
                });

                node_test_class::test([]()
                {
                    // Move assignment
                    auto x = get_test_tree();
                    test_type y;

                    auto itr = y.insert(std::end(y), 0);
                    itr = y.insert(std::end(itr), 1);

                    Assert::IsTrue(y.size() == 2);
                    verify_test_tree(x);

                    y = std::move(x);
                    verify_test_tree(y);
                });

                node_test_class::test([]()
                {
                    // Remove all elements before the tree is destroyed
                    test_type x = get_test_tree();
                    x.erase(std::begin(x), std::end(x));
                });

                // Compare ending state of the heap to the initial state
                _CrtMemState endState;
                (void)endState;
                _CrtMemCheckpoint(&endState);

                _CrtMemState diff;
                (void)diff;
                Assert::IsTrue(!_CrtMemDifference(&diff, &startState, &endState));
            }
        };
    }
}
