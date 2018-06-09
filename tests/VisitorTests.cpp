/*
 * Duncan Horn
 *
 * VisitorTests.cpp
 *
 * Tests for the visitor.h functions
 */
#include "stdafx.h"

#include <dhorn/visitor.h>
#include <string>
#include <variant>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::literals;

namespace dhorn::tests
{
    TEST_CLASS(VisitorTests)
    {
        template <int Value>
        using tag = std::integral_constant<int, Value>;

        TEST_METHOD(SingleVisitorTest)
        {
            Assert::AreEqual(42, visitor{ []() { return 42; } }());
        }

        TEST_METHOD(TwoVisitorsTest)
        {
            visitor v{
                [](tag<0>) { return 8; },
                [](tag<1>) { return 42; }
            };

            Assert::AreEqual(8, v(tag<0>{}));
            Assert::AreEqual(42, v(tag<1>{}));
        }

        TEST_METHOD(ManyVisitorsTest)
        {
            visitor v{
                [](tag<0>) { return 0; },
                [](tag<1>) { return 1; },
                [](tag<2>) { return 2; },
                [](tag<3>) { return 3; },
                [](tag<4>) { return 4; },
                [](tag<5>) { return 5; },
                [](tag<6>) { return 6; },
                [](tag<7>) { return 7; },
                [](tag<8>) { return 8; },
                [](tag<9>) { return 9; }
            };

            Assert::AreEqual(0, v(tag<0>{}));
            Assert::AreEqual(1, v(tag<1>{}));
            Assert::AreEqual(2, v(tag<2>{}));
            Assert::AreEqual(3, v(tag<3>{}));
            Assert::AreEqual(4, v(tag<4>{}));
            Assert::AreEqual(5, v(tag<5>{}));
            Assert::AreEqual(6, v(tag<6>{}));
            Assert::AreEqual(7, v(tag<7>{}));
            Assert::AreEqual(8, v(tag<8>{}));
            Assert::AreEqual(9, v(tag<9>{}));
        }

        TEST_METHOD(StdVisitTest)
        {
            visitor v{
                [](int&) { return 0; },
                [](float&) { return 1; },
                [](double &) { return 2; },
                [](std::string&) { return 3; }
            };

            std::variant<int, float, double, std::string> value(0);
            Assert::AreEqual(0, std::visit(v, value));

            value = 1.0f;
            Assert::AreEqual(1, std::visit(v, value));

            value = 1.0;
            Assert::AreEqual(2, std::visit(v, value));

            value = "foo";
            Assert::AreEqual(3, std::visit(v, value));
        }
    };
}
