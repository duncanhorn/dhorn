/*
* Duncan Horn
*
* WindowsTests
*
* Tests for various Win32 functions and wrappers
*/

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/service_container.h>

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
        /*constexpr*/ inline static const dhorn::uuid &id(void)
        {
            static const dhorn::uuid id{ 0x7c026cbf, 0x3df9, 0x4a70,
                { 0xae, 0x26, 0xef, 0x30, 0x11, 0xf1, 0xfe, 0xe2 } };
            return id;
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
        /*constexpr*/ inline static const dhorn::uuid &id(void)
        {
            static const dhorn::uuid id{ 0x7c026cbf, 0x3df9, 0x4a70,
                { 0xae, 0x26, 0xef, 0x30, 0x11, 0xf1, 0xfe, 0xe2 } };
            return id;
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
        /*constexpr*/ inline static const dhorn::uuid &id(void)
        {
            static const dhorn::uuid id{ 0xa8582c22, 0xf943, 0x426d,
                { 0x93, 0x21, 0x76, 0xd, 0x78, 0xc0, 0x4, 0x2 } };
            return id;
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
                // TODO
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                // TODO
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
                catch (std::bad_cast &)
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
                catch (std::bad_cast &)
                {
                }
                Assert::IsTrue(x.size() == 2);

                // Removing test_type_1 should succeed without the destructor running
                auto ptr = x.remove<test_type_1>();
                Assert::IsTrue(ptr->val == 8);
                Assert::IsTrue(x.size() == 1);

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

                    tt2 = ptr2.get();
                    Assert::IsTrue(tt2->val == 42);
                }
                Assert::IsTrue(tt2->val != 42);
            }
        };
    }
}
