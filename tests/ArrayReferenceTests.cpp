/*
 * Duncan Horn
 *
 * ArrayReferenceTests.cpp
 *
 * Tests for the array_reference type
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/array_reference.h>
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
                dhorn::array_reference<int> arr;
                Assert::AreEqual(0u, arr.size());
            }

            TEST_METHOD(SingleElementArrayTest)
            {
                int val = 42;
                dhorn::array_reference<int> arr(&val, 1);

                Assert::AreEqual(1u, arr.size());
                Assert::AreEqual(42, arr.at(0));
                Assert::AreEqual(42, arr.front());
                Assert::AreEqual(42, arr.back());
                Assert::AreEqual(42, arr[0]);

                // Should be able to set the value
                arr[0] = 8;
                Assert::AreEqual(8, val);

                arr.at(0) = 10;
                Assert::AreEqual(10, val);
            }

            TEST_METHOD(ConstArrayTest)
            {
                int val = 42;
                const dhorn::array_reference<int> arr(&val, 1);

                Assert::AreEqual(1u, arr.size());
                Assert::AreEqual(42, arr.at(0));
                Assert::AreEqual(42, arr.front());
                Assert::AreEqual(42, arr.back());
                Assert::AreEqual(42, arr[0]);
            }

            TEST_METHOD(MultiElementArrayTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals, dhorn::array_size(vals));

                Assert::AreEqual(dhorn::array_size(vals), arr.size());

                for (size_t i = 0; i < dhorn::array_size(vals); ++i)
                {
                    Assert::AreEqual(vals[i], arr.at(i));
                    Assert::AreEqual(vals[i], arr[i]);
                }
            }

            TEST_METHOD(ArrayConstructionTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals);

                Assert::AreEqual(dhorn::array_size(vals), arr.size());

                for (size_t i = 0; i < dhorn::array_size(vals); ++i)
                {
                    Assert::AreEqual(vals[i], arr.at(i));
                }
            }

            TEST_METHOD(OutOfRangeTest)
            {
                int val = 42;
                dhorn::array_reference<int> arr(&val, 1);

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
                dhorn::array_reference<int> arr(vals);

                // Using std::cend is deliberate here - we want to make sure that we can compare two iterators of
                // different types
                size_t i = 0;
                for (auto itr = std::begin(arr); itr != std::cend(arr); ++itr)
                {
                    auto value = vals[i++];

                    Assert::AreEqual(value, *itr);
                    Assert::AreEqual(value, itr[0]);

                    // Should be able to assign
                    ++(*itr);
                    Assert::AreEqual(value + 1, *itr);
                }
            }

            TEST_METHOD(ForwardConstIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals);

                // Using std::end is deliberate here - we want to make sure that we can compare two iterators of
                // different types
                size_t i = 0;
                for (auto itr = std::cbegin(arr); itr != std::end(arr); ++itr)
                {
                    auto value = vals[i++];

                    Assert::AreEqual(value, *itr);
                    Assert::AreEqual(value, itr[0]);
                }
            }

            TEST_METHOD(IteratorOperatorArrowTest)
            {
                std::string vals[] = { "foo" };
                dhorn::array_reference<std::string> arr(vals);

                auto itr = arr.begin();
                Assert::AreEqual(vals[0].size(), itr->size());

                auto size = itr->size();
                itr->push_back('_');
                Assert::AreEqual(size + 1, itr->size());

                auto constItr = arr.cbegin();
                Assert::AreEqual(vals[0].size(), constItr->size());
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals);

                size_t i = dhorn::array_size(vals) - 1;
                for (auto itr = arr.rbegin(); itr != arr.crend(); ++itr)
                {
                    auto value = vals[i--];

                    Assert::AreEqual(value, *itr);

                    // Should be assignable
                    *itr += 10;
                    Assert::AreEqual(value + 10, *itr);
                }
            }

            TEST_METHOD(ReverseConstIteratorTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals);

                size_t i = dhorn::array_size(vals) - 1;
                for (auto itr = arr.crbegin(); itr != arr.rend(); ++itr)
                {
                    auto value = vals[i--];

                    Assert::AreEqual(value, *itr);
                }
            }

            TEST_METHOD(EqualityTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsTrue(arr1 == arr2);
                Assert::IsTrue(arr2 == arr1);
                Assert::IsFalse(arr1 == arr3);
                Assert::IsFalse(arr3 == arr1);
                Assert::IsFalse(arr1 == arr4);
                Assert::IsFalse(arr4 == arr1);
                Assert::IsFalse(arr1 == arr5);
                Assert::IsFalse(arr5 == arr1);
            }

            TEST_METHOD(InequalityTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsFalse(arr1 != arr2);
                Assert::IsFalse(arr2 != arr1);
                Assert::IsTrue(arr1 != arr3);
                Assert::IsTrue(arr3 != arr1);
                Assert::IsTrue(arr1 != arr4);
                Assert::IsTrue(arr4 != arr1);
                Assert::IsTrue(arr1 != arr5);
                Assert::IsTrue(arr5 != arr1);
            }

            TEST_METHOD(LessThanTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsFalse(arr1 < arr2);
                Assert::IsFalse(arr2 < arr1);
                Assert::IsFalse(arr1 < arr3);
                Assert::IsTrue(arr3 < arr1);
                Assert::IsFalse(arr1 < arr4);
                Assert::IsTrue(arr4 < arr1);
                Assert::IsTrue(arr1 < arr5);
                Assert::IsFalse(arr5 < arr1);
            }

            TEST_METHOD(LessThanOrEqualTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsTrue(arr1 <= arr2);
                Assert::IsTrue(arr2 <= arr1);
                Assert::IsFalse(arr1 <= arr3);
                Assert::IsTrue(arr3 <= arr1);
                Assert::IsFalse(arr1 <= arr4);
                Assert::IsTrue(arr4 <= arr1);
                Assert::IsTrue(arr1 <= arr5);
                Assert::IsFalse(arr5 <= arr1);
            }

            TEST_METHOD(GreaterThanTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsFalse(arr1 > arr2);
                Assert::IsFalse(arr2 > arr1);
                Assert::IsTrue(arr1 > arr3);
                Assert::IsFalse(arr3 > arr1);
                Assert::IsTrue(arr1 > arr4);
                Assert::IsFalse(arr4 > arr1);
                Assert::IsFalse(arr1 > arr5);
                Assert::IsTrue(arr5 > arr1);
            }

            TEST_METHOD(GreaterThanOrEqualTest)
            {
                int vals1[] = { 0, 1, 2, 3, 4 };
                int vals2[] = { 0, 1, 2, 3, 4 };
                int vals3[] = { 0, 1, 2, 3 };
                int vals4[] = { 0, 1, 2, 3, 3 };
                int vals5[] = { 0, 1, 2, 3, 4, 5 };
                dhorn::array_reference<int> arr1(vals1);
                dhorn::array_reference<const int> arr2(vals2);
                dhorn::array_reference<int> arr3(vals3);
                dhorn::array_reference<const int> arr4(vals4);
                dhorn::array_reference<int> arr5(vals5);

                Assert::IsTrue(arr1 >= arr2);
                Assert::IsTrue(arr2 >= arr1);
                Assert::IsTrue(arr1 >= arr3);
                Assert::IsFalse(arr3 >= arr1);
                Assert::IsTrue(arr1 >= arr4);
                Assert::IsFalse(arr4 >= arr1);
                Assert::IsFalse(arr1 >= arr5);
                Assert::IsTrue(arr5 >= arr1);
            }

            TEST_METHOD(ResizeTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals, 2);

                Assert::AreEqual(2u, arr.size());

                arr.resize(dhorn::array_size(vals));
                Assert::AreEqual(dhorn::array_size(vals), arr.size());

                size_t i = 0;
                for (auto &val : arr)
                {
                    Assert::AreEqual(vals[i++], val);
                }
                Assert::AreEqual(dhorn::array_size(vals), i);
            }

            TEST_METHOD(CastToTest)
            {
                uint32_t vals32[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<uint32_t> arr32(vals32);

                auto arr8 = arr32.cast_to<uint8_t>();
                Assert::AreEqual(dhorn::array_size(vals32) * 4, arr8.size());

                auto arr32_2 = arr8.cast_to<uint32_t>();
                Assert::AreEqual(arr32.size(), arr32_2.size());

                uint8_t vals8[] = { 0, 1, 2, 3, 4, 5, 6 };
                arr8 = dhorn::array_reference<uint8_t>(vals8);
                arr32 = arr8.cast_to<uint32_t>();
                Assert::AreEqual(1u, arr32.size());
            }

            TEST_METHOD(IncrementTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals);

                auto test = arr + 1;
                Assert::AreEqual(1, *test);

                test = test + -1;
                Assert::AreEqual(0, *test);

                Assert::AreEqual(1, *(++arr));
                Assert::AreEqual(1, *(arr++)); // arr now points to 2
                Assert::AreEqual(3u, arr.size());

                arr += 2; // Now points to 4
                Assert::AreEqual(4, *arr);

                // Shouldn't assert if the size becomes zero
                ++arr;
                Assert::AreEqual(0u, arr.size());
            }

            TEST_METHOD(DecrementTest)
            {
                int vals[] = { 0, 1, 2, 3, 4 };
                dhorn::array_reference<int> arr(vals + dhorn::array_size(vals), 0);

                auto test = arr - 1;
                Assert::AreEqual(4, *test);

                test = test - -1;
                Assert::AreEqual(0u, test.size());

                Assert::AreEqual(4, *(--arr));
                Assert::AreEqual(4, *(arr--)); // arr now points to 3
                Assert::AreEqual(2u, arr.size());

                arr -= 2; // Now points to 4
                Assert::AreEqual(1, *arr);
            }
        };
    }
}
