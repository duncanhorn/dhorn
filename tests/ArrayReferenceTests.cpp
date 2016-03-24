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
        };
    }
}
