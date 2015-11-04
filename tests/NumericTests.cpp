/*
 * NumericTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the numeric.h functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/numeric.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(NumericTests)
        {
#pragma region Helpers

            static constexpr double tolerance = 1e-10;

            template <
                typename Ty,
                typename CharT,
                typename = typename std::enable_if<!std::is_same<Ty, double>::value>::type>
            void DoTest(CharT *str, Ty expected)
            {
                auto val = numeric_cast<Ty>(str);
                Assert::AreEqual(expected, val);
            }

            template <typename, typename CharT>
            void DoTest(CharT *str, double expected)
            {
                auto val = numeric_cast<double>(str);
                Assert::AreEqual(expected, val, tolerance);
            }

            template <typename Ty>
            void ExpectException(char *str)
            {
                try
                {
                    numeric_cast<Ty>(str);
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::invalid_argument &)
                {
                }
            }

#pragma endregion



#pragma region Signed Integer Tests

            TEST_METHOD(SimpleSignedIntegerPositiveValueTest)
            {
                DoTest<int>(u8"0", 0);
                DoTest<int>(u8"+1", 1);
                DoTest<int>(u8"1337", 1337);
            }

            TEST_METHOD(SimpleSignedIntegerNegativeValueTest)
            {
                DoTest<int>(u8"-0", 0);
                DoTest<int>(u8"-1", -1);
                DoTest<int>(u8"-1337", -1337);
            }

            TEST_METHOD(SimpleSignedIntegerFractionalPositiveValueTest)
            {
                DoTest<int>(u8"0.5", 0);
                DoTest<int>(u8".001", 0);
                DoTest<int>(u8"+404.99", 404);
            }

            TEST_METHOD(SimpleSignedIntegerFractionalNegativeValueTest)
            {
                DoTest<int>(u8"-0.5", 0);
                DoTest<int>(u8"-.001", 0);
                DoTest<int>(u8"-99.99999999", -99);
            }

            TEST_METHOD(SimpleSignedIntegerExponentialPositiveValueTest)
            {
                DoTest<int>(u8"123.456e3", 123456);
                DoTest<int>(u8"123.456e+1", 1234);
                DoTest<int>(u8"123.456e0", 123);
                DoTest<int>(u8"123.456E4", 1234560);
                DoTest<int>(u8"123.456e-2", 1);
                DoTest<int>(u8"123.456e-3", 0);
                DoTest<int>(u8"123.456e-4", 0);
                DoTest<int>(u8"1e2", 100);
                DoTest<int>(u8"0e3", 0);
                DoTest<int>(u8".123456e2", 12);
            }

            TEST_METHOD(SimpleSignedIntegerExponentialNegativeValueTest)
            {
                DoTest<int>(u8"-123.456e3", -123456);
                DoTest<int>(u8"-123.456e+1", -1234);
                DoTest<int>(u8"-123.456e0", -123);
                DoTest<int>(u8"-123.456E4", -1234560);
                DoTest<int>(u8"-123.456e-2", -1);
                DoTest<int>(u8"-123.456e-3", 0);
                DoTest<int>(u8"-123.456e-4", 0);
                DoTest<int>(u8"-1e2", -100);
                DoTest<int>(u8"-0e3", 0);
                DoTest<int>(u8"-.123456e2", -12);
            }

            TEST_METHOD(LargeValueSignedIntegerPositiveValueTest)
            {
                // Max integer value is 2,147,483,647
                DoTest<int>(u8"2147483647", 2147483647);
                DoTest<int>(u8"0.2147483647e10", 2147483647);
            }

#pragma endregion



#pragma region Unsigned Integer Tests

            TEST_METHOD(SimpleUnsignedIntegerPositiveValueTest)
            {
                DoTest<unsigned int>(u8"0", 0u);
                DoTest<unsigned int>(u8"+1", 1u);
                DoTest<unsigned int>(u8"1337", 1337u);
            }

            TEST_METHOD(SimpleUnsignedIntegerNegativeValueTest)
            {
                // Negative zero is zero, so this should be okay
                DoTest<unsigned int>(u8"-0", 0u);

                ExpectException<unsigned int>(u8"-1");
            }

            TEST_METHOD(SimpleUnsignedIntegerFractionalPositiveValueTest)
            {
                DoTest<unsigned int>(u8"0.5", 0u);
                DoTest<unsigned int>(u8".001", 0u);
                DoTest<unsigned int>(u8"+404.99", 404u);
            }

            TEST_METHOD(SimpleUnsignedIntegerFractionalNegativeValueTest)
            {
                ExpectException<unsigned int>(u8"-0.5");
            }

            TEST_METHOD(SimpleUnsignedIntegerExponentialPositiveValueTest)
            {
                DoTest<unsigned int>(u8"123.456e3", 123456u);
                DoTest<unsigned int>(u8"123.456e+1", 1234u);
                DoTest<unsigned int>(u8"123.456e0", 123u);
                DoTest<unsigned int>(u8"123.456E4", 1234560u);
                DoTest<unsigned int>(u8"123.456e-2", 1u);
                DoTest<unsigned int>(u8"123.456e-3", 0u);
                DoTest<unsigned int>(u8"123.456e-4", 0u);
                DoTest<unsigned int>(u8"1e2", 100u);
                DoTest<unsigned int>(u8"0e3", 0u);
                DoTest<unsigned int>(u8".123456e2", 12u);
            }

            TEST_METHOD(SimpleUnsignedIntegerExponentialNegativeValueTest)
            {
                // Negative zero is zero, so this should be okay
                DoTest<unsigned int>(u8"-0.0e3", 0u);

                ExpectException<unsigned int>(u8"-123.456e3");
                ExpectException<unsigned int>(u8"-123.456e+1");
                ExpectException<unsigned int>(u8"-123.456e0");
                ExpectException<unsigned int>(u8"-123.456E4");
                ExpectException<unsigned int>(u8"-123.456e-2");
                ExpectException<unsigned int>(u8"-123.456e-3");
                ExpectException<unsigned int>(u8"-123.456e-4");
                ExpectException<unsigned int>(u8"-1e2");
                ExpectException<unsigned int>(u8"-.123456e2");
            }

#pragma endregion



#pragma region Double Tests

            TEST_METHOD(SimpleDoublePositiveValueTest)
            {
                DoTest<double>(u8"0", 0.0);
                DoTest<double>(u8"+1", 1.0);
                DoTest<double>(u8"1337", 1337.0);
            }

            TEST_METHOD(SimpleDoubleNegativeValueTest)
            {
                DoTest<double>(u8"-0", 0.0);
                DoTest<double>(u8"-1", -1.0);
                DoTest<double>(u8"-1337", -1337.0);
            }

            TEST_METHOD(SimpleDoubleFractionalPositiveValueTest)
            {
                DoTest<double>(u8"0.5", 0.5);
                DoTest<double>(u8".001", 0.001);
                DoTest<double>(u8"+404.99", 404.99);
            }

            TEST_METHOD(SimpleDoubleFractionalNegativeValueTest)
            {
                DoTest<double>(u8"-0.5", -0.5);
                DoTest<double>(u8"-.001", -0.001);
                DoTest<double>(u8"-99.99999999", -99.99999999);
            }

            TEST_METHOD(SimpleDoubleExponentialPositiveValueTest)
            {
                DoTest<double>(u8"123.456e3", 123456.0);
                DoTest<double>(u8"123.456e+1", 1234.56);
                DoTest<double>(u8"123.456e0", 123.456);
                DoTest<double>(u8"123.456E4", 1234560.0);
                DoTest<double>(u8"123.456e-2", 1.23456);
                DoTest<double>(u8"123.456e-3", 0.123456);
                DoTest<double>(u8"123.456e-4", 0.0123456);
                DoTest<double>(u8"1e2", 100.0);
                DoTest<double>(u8"0e3", 0.0);
                DoTest<double>(u8".123456e2", 12.3456);
            }

            TEST_METHOD(SimpleDoubleExponentialNegativeValueTest)
            {
                DoTest<double>(u8"-123.456e3", -123456.0);
                DoTest<double>(u8"-123.456e+1", -1234.56);
                DoTest<double>(u8"-123.456e0", -123.456);
                DoTest<double>(u8"-123.456E4", -1234560.0);
                DoTest<double>(u8"-123.456e-2", -1.23456);
                DoTest<double>(u8"-123.456e-3", -0.123456);
                DoTest<double>(u8"-123.456e-4", -0.0123456);
                DoTest<double>(u8"-1e2", -100.0);
                DoTest<double>(u8"-0e3", 0.0);
                DoTest<double>(u8"-.123456e2", -12.3456);
            }

#pragma endregion
        };
    }
}
