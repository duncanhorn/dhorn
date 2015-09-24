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
            TEST_METHOD(EqualityTest)
            {
                dhorn::meters v1(100);
                dhorn::meters v2(150);
                dhorn::meters v3(100);

                Assert::IsFalse(v1 == v2);
                Assert::IsFalse(v2 == v1);
                Assert::IsTrue(v1 == v3);
                Assert::IsTrue(v3 == v1);
                Assert::IsFalse(v2 == v3);
                Assert::IsFalse(v3 == v2);

                Assert::IsTrue(100 == v1);
                Assert::IsTrue(v1 == 100);
                Assert::IsFalse(200 == v1);
                Assert::IsFalse(v1 == 200);
            }

            TEST_METHOD(InequalityTest)
            {
                dhorn::meters v1(100);
                dhorn::meters v2(150);
                dhorn::meters v3(100);

                Assert::IsTrue(v1 != v2);
                Assert::IsTrue(v2 != v1);
                Assert::IsFalse(v1 != v3);
                Assert::IsFalse(v3 != v1);
                Assert::IsTrue(v2 != v3);
                Assert::IsTrue(v3 != v2);

                Assert::IsFalse(100 != v1);
                Assert::IsFalse(v1 != 100);
                Assert::IsTrue(200 != v1);
                Assert::IsTrue(v1 != 200);
            }

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

            TEST_METHOD(AdditionTest)
            {
                dhorn::meters v1(100);
                dhorn::meters v2(250);

                Assert::IsTrue((v1 + 50).value() == 150);
                Assert::IsTrue((150 + v2).value() == 400);
                Assert::IsTrue((v1 + v2).value() == 350);
            }

            TEST_METHOD(SubtractionTest)
            {
                dhorn::meters v1(100);
                dhorn::meters v2(250);

                Assert::IsTrue((v1 - 50).value() == 50);
                Assert::IsTrue((150 - v2).value() == -100);
                Assert::IsTrue((v1 - v2).value() == -150);
            }

            TEST_METHOD(UnitCastTest)
            {
                dhorn::kilometers km(1);

                // Same type should give same value
                auto other = dhorn::unit_cast<dhorn::kilometers>(km);
                Assert::IsTrue(other.value() == 1);

                // One kilometer is 1000 meters
                auto m = dhorn::unit_cast<dhorn::meters>(km);
                Assert::IsTrue(m.value() == 1000);

                // 2500 meters is 2.5 (rounds down to 2) kilometers
                m = dhorn::meters(2500);
                km = dhorn::unit_cast<dhorn::kilometers>(m);
                Assert::IsTrue(km.value() == 2);
            }

            TEST_METHOD(ConstructConvertTest)
            {
                dhorn::kilometers km(1);

                dhorn::meters m(km);
                Assert::IsTrue(m.value() == 1000);

                m = 2000;
                dhorn::kilometers km2(m);
                Assert::IsTrue(km2.value() == 2);
            }

            TEST_METHOD(AssignConvertTest)
            {
                dhorn::kilometers km(1);

                dhorn::meters m(1);
                Assert::IsTrue(m.value() == 1); // Prevent optimizations
                m = km;
                Assert::IsTrue(m.value() == 1000);

                m = 2000;
                km = m;
                Assert::IsTrue(km.value() == 2);
            }
        };
    }
}
