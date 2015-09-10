/*
 * Duncan Horn
 *
 * UnitsTests.cpp
 *
 * Tests for the units.h types/functions
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/units.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UnitsTests)
        {
            TEST_METHOD(PositiveTest)
            {
                dhorn::meters val(1);
                Assert::IsTrue((+val).value() == 1);

                val = -2;
                Assert::IsTrue((+val).value() == -2);

                val = 0;
                Assert::IsTrue((+val).value() == 0);
            }

            TEST_METHOD(NegateTest)
            {
                dhorn::meters val(1);
                Assert::IsTrue((-val).value() == -1);

                val = -2;
                Assert::IsTrue((-val).value() == 2);

                val = 0;
                Assert::IsTrue((-val).value() == 0);
            }

            TEST_METHOD(IncrementTest)
            {
                dhorn::meters val(0);

                Assert::IsTrue((++val).value() == 1);
                Assert::IsTrue(val.value() == 1);

                Assert::IsTrue((val++).value() == 1);
                Assert::IsTrue(val.value() == 2);
            }

            TEST_METHOD(DecrementTest)
            {
                dhorn::meters val(0);

                Assert::IsTrue((--val).value() == -1);
                Assert::IsTrue(val.value() == -1);

                Assert::IsTrue((val--).value() == -1);
                Assert::IsTrue(val.value() == -2);
            }

            TEST_METHOD(MultiplyTest)
            {
                dhorn::meters val(10);
                Assert::IsTrue((val * 10).value() == 100);

                val *= 3;
                Assert::IsTrue(val.value() == 30);

                val = -10;
                Assert::IsTrue((val * 2).value() == -20);

                val *= 4;
                Assert::IsTrue(val.value() == -40);
            }

            TEST_METHOD(DivideTest)
            {
                dhorn::meters val(100);
                Assert::IsTrue((val / 2).value() == 50);

                val /= 20;
                Assert::IsTrue(val.value() == 5);

                val = -100;
                Assert::IsTrue((val / 5).value() == -20);

                val /= 50;
                Assert::IsTrue(val.value() == -2);
            }

            TEST_METHOD(ModulousTest)
            {
                dhorn::meters val(10);
                Assert::IsTrue((val % 3).value() == 1);

                val %= 6;
                Assert::IsTrue(val.value() == 4);

                val = -10;
                Assert::IsTrue((val % 4).value() == -2);

                val %= 7;
                Assert::IsTrue(val.value() == -3);
            }

            // The unit_result_type helper is key in determining the correct return type of arithmetic operations and
            // thus deserves its own tests
            //TEST_METHOD(UnitResultTypeTest)
            //{
            //}

            TEST_METHOD(UnitCastTest)
            {
                dhorn::kilometers km(1);

                // One kilometer is 1000 meters
                auto m = dhorn::unit_cast<dhorn::meters>(km);
                Assert::IsTrue(m.value() == 1000);

                // 2500 meters is 2.5 (rounds down to 2) kilometers
                m = dhorn::meters(2500);
                km = dhorn::unit_cast<dhorn::kilometers>(m);
                Assert::IsTrue(km.value() == 2);
            }
        };
    }
}
