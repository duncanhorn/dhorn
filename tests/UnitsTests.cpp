/*
 * Duncan Horn
 *
 * UnitsTests.cpp
 *
 * Tests for the units.h types/functions
 */
#include "stdafx.h"

#include <dhorn/experimental/units.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UnitsTests)
        {
            TEST_METHOD(EqualityTest)
            {
                dhorn::experimental::meters v1(100);
                dhorn::experimental::meters v2(150);
                dhorn::experimental::meters v3(100);

                ASSERT_FALSE(v1 == v2);
                ASSERT_FALSE(v2 == v1);
                ASSERT_TRUE(v1 == v3);
                ASSERT_TRUE(v3 == v1);
                ASSERT_FALSE(v2 == v3);
                ASSERT_FALSE(v3 == v2);

                ASSERT_TRUE(100 == v1);
                ASSERT_TRUE(v1 == 100);
                ASSERT_FALSE(200 == v1);
                ASSERT_FALSE(v1 == 200);
            }

            TEST_METHOD(InequalityTest)
            {
                dhorn::experimental::meters v1(100);
                dhorn::experimental::meters v2(150);
                dhorn::experimental::meters v3(100);

                ASSERT_TRUE(v1 != v2);
                ASSERT_TRUE(v2 != v1);
                ASSERT_FALSE(v1 != v3);
                ASSERT_FALSE(v3 != v1);
                ASSERT_TRUE(v2 != v3);
                ASSERT_TRUE(v3 != v2);

                ASSERT_FALSE(100 != v1);
                ASSERT_FALSE(v1 != 100);
                ASSERT_TRUE(200 != v1);
                ASSERT_TRUE(v1 != 200);
            }

            TEST_METHOD(PositiveTest)
            {
                dhorn::experimental::meters val(1);
                ASSERT_TRUE((+val).value() == 1);

                val = -2;
                ASSERT_TRUE((+val).value() == -2);

                val = 0;
                ASSERT_TRUE((+val).value() == 0);
            }

            TEST_METHOD(NegateTest)
            {
                dhorn::experimental::meters val(1);
                ASSERT_TRUE((-val).value() == -1);

                val = -2;
                ASSERT_TRUE((-val).value() == 2);

                val = 0;
                ASSERT_TRUE((-val).value() == 0);
            }

            TEST_METHOD(IncrementTest)
            {
                dhorn::experimental::meters val(0);

                ASSERT_TRUE((++val).value() == 1);
                ASSERT_TRUE(val.value() == 1);

                ASSERT_TRUE((val++).value() == 1);
                ASSERT_TRUE(val.value() == 2);
            }

            TEST_METHOD(DecrementTest)
            {
                dhorn::experimental::meters val(0);

                ASSERT_TRUE((--val).value() == -1);
                ASSERT_TRUE(val.value() == -1);

                ASSERT_TRUE((val--).value() == -1);
                ASSERT_TRUE(val.value() == -2);
            }

            TEST_METHOD(MultiplyTest)
            {
                dhorn::experimental::meters val(10);
                ASSERT_TRUE((val * 10).value() == 100);

                val *= 3;
                ASSERT_TRUE(val.value() == 30);

                val = -10;
                ASSERT_TRUE((val * 2).value() == -20);

                val *= 4;
                ASSERT_TRUE(val.value() == -40);
            }

            TEST_METHOD(DivideTest)
            {
                dhorn::experimental::meters val(100);
                ASSERT_TRUE((val / 2).value() == 50);

                val /= 20;
                ASSERT_TRUE(val.value() == 5);

                val = -100;
                ASSERT_TRUE((val / 5).value() == -20);

                val /= 50;
                ASSERT_TRUE(val.value() == -2);
            }

            TEST_METHOD(ModulousTest)
            {
                dhorn::experimental::meters val(10);
                ASSERT_TRUE((val % 3).value() == 1);

                val %= 6;
                ASSERT_TRUE(val.value() == 4);

                val = -10;
                ASSERT_TRUE((val % 4).value() == -2);

                val %= 7;
                ASSERT_TRUE(val.value() == -3);
            }

            TEST_METHOD(AdditionTest)
            {
                dhorn::experimental::meters v1(100);
                dhorn::experimental::meters v2(250);

                ASSERT_TRUE((v1 + 50).value() == 150);
                ASSERT_TRUE((150 + v2).value() == 400);
                ASSERT_TRUE((v1 + v2).value() == 350);
            }

            TEST_METHOD(SubtractionTest)
            {
                dhorn::experimental::meters v1(100);
                dhorn::experimental::meters v2(250);

                ASSERT_TRUE((v1 - 50).value() == 50);
                ASSERT_TRUE((150 - v2).value() == -100);
                ASSERT_TRUE((v1 - v2).value() == -150);
            }

            TEST_METHOD(UnitCastTest)
            {
                dhorn::experimental::kilometers km(1);

                // Same type should give same value
                auto other = dhorn::experimental::unit_cast<dhorn::experimental::kilometers>(km);
                ASSERT_TRUE(other.value() == 1);

                // One kilometer is 1000 meters
                auto m = dhorn::experimental::unit_cast<dhorn::experimental::meters>(km);
                ASSERT_TRUE(m.value() == 1000);

                // 2500 meters is 2.5 (rounds down to 2) kilometers
                m = dhorn::experimental::meters(2500);
                km = dhorn::experimental::unit_cast<dhorn::experimental::kilometers>(m);
                ASSERT_TRUE(km.value() == 2);
            }

            TEST_METHOD(ConstructConvertTest)
            {
                dhorn::experimental::kilometers km(1);

                dhorn::experimental::meters m(km);
                ASSERT_TRUE(m.value() == 1000);

                m = 2000;
                dhorn::experimental::kilometers km2(m);
                ASSERT_TRUE(km2.value() == 2);
            }

            TEST_METHOD(AssignConvertTest)
            {
                dhorn::experimental::kilometers km(1);

                dhorn::experimental::meters m(1);
                ASSERT_TRUE(m.value() == 1); // Prevent optimizations
                m = km;
                ASSERT_TRUE(m.value() == 1000);

                m = 2000;
                km = m;
                ASSERT_TRUE(km.value() == 2);
            }
        };
    }
}
