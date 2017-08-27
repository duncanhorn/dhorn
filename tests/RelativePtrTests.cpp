/*
 * Duncan Horn
 *
 * RelativePtrTests.cpp
 *
 * Tests for the relative_ptr header
 */
#include "stdafx.h"

#include <dhorn/experimental/relative_ptr.h>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::experimental;

namespace dhorn::tests
{
    /*
     * Test Types
     */
    struct base
    {
    };

    struct derived : base
    {
    };



    TEST_CLASS(RelativePtrTests)
    {
        TEST_METHOD(SizeTest)
        {
            Assert::AreEqual(1u, sizeof(relative_ptr8<int>));
            Assert::AreEqual(2u, sizeof(relative_ptr16<int>));
            Assert::AreEqual(4u, sizeof(relative_ptr32<int>));
            Assert::AreEqual(8u, sizeof(relative_ptr64<int>));
        }



#pragma region Constructor Tests

        TEST_METHOD(DefaultConstructorTest)
        {
            relative_ptr8<int> ptr;
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(NullPtrConstructorTest)
        {
            relative_ptr8<int> ptr(nullptr);
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(PointerConstructorTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            Assert::IsTrue(static_cast<bool>(ptr));
            Assert::IsNotNull(ptr.get());
            Assert::AreEqual(&value, ptr.get());
            Assert::AreEqual(42, *ptr.get());

            int* intPtr = nullptr;
            relative_ptr8<int> ptr2(intPtr);
            Assert::IsFalse(static_cast<bool>(ptr2));
            Assert::IsNull(ptr2.get());
        }

        TEST_METHOD(PointerConstructorOutOfRangeTest)
        {
            try
            {
                int values[256];
                relative_ptr8<int> ptr(&values[127]);
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(CopyConstructorTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            relative_ptr8<int> ptrCopy(ptr);
            Assert::IsTrue(static_cast<bool>(ptrCopy));
            Assert::IsNotNull(ptrCopy.get());
            Assert::AreEqual(&value, ptrCopy.get());
            Assert::AreEqual(42, *ptrCopy.get());

            relative_ptr8<int> nullPtr;
            relative_ptr8<int> nullPtrCopy(nullPtr);
            Assert::IsFalse(static_cast<bool>(nullPtrCopy));
            Assert::IsNull(nullPtrCopy.get());
        }

        TEST_METHOD(CopyConstructorOutOfRangeTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            try
            {
                // The heap should be far enough away from the stack...
                std::make_unique<relative_ptr8<int>>(ptr);
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(MoveConstructorTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            relative_ptr8<int> ptrCopy(std::move(ptr));
            Assert::IsTrue(static_cast<bool>(ptrCopy));
            Assert::IsNotNull(ptrCopy.get());
            Assert::AreEqual(&value, ptrCopy.get());
            Assert::AreEqual(42, *ptrCopy.get());

            relative_ptr8<int> nullPtr;
            relative_ptr8<int> nullPtrCopy(std::move(nullPtr));
            Assert::IsFalse(static_cast<bool>(nullPtrCopy));
            Assert::IsNull(nullPtrCopy.get());
        }

        TEST_METHOD(ConversionConstructorTest)
        {
            // Can down-cast; cannot up-cast
            derived d;
            relative_ptr8<derived> derivedPtr(&d);
            relative_ptr8<base> basePtr(derivedPtr);
            Assert::IsTrue(derivedPtr.get() == basePtr.get());

            Assert::IsTrue(std::is_constructible_v<relative_ptr8<base>, relative_ptr8<derived>>);
            Assert::IsFalse(std::is_constructible_v<relative_ptr8<derived>, relative_ptr8<base>>);

            // Can also add const; cannot remove const
            relative_ptr8<const base> constBasePtr(derivedPtr);
            Assert::IsTrue(derivedPtr.get() == constBasePtr.get());

            Assert::IsTrue(std::is_constructible_v<relative_ptr8<const int>, relative_ptr8<int>>);
            Assert::IsFalse(std::is_constructible_v<relative_ptr8<int>, relative_ptr8<const int>>);

            // Can also change the offset size
            relative_ptr16<base> basePtr16(derivedPtr);
            Assert::IsTrue(derivedPtr.get() == basePtr16.get());

            Assert::IsTrue(std::is_constructible_v<relative_ptr16<int>, relative_ptr8<int>>);
            Assert::IsTrue(std::is_constructible_v<relative_ptr8<int>, relative_ptr16<int>>);
        }

        TEST_METHOD(ConversionConstructionOutOfRangeTest)
        {
            int array[256];
            relative_ptr16<int> ptr16(&array[127]);
            try
            {
                relative_ptr8<int> ptr8(ptr16);
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

#pragma endregion



#pragma region Assignment Operator Tests

        TEST_METHOD(NullPtrAssignmentTest)
        {
            int value;
            relative_ptr8<int> ptr(&value);

            ptr = nullptr;
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(CopyAssignmentTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            relative_ptr8<int> ptrCopy;
            ptrCopy = ptr;

            Assert::IsTrue(static_cast<bool>(ptrCopy));
            Assert::IsNotNull(ptrCopy.get());
            Assert::AreEqual(&value, ptrCopy.get());
            Assert::AreEqual(42, *ptrCopy.get());

            relative_ptr8<int> nullPtr;
            relative_ptr8<int> nullPtrCopy;
            nullPtrCopy = nullPtr;

            Assert::IsFalse(static_cast<bool>(nullPtrCopy));
            Assert::IsNull(nullPtrCopy.get());
        }

        TEST_METHOD(CopyAssignmentOutOfRangeTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            try
            {
                // The heap should be far enough away from the stack...
                auto heapPtr = std::make_unique<relative_ptr8<int>>();
                *heapPtr = ptr;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(MoveAssignmentTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            relative_ptr8<int> ptrCopy;
            ptrCopy = std::move(ptr);

            Assert::IsTrue(static_cast<bool>(ptrCopy));
            Assert::IsNotNull(ptrCopy.get());
            Assert::AreEqual(&value, ptrCopy.get());
            Assert::AreEqual(42, *ptrCopy.get());

            relative_ptr8<int> nullPtr;
            relative_ptr8<int> nullPtrCopy;
            nullPtrCopy = std::move(nullPtr);

            Assert::IsFalse(static_cast<bool>(nullPtrCopy));
            Assert::IsNull(nullPtrCopy.get());
        }

        TEST_METHOD(ConversionAssignmentTest)
        {
            // Can down-cast; cannot up-cast
            derived d;
            relative_ptr8<derived> derivedPtr(&d);
            relative_ptr8<base> basePtr;
            basePtr = derivedPtr;

            Assert::IsTrue(derivedPtr.get() == basePtr.get());
            Assert::IsTrue(std::is_assignable_v<relative_ptr8<base>&, relative_ptr8<derived>>);
            Assert::IsFalse(std::is_assignable_v<relative_ptr8<derived>&, relative_ptr8<base>>);

            // Can also add const; cannot remove const
            relative_ptr8<const base> constBasePtr;
            constBasePtr = derivedPtr;

            Assert::IsTrue(derivedPtr.get() == constBasePtr.get());
            Assert::IsTrue(std::is_assignable_v<relative_ptr8<const int>&, relative_ptr8<int>>);
            Assert::IsFalse(std::is_assignable_v<relative_ptr8<int>&, relative_ptr8<const int>>);

            // Can also change the offset size
            relative_ptr16<base> basePtr16;
            basePtr16 = derivedPtr;

            Assert::IsTrue(derivedPtr.get() == basePtr16.get());
            Assert::IsTrue(std::is_assignable_v<relative_ptr16<int>&, relative_ptr8<int>>);
            Assert::IsTrue(std::is_assignable_v<relative_ptr8<int>&, relative_ptr16<int>>);
        }

        TEST_METHOD(ConversionAssignmentOutOfRangeTest)
        {
            int array[256];
            relative_ptr16<int> ptr16(&array[127]);
            try
            {
                relative_ptr8<int> ptr8;
                ptr8 = ptr16;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

#pragma endregion



#pragma region Arithmetic Tests

        TEST_METHOD(AdditionAssignmentTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr = values;

            Assert::AreEqual(values[0], *ptr.get());

            ptr += 1;
            Assert::AreEqual(values[1], *ptr.get());

            ptr += 2;
            Assert::AreEqual(values[3], *ptr.get());

            ptr += -1;
            Assert::AreEqual(values[2], *ptr.get());
        }

#pragma endregion
    };
}
