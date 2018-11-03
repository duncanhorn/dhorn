/*
 * Duncan Horn
 *
 * ArrayReferenceTests.cpp
 *
 * Tests for the array_reference type
 */
#include "stdafx.h"

#include <dhorn/experimental/array_reference.h>
#include <dhorn/type_traits.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(ArrayReferenceTests)
        {
            TEST_METHOD(EmptyArrayTest)
            {
                dhorn::experimental::array_reference<int> arr;
                ASSERT_EQ(static_cast<std::size_t>(0), arr.size());
            }

            TEST_METHOD(SingleElementArrayTest)
            {
                int val = 42;
                dhorn::experimental::array_reference<int> arr(&val, 1);

                ASSERT_EQ(static_cast<std::size_t>(1), arr.size());
                ASSERT_EQ(42, arr.at(0));
                ASSERT_EQ(42, arr.front());
                ASSERT_EQ(42, arr.back());
                ASSERT_EQ(42, arr[0]);

                // Should be able to set the value
                arr[0] = 8;
                ASSERT_EQ(8, val);

                arr.at(0) = 10;
                ASSERT_EQ(10, val);
            }

            TEST_METHOD(ConstArrayTest)
            {
                int val = 42;
                const dhorn::experimental::array_reference<int> arr(&val, 1);

                ASSERT_EQ(static_cast<std::size_t>(1), arr.size());
                ASSERT_EQ(42, arr.at(0));
                ASSERT_EQ(42, arr.front());
                ASSERT_EQ(42, arr.back());
                ASSERT_EQ(42, arr[0]);
            }

            TEST_METHOD(MultiElementArrayTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals, std::size(vals));

                ASSERT_EQ(std::size(vals), arr.size());

                for (std::size_t i = 0; i < std::size(vals); ++i)
                {
                    ASSERT_EQ(vals[i], arr.at(i));
                    ASSERT_EQ(vals[i], arr[i]);
                }
            }

            TEST_METHOD(ArrayConstructionTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                ASSERT_EQ(std::size(vals), arr.size());

                for (std::size_t i = 0; i < std::size(vals); ++i)
                {
                    ASSERT_EQ(vals[i], arr.at(i));
                }
            }

            TEST_METHOD(OutOfRangeTest)
            {
                int val = 42;
                dhorn::experimental::array_reference<int> arr(&val, 1);

                try
                {
                    arr.at(1) = 0;
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::out_of_range &)
                {
                }
            }

            TEST_METHOD(ForwardIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                // Using cend is deliberate here - we want to make sure that we can compare two iterators of different types
                std::size_t i = 0;
                for (auto itr = arr.begin(); itr != arr.cend(); ++itr)
                {
                    auto value = vals[i++];

                    ASSERT_EQ(value, *itr);
                    ASSERT_EQ(value, itr[0]);

                    // Should be able to assign
                    ++(*itr);
                    ASSERT_EQ(value + 1, *itr);
                }
            }

            TEST_METHOD(ForwardConstIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                // Using end is deliberate here - we want to make sure that we can compare two iterators of different types
                std::size_t i = 0;
                for (auto itr = arr.cbegin(); itr != arr.end(); ++itr)
                {
                    auto value = vals[i++];

                    ASSERT_EQ(value, *itr);
                    ASSERT_EQ(value, itr[0]);
                }
            }

            TEST_METHOD(IteratorOperatorArrowTest)
            {
                std::string vals[] = { "foo" };
                dhorn::experimental::array_reference<std::string> arr(vals);

                auto itr = arr.begin();
                ASSERT_EQ(vals[0].size(), itr->size());

                auto size = itr->size();
                itr->push_back('_');
                ASSERT_EQ(size + 1, itr->size());

                auto constItr = arr.cbegin();
                ASSERT_EQ(vals[0].size(), constItr->size());
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                std::size_t i = std::size(vals) - 1;
                for (auto itr = arr.rbegin(); itr != arr.crend(); ++itr)
                {
                    auto value = vals[i--];

                    ASSERT_EQ(value, *itr);

                    // Should be assignable
                    *itr += 10;
                    ASSERT_EQ(value + 10, *itr);
                }
            }

            TEST_METHOD(ReverseConstIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                std::size_t i = std::size(vals) - 1;
                for (auto itr = arr.crbegin(); itr != arr.rend(); ++itr)
                {
                    auto value = vals[i--];

                    ASSERT_EQ(value, *itr);
                }
            }

            TEST_METHOD(EqualityTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_TRUE(arr1 == arr2);
                ASSERT_TRUE(arr2 == arr1);
                ASSERT_FALSE(arr1 == arr3);
                ASSERT_FALSE(arr3 == arr1);
                ASSERT_FALSE(arr1 == arr4);
                ASSERT_FALSE(arr4 == arr1);
                ASSERT_FALSE(arr1 == arr5);
                ASSERT_FALSE(arr5 == arr1);
            }

            TEST_METHOD(InequalityTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_FALSE(arr1 != arr2);
                ASSERT_FALSE(arr2 != arr1);
                ASSERT_TRUE(arr1 != arr3);
                ASSERT_TRUE(arr3 != arr1);
                ASSERT_TRUE(arr1 != arr4);
                ASSERT_TRUE(arr4 != arr1);
                ASSERT_TRUE(arr1 != arr5);
                ASSERT_TRUE(arr5 != arr1);
            }

            TEST_METHOD(LessThanTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_FALSE(arr1 < arr2);
                ASSERT_FALSE(arr2 < arr1);
                ASSERT_FALSE(arr1 < arr3);
                ASSERT_TRUE(arr3 < arr1);
                ASSERT_FALSE(arr1 < arr4);
                ASSERT_TRUE(arr4 < arr1);
                ASSERT_TRUE(arr1 < arr5);
                ASSERT_FALSE(arr5 < arr1);
            }

            TEST_METHOD(LessThanOrEqualTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_TRUE(arr1 <= arr2);
                ASSERT_TRUE(arr2 <= arr1);
                ASSERT_FALSE(arr1 <= arr3);
                ASSERT_TRUE(arr3 <= arr1);
                ASSERT_FALSE(arr1 <= arr4);
                ASSERT_TRUE(arr4 <= arr1);
                ASSERT_TRUE(arr1 <= arr5);
                ASSERT_FALSE(arr5 <= arr1);
            }

            TEST_METHOD(GreaterThanTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_FALSE(arr1 > arr2);
                ASSERT_FALSE(arr2 > arr1);
                ASSERT_TRUE(arr1 > arr3);
                ASSERT_FALSE(arr3 > arr1);
                ASSERT_TRUE(arr1 > arr4);
                ASSERT_FALSE(arr4 > arr1);
                ASSERT_FALSE(arr1 > arr5);
                ASSERT_TRUE(arr5 > arr1);
            }

            TEST_METHOD(GreaterThanOrEqualTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::experimental::array_reference<int> arr1(vals1);
                dhorn::experimental::array_reference<const int> arr2(vals2);
                dhorn::experimental::array_reference<int> arr3(vals3);
                dhorn::experimental::array_reference<const int> arr4(vals4);
                dhorn::experimental::array_reference<int> arr5(vals5);

                ASSERT_TRUE(arr1 >= arr2);
                ASSERT_TRUE(arr2 >= arr1);
                ASSERT_TRUE(arr1 >= arr3);
                ASSERT_FALSE(arr3 >= arr1);
                ASSERT_TRUE(arr1 >= arr4);
                ASSERT_FALSE(arr4 >= arr1);
                ASSERT_FALSE(arr1 >= arr5);
                ASSERT_TRUE(arr5 >= arr1);
            }

            TEST_METHOD(ResizeTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals, 2);

                ASSERT_EQ(static_cast<std::size_t>(2), arr.size());

                arr.resize(std::size(vals));
                ASSERT_EQ(std::size(vals), arr.size());

                std::size_t i = 0;
                for (auto &val : arr)
                {
                    ASSERT_EQ(vals[i++], val);
                }
                ASSERT_EQ(std::size(vals), i);
            }

            TEST_METHOD(CastToTest)
            {
                std::uint32_t vals32[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<std::uint32_t> arr32(vals32);

                auto arr8 = arr32.cast_to<std::uint8_t>();
                ASSERT_EQ(std::size(vals32) * 4, arr8.size());

                auto arr32_2 = arr8.cast_to<std::uint32_t>();
                ASSERT_EQ(arr32.size(), arr32_2.size());

                std::uint8_t vals8[] = { 0, 1, 2, 3, 4, 5, 6 };
                arr8 = dhorn::experimental::array_reference<std::uint8_t>(vals8);
                arr32 = arr8.cast_to<std::uint32_t>();
                ASSERT_EQ(static_cast<std::size_t>(1), arr32.size());
            }

            TEST_METHOD(IncrementTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals);

                auto test = arr + 1;
                ASSERT_EQ(1, *test);

                test = test + -1;
                ASSERT_EQ(0, *test);

                ASSERT_EQ(1, *(++arr));
                ASSERT_EQ(1, *(arr++)); // arr now points to 2
                ASSERT_EQ(static_cast<std::size_t>(3), arr.size());

                arr += 2; // Now points to 4
                ASSERT_EQ(4, *arr);

                // Shouldn't assert if the size becomes zero
                ++arr;
                ASSERT_EQ(static_cast<std::size_t>(0), arr.size());
            }

            TEST_METHOD(DecrementTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::experimental::array_reference<int> arr(vals + std::size(vals), 0);

                auto test = arr - 1;
                ASSERT_EQ(4, *test);

                test = test - -1;
                ASSERT_EQ(static_cast<std::size_t>(0), test.size());

                ASSERT_EQ(4, *(--arr));
                ASSERT_EQ(4, *(arr--)); // arr now points to 3
                ASSERT_EQ(static_cast<std::size_t>(2), arr.size());

                arr -= 2; // Now points to 4
                ASSERT_EQ(1, *arr);
            }
        };
    }
}
