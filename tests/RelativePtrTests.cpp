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



#pragma region Reset Tests

        TEST_METHOD(ResetEmptyTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);

            ptr.reset();
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(ResetNullPtrTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);

            ptr.reset(nullptr);
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(ResetPointerTest)
        {
            relative_ptr8<int> ptr;

            int value = 42;
            ptr.reset(&value);
            Assert::IsTrue(static_cast<bool>(ptr));
            Assert::IsNotNull(ptr.get());
            Assert::AreEqual(&value, ptr.get());
            Assert::AreEqual(42, *ptr.get());

            int* intPtr = nullptr;
            ptr.reset(intPtr);
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

        TEST_METHOD(ResetPointerOutOfRangeTest)
        {
            relative_ptr8<int> ptr;
            try
            {
                int values[256];
                ptr.reset(&values[127]);
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(ResetConversionTest)
        {
            relative_ptr8<base> ptr;

            derived d;
            ptr.reset(&d);
            Assert::IsTrue(static_cast<bool>(ptr));
            Assert::IsNotNull(ptr.get());
            Assert::IsTrue(&d == ptr.get());
        }

#pragma endregion



#pragma region Swap Tests

        TEST_METHOD(SwapTest)
        {
            int values[] = { 8, 42 };
            relative_ptr8<int> ptr0(&values[0]);
            relative_ptr8<int> ptr1(&values[1]);

            ptr0.swap(ptr1);
            Assert::IsTrue(static_cast<bool>(ptr0));
            Assert::IsTrue(static_cast<bool>(ptr1));
            Assert::AreEqual(values[0], *ptr1.get());
            Assert::AreEqual(&values[0], ptr1.get());
            Assert::AreEqual(values[1], *ptr0.get());
            Assert::AreEqual(&values[1], ptr0.get());
        }

        TEST_METHOD(SwapConversionTest)
        {
            int values[] = { 8, 42 };
            relative_ptr8<int> ptr0(&values[0]);
            relative_ptr16<int> ptr1(&values[1]);

            ptr0.swap(ptr1);
            Assert::IsTrue(static_cast<bool>(ptr0));
            Assert::IsTrue(static_cast<bool>(ptr1));
            Assert::AreEqual(values[0], *ptr1.get());
            Assert::AreEqual(&values[0], ptr1.get());
            Assert::AreEqual(values[1], *ptr0.get());
            Assert::AreEqual(&values[1], ptr0.get());
        }

        TEST_METHOD(SwapNullTest)
        {
            int value = 42;
            relative_ptr8<int> ptr0(&value);
            relative_ptr8<int> ptr1;

            ptr0.swap(ptr1);
            Assert::IsFalse(static_cast<bool>(ptr0));
            Assert::IsTrue(static_cast<bool>(ptr1));
            Assert::IsNull(ptr0.get());
            Assert::IsTrue(ptr1.get() == &value);
        }

        TEST_METHOD(SwapOutOfRangeTest)
        {
            int local = 42;
            static int global = 8;

            // 64-bit pointer should be able to reach global no matter what
            relative_ptr8<int> ptr8(&local);
            relative_ptr64<int> ptr64(&global);

            try
            {
                ptr8.swap(ptr64);
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(SelfSwapTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);

            ptr.swap(ptr);
            Assert::IsTrue(static_cast<bool>(ptr));
            Assert::IsTrue(ptr.get() == &value);

            ptr.reset();
            ptr.swap(ptr);
            Assert::IsFalse(static_cast<bool>(ptr));
            Assert::IsNull(ptr.get());
        }

#pragma endregion



#pragma region Accessors Tests

        TEST_METHOD(DereferenceTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);

            Assert::AreEqual(value, *ptr);
        }

        TEST_METHOD(ArrowOperatorTest)
        {
            std::string str = "foobar";
            relative_ptr8<std::string> ptr(&str);

            Assert::AreEqual(str.length(), ptr->length());
        }

        TEST_METHOD(IndexOperatorTest)
        {
            int values[] = { 0, 1, 2, 3, 4, 5, 6 };
            relative_ptr8<int> ptr(&values[3]);

            Assert::AreEqual(values[0], ptr[-3]);
            Assert::AreEqual(values[1], ptr[-2]);
            Assert::AreEqual(values[2], ptr[-1]);
            Assert::AreEqual(values[3], ptr[0]);
            Assert::AreEqual(values[4], ptr[1]);
            Assert::AreEqual(values[5], ptr[2]);
            Assert::AreEqual(values[6], ptr[3]);
        }

#pragma endregion



#pragma region Arithmetic Tests

        TEST_METHOD(PreIncrementTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values);

            Assert::AreEqual(values[1], *++ptr);
            Assert::AreEqual(values[2], *++ptr);
            Assert::AreEqual(values[3], *++ptr);
        }

        TEST_METHOD(PreIncrementOutOfRangeTest)
        {
            int value;
            relative_ptr8<int> ptr(&value);

            try
            {
                for (std::size_t i = 0; i <= 128; ++i)
                {
                    ++ptr;
                }
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(PostIncrementTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values);

            Assert::AreEqual(values[0], *ptr++);
            Assert::AreEqual(values[1], *ptr++);
            Assert::AreEqual(values[2], *ptr++);
        }

        TEST_METHOD(PostIncrementOutOfRangeTest)
        {
            int value;
            relative_ptr8<int> ptr(&value);

            try
            {
                for (std::size_t i = 0; i <= 128; ++i)
                {
                    ptr++;
                }
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(AdditionAssignmentTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values);

            Assert::AreEqual(values[0], *ptr.get());

            ptr += 1;
            Assert::AreEqual(values[1], *ptr.get());

            ptr += 2;
            Assert::AreEqual(values[3], *ptr.get());

            ptr += -1;
            Assert::AreEqual(values[2], *ptr.get());
        }

        TEST_METHOD(AdditionAssignmentOutOfRangeTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            try
            {
                // 7 bits of "forward" space = 128 bytes, or 32 ints
                ptr += 34;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }

            ptr.reset();
            try
            {
                ptr += 42;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(AdditionTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values);
            relative_ptr8<int> ptr2(values + 3);

            Assert::AreEqual(values[3], *(ptr + 3));
            Assert::AreEqual(values[0], *(ptr2 + -3));

            Assert::AreEqual(values[3], *(3 + ptr));
            Assert::AreEqual(values[0], *(-3 + ptr2));

            // NOTE: operator+ doesn't construct a new relative_ptr, so it should never throw
            ptr + 1024;
        }

        TEST_METHOD(PreDecrementTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values + 3);

            Assert::AreEqual(values[2], *--ptr);
            Assert::AreEqual(values[1], *--ptr);
            Assert::AreEqual(values[0], *--ptr);
        }

        TEST_METHOD(PreDecrementOutOfRangeTest)
        {
            int value;
            relative_ptr8<int> ptr(&value);

            try
            {
                for (std::size_t i = 0; i <= 128; ++i)
                {
                    --ptr;
                }
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(PostDecrementTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values + 3);

            Assert::AreEqual(values[3], *ptr--);
            Assert::AreEqual(values[2], *ptr--);
            Assert::AreEqual(values[1], *ptr--);
        }

        TEST_METHOD(PostDecrementOutOfRangeTest)
        {
            int value;
            relative_ptr8<int> ptr(&value);

            try
            {
                for (std::size_t i = 0; i <= 128; ++i)
                {
                    ptr--;
                }
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(SubtractionAssignmentTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values + 3);

            Assert::AreEqual(values[3], *ptr.get());

            ptr -= 1;
            Assert::AreEqual(values[2], *ptr.get());

            ptr -= 2;
            Assert::AreEqual(values[0], *ptr.get());

            ptr -= -1;
            Assert::AreEqual(values[1], *ptr.get());
        }

        TEST_METHOD(SubtractionAssignmentOutOfRangeTest)
        {
            int value = 42;
            relative_ptr8<int> ptr(&value);
            try
            {
                // 7 bits of "forward" space = 128 bytes, or 32 ints
                ptr -= 35;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }

            ptr.reset();
            try
            {
                ptr -= 42;
                Assert::Fail(L"Expected an exception");
            }
            catch (std::range_error&)
            {
            }
        }

        TEST_METHOD(SubtractionTest)
        {
            int values[] = { 0, 1, 2, 3 };
            relative_ptr8<int> ptr(values + 3);
            relative_ptr8<int> ptr2(values);

            Assert::AreEqual(values[0], *(ptr - 3));
            Assert::AreEqual(values[3], *(ptr2 - -3));

            // NOTE: operator- doesn't construct a new relative_ptr, so it should never throw
            ptr - 1024;
        }

        TEST_METHOD(PointerSubtractionTest)
        {
            int values[] = { 0, 1 };
            relative_ptr8<int> ptr(values);
            relative_ptr8<int> ptr2(values + 1);

            Assert::AreEqual(0, ptr - ptr);
            Assert::AreEqual(0, ptr2 - ptr2);
            Assert::AreEqual(1, ptr2 - ptr);
            Assert::AreEqual(-1, ptr - ptr2);
        }

#pragma endregion



#pragma region Comparison Operators

        template <typename Func>
        void DoComparisonOperatorTest(Func&& func)
        {
            int values[2];
            relative_ptr8<int> ptr0(values);
            relative_ptr8<int> ptr1(values);

            func(0, ptr0, ptr1);
            func(0, ptr1, ptr0);

            ++ptr1;
            func(-1, ptr0, ptr1);
            func(1, ptr1, ptr0);

            ptr1.reset();
            func(1, ptr0, ptr1);
            func(-1, ptr1, ptr0);

            // Comparison should not require any operation that could go out of bounds
            auto heapPtr = std::make_unique<int>();
            auto expect = (heapPtr.get() < values) ? 1 : -1;

            relative_ptr64<int> ptr2(heapPtr.get());
            func(expect, ptr0, ptr2);
            func(-expect, ptr2, ptr0);
        }

        TEST_METHOD(EqualityOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value == 0, lhs == rhs);
            });
        }

        TEST_METHOD(InequalityOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value != 0, lhs != rhs);
            });
        }

        TEST_METHOD(LessThanOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value < 0, lhs < rhs);
            });
        }

        TEST_METHOD(LessThanOrEqualOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value <= 0, lhs <= rhs);
            });
        }

        TEST_METHOD(GreaterThanOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value > 0, lhs > rhs);
            });
        }

        TEST_METHOD(GreaterThanOrEqualOperatorTest)
        {
            DoComparisonOperatorTest([](int value, auto& lhs, auto& rhs)
            {
                Assert::AreEqual(value >= 0, lhs >= rhs);
            });
        }

#pragma endregion
    };
}
