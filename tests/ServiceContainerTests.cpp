/*
 * Duncan Horn
 *
 * WindowsTests
 *
 * Tests for various Win32 functions and wrappers
 */
#include "stdafx.h"

#include <dhorn/experimental/service_container.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    struct test_type_1
    {
    public:
        ~test_type_1(void) { val = 0; }
        int val = 8;
    };

    template <>
    struct service_type_traits<test_type_1>
    {
        inline static const dhorn::uuid id(void)
        {
            return { 0x7c026cbf, 0x3df9, 0x4a70, { 0xae, 0x26, 0xef, 0x30, 0x11, 0xf1, 0xfe, 0xe2 } };
        }
    };

    struct fake_test_type_1
    {
    public:
        int val = -1;
    };

    template <>
    struct service_type_traits<fake_test_type_1>
    {
        inline static const dhorn::uuid id(void)
        {
            return { 0x7c026cbf, 0x3df9, 0x4a70, { 0xae, 0x26, 0xef, 0x30, 0x11, 0xf1, 0xfe, 0xe2 } };
        }
    };



    struct test_type_2
    {
    public:
        ~test_type_2(void) { val = 0; }
        int val = 42;
    };

    template <>
    struct service_type_traits<test_type_2>
    {
        inline static const dhorn::uuid id(void)
        {
            return { 0xa8582c22, 0xf943, 0x426d, { 0x93, 0x21, 0x76, 0xd, 0x78, 0xc0, 0x4, 0x2 } };
        }
    };



    namespace tests
    {
        TEST_CLASS(ServiceContainerTests)
        {
            TEST_METHOD(DefaultConstructorTest)
            {
                dhorn::service_container x;
                Assert::IsTrue(x.size() == 0);
                Assert::IsTrue(x.empty());
            }

            TEST_METHOD(MoveConstructorTest)
            {
                dhorn::service_container x;

                x.insert(new test_type_1());
                x.insert(new test_type_2());
                Assert::IsTrue(x.size() == 2);

                dhorn::service_container y(std::move(x));
                Assert::IsTrue(y.size() == 2);

                // Pointers should still be there
                Assert::IsTrue(y.find<test_type_1>()->val == 8);
                Assert::IsTrue(y.find<test_type_2>()->val == 42);

                // Shouldn't be able to find fake_test_type_1
                try
                {
                    y.find<fake_test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &)
                {
                }
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::service_container y;
                {
                    dhorn::service_container x;

                    x.insert(new test_type_1());
                    x.insert(new test_type_2());
                    Assert::IsTrue(x.size() == 2);
                    Assert::IsTrue(y.empty());

                    y = std::move(x);
                }
                Assert::IsTrue(y.size() == 2);

                // Pointers should still be there
                Assert::IsTrue(y.find<test_type_1>()->val == 8);
                Assert::IsTrue(y.find<test_type_2>()->val == 42);

                // Shouldn't be able to find fake_test_type_1
                try
                {
                    y.find<fake_test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &)
                {
                }
            }

            TEST_METHOD(PointerInsertTest)
            {
                dhorn::service_container x;
                Assert::IsTrue(x.size() == 0);

                x.insert(new test_type_1());
                Assert::IsTrue(x.size() == 1);

                x.insert(new test_type_2(), std::default_delete<test_type_2>());
                Assert::IsTrue(x.size() == 2);

                // Make sure exceptions are thrown
                try
                {
                    x.insert(new test_type_1());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::test_type_1"));
                }
                Assert::IsTrue(x.size() == 2);

                try
                {
                    x.insert(new test_type_2());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::test_type_2"));
                }
                Assert::IsTrue(x.size() == 2);

                // fake_test_type_1 has same uuid as test_type_1, so it should fail
                try
                {
                    x.insert(new fake_test_type_1());
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::fake_test_type_1"));
                }
                Assert::IsTrue(x.size() == 2);
            }

            TEST_METHOD(EmplaceTest)
            {
                dhorn::service_container x;
                Assert::IsTrue(x.size() == 0);

                x.emplace<test_type_1>();
                Assert::IsTrue(x.size() == 1);

                x.emplace<test_type_2>();
                Assert::IsTrue(x.size() == 2);

                // Make sure exceptions are thrown
                try
                {
                    x.emplace<test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::test_type_1"));
                }
                Assert::IsTrue(x.size() == 2);

                try
                {
                    x.emplace<test_type_2>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::test_type_2"));
                }
                Assert::IsTrue(x.size() == 2);

                // fake_test_type_1 has same uuid as test_type_1, so it should fail
                try
                {
                    x.emplace<fake_test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::fake_test_type_1"));
                }
                Assert::IsTrue(x.size() == 2);
            }

            TEST_METHOD(AllocateTest)
            {
                dhorn::service_container x;
                Assert::IsTrue(x.size() == 0);
                Assert::IsTrue(x.empty());

                x.allocate<test_type_1>(std::allocator<test_type_1>());
                Assert::IsTrue(x.size() == 1);
                Assert::IsTrue(!x.empty());
                Assert::IsTrue(x.find<test_type_1>()->val == 8);

                // Cannot add again
                try
                {
                    x.allocate<test_type_1>(std::allocator<test_type_1>());
                }
                catch (dhorn::service_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_published : struct dhorn::test_type_1"));
                }
            }

            TEST_METHOD(FindTest)
            {
                dhorn::service_container x;

                // Should throw exception
                try
                {
                    x.find<test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_not_published : struct dhorn::test_type_1"));
                }

                // No exception after inserting
                x.insert(new test_type_1());
                auto ptr = x.find<test_type_1>();
                Assert::IsTrue(ptr->val == 8);

                // Should throw exception
                try
                {
                    x.find<test_type_2>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_not_published : struct dhorn::test_type_2"));
                }

                // No exception after inserting
                x.insert(new test_type_2());
                auto ptr2 = x.find<test_type_2>();
                Assert::IsTrue(ptr2->val == 42);

                // test_type_1 should still be there
                ptr = x.find<test_type_1>();
                Assert::IsTrue(ptr->val == 8);

                // Even though fake_test_type_1 has the same uuid, it should still give bad_cast
                try
                {
                    x.find<fake_test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &)
                {
                }
            }

            TEST_METHOD(RemoveTest)
            {
                dhorn::service_container x;
                x.insert(new test_type_1());
                x.insert(new test_type_2());
                Assert::IsTrue(x.size() == 2);

                // Trying to remove fake_test_type_1 should give bad_cast
                try
                {
                    x.remove<fake_test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &)
                {
                }
                Assert::IsTrue(x.size() == 2);

                // Removing test_type_1 should succeed without the destructor running
                auto ptr = x.remove<test_type_1>();
                Assert::IsTrue(ptr->val == 8);
                Assert::IsTrue(x.size() == 1);

                // Cannot find
                try
                {
                    x.find<test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_not_published : struct dhorn::test_type_1"));
                }

                // Should be able to add test_type_1 again
                x.insert(new test_type_1());
                Assert::IsTrue(x.find<test_type_1>()->val == 8);
                Assert::IsTrue(x.size() == 2);
                Assert::IsTrue(x.remove<test_type_1>()->val == 8);
                Assert::IsTrue(x.size() == 1);

                // Can still find test_type_2
                Assert::IsTrue(x.find<test_type_2>()->val == 42);

                // Attempting to remove test_type_1 again should give an exception
                try
                {
                    x.remove<test_type_1>();
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::service_not_published &e)
                {
                    Assert::IsTrue(e.what() == std::string("service_not_published : struct dhorn::test_type_1"));
                }

                // Removing test_type_2 should give valid pointer that gets the destructor run
                // when the result falls out of scope
                test_type_2 *tt2;
                {
                    auto ptr2 = x.remove<test_type_2>();
                    Assert::IsTrue(x.size() == 0);
                    Assert::IsTrue(x.empty());

                    tt2 = ptr2.get();
                    Assert::IsTrue(tt2->val == 42);
                }
                Assert::IsTrue(tt2->val != 42);
            }

            TEST_METHOD(MemoryLeakTest)
            {
                _CrtMemState start;
                UNREFERENCED_PARAMETER(start);
                _CrtMemCheckpoint(&start);

                {
                    dhorn::service_container x;
                    auto ptr1 = x.insert(new test_type_1());
                    auto ptr2 = x.insert(new test_type_2());

                    // Assertions shouldn't cause memory leaks
                    try
                    {
                        auto ptr = x.insert(new test_type_1());
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (dhorn::service_published &)
                    {
                    }

                    try
                    {
                        auto ptr = x.emplace<test_type_2>();
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (dhorn::service_published &)
                    {
                    }

                    try
                    {
                        auto ptr = x.allocate<test_type_1>(std::allocator<test_type_1>());
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (dhorn::service_published &)
                    {
                    }

                    auto ptr3 = x.remove<test_type_1>();
                    Assert::IsTrue(ptr3 == ptr1);

                    x.insert(ptr3);
                    Assert::IsTrue(x.remove<test_type_1>() == ptr1);

                    // Moving shouldn't have any effect on memory
                    dhorn::service_container y(std::move(x));
                    y.emplace<test_type_1>();
                    Assert::IsTrue(y.find<test_type_1>() != ptr1);

                    Assert::IsTrue(y.remove<test_type_2>() == ptr2);
                    y.allocate<test_type_2>(std::allocator<test_type_2>());
                    Assert::IsTrue(y.find<test_type_2>() != ptr2);
                }

                _CrtMemState end;
                UNREFERENCED_PARAMETER(end);
                _CrtMemCheckpoint(&end);

                _CrtMemState diff;
                UNREFERENCED_PARAMETER(diff);
                Assert::IsTrue(!_CrtMemDifference(&diff, &start, &end));
            }
        };
    }
}
