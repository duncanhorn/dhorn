/*
 * Duncan Horn
 *
 * NullableValueTests.cpp
 *
 * Tests for the nullable_value.h functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/nullable_value.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(NullableValueTests)
        {
            TEST_METHOD_INITIALIZE(MethodInit)
            {
                object_counter::reset();
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                {
                    dhorn::nullable_value<object_counter> value;
                    Assert::AreEqual(0u, object_counter::constructed_count);
                }
                Assert::AreEqual(0u, object_counter::destructed_count);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                {
                    dhorn::nullable_value<object_counter> value;
                    value.set(object_counter{});
                    auto value2 = value;

                    Assert::AreEqual(2u, object_counter::instance_count);
                    Assert::AreEqual(1u, object_counter::copy_count);
                }
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                {
                    dhorn::nullable_value<object_counter> value;
                    value.set(object_counter{});
                    auto value2 = std::move(value);

                    // nullable_value should behave like a pointer, so instance_count should be one here
                    Assert::AreEqual(1u, object_counter::instance_count);
                    Assert::AreEqual(0u, object_counter::copy_count);
                    Assert::AreEqual(2u, object_counter::move_count);
                }
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                dhorn::nullable_value<object_counter> value;
                {
                    dhorn::nullable_value<object_counter> value2;
                    value2.set(object_counter{});
                    value = value2;

                    Assert::AreEqual(2u, object_counter::instance_count);
                    Assert::AreEqual(1u, object_counter::copy_count);
                }
                Assert::AreEqual(1u, object_counter::instance_count);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                dhorn::nullable_value<object_counter> value;
                {
                    dhorn::nullable_value<object_counter> value2;
                    value2.set(object_counter{});
                    value = std::move(value2);

                    // nullable_value should behave like a pointer, so instance_count should be one here
                    Assert::AreEqual(1u, object_counter::instance_count);
                    Assert::AreEqual(0u, object_counter::copy_count);
                    Assert::AreEqual(2u, object_counter::move_count);
                }
                Assert::AreEqual(1u, object_counter::instance_count);
            }

            TEST_METHOD(DereferenceTest)
            {
                dhorn::nullable_value<int> value;
                value.set(42);
                Assert::AreEqual(42, *value);
            }

            TEST_METHOD(OperatorArrowTest)
            {
                dhorn::nullable_value<std::string> value;
                value.set("foobar");
                Assert::AreEqual(6u, value->length());
            }

            TEST_METHOD(OperatorBoolTest)
            {
                dhorn::nullable_value<int> value;
                Assert::IsFalse(value);

                value.set(42);
                Assert::IsTrue(value);

                value.reset();
                Assert::IsFalse(value);
            }

            TEST_METHOD(CopyAssignValueTest)
            {
                {
                    object_counter obj;
                    dhorn::nullable_value<object_counter> value;
                    value.set(obj);
                    Assert::AreEqual(2u, object_counter::instance_count);
                    Assert::AreEqual(1u, object_counter::copy_count);
                    Assert::AreEqual(0u, object_counter::move_count);
                }

                Assert::AreEqual(0u, object_counter::instance_count);
            }

            TEST_METHOD(MoveAssignValueTest)
            {
                {
                    dhorn::nullable_value<object_counter> value;
                    value.set(object_counter{});
                    Assert::AreEqual(1u, object_counter::instance_count);
                    Assert::AreEqual(0u, object_counter::copy_count);
                    Assert::AreEqual(1u, object_counter::move_count);
                }

                Assert::AreEqual(0u, object_counter::instance_count);
            }

            TEST_METHOD(GetValueTest)
            {
                dhorn::nullable_value<int> value;
                value.set(42);
                Assert::AreEqual(42, value.get());
            }

            TEST_METHOD(HasValueTest)
            {
                dhorn::nullable_value<int> value;
                Assert::IsFalse(value.has_value());

                value.set(42);
                Assert::IsTrue(value.has_value());

                value.reset();
                Assert::IsFalse(value.has_value());
            }

            TEST_METHOD(ResetTest)
            {
                dhorn::nullable_value<object_counter> value;
                value.reset(object_counter{});
                Assert::AreEqual(1u, object_counter::instance_count);

                value.reset();
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::nullable_value<int> value1;
                dhorn::nullable_value<int> value2;

                value1.swap(value2);
                Assert::IsFalse(value1);
                Assert::IsFalse(value2);

                value1.set(42);
                value1.swap(value2);
                Assert::IsFalse(value1);
                Assert::IsTrue(value2);
                Assert::AreEqual(42, value2.get());

                value2.set(42);
                value2.swap(value1);
                Assert::IsTrue(value1);
                Assert::IsFalse(value2);
                Assert::AreEqual(42, value1.get());

                value1.swap(value1);
                Assert::IsTrue(value1);
                Assert::AreEqual(42, value1.get());
            }
        };
    }
}
