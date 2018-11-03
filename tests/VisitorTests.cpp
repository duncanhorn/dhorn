/*
 * Duncan Horn
 *
 * VisitorTests.cpp
 *
 * Tests for the visitor.h functions
 */

#include <dhorn/visitor.h>
#include <gtest/gtest.h>
#include <string>
#include <variant>

using namespace std::literals;

template <int Value>
using tag = std::integral_constant<int, Value>;

TEST(VisitorTests, SingleVisitorTest)
{
    ASSERT_EQ(42, dhorn::visitor{ []() { return 42; } }());
}

TEST(VisitorTests, TwoVisitorsTest)
{
    dhorn::visitor v{
        [](tag<0>) { return 8; },
        [](tag<1>) { return 42; }
    };

    ASSERT_EQ(8, v(tag<0>{}));
    ASSERT_EQ(42, v(tag<1>{}));
}

TEST(VisitorTests, ManyVisitorsTest)
{
    dhorn::visitor v{
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

    ASSERT_EQ(0, v(tag<0>{}));
    ASSERT_EQ(1, v(tag<1>{}));
    ASSERT_EQ(2, v(tag<2>{}));
    ASSERT_EQ(3, v(tag<3>{}));
    ASSERT_EQ(4, v(tag<4>{}));
    ASSERT_EQ(5, v(tag<5>{}));
    ASSERT_EQ(6, v(tag<6>{}));
    ASSERT_EQ(7, v(tag<7>{}));
    ASSERT_EQ(8, v(tag<8>{}));
    ASSERT_EQ(9, v(tag<9>{}));
}

TEST(VisitorTests, StdVisitTest)
{
    dhorn::visitor v{
        [](int&) { return 0; },
        [](float&) { return 1; },
        [](double &) { return 2; },
        [](std::string&) { return 3; }
    };

    std::variant<int, float, double, std::string> value(0);
    ASSERT_EQ(0, std::visit(v, value));

    value = 1.0f;
    ASSERT_EQ(1, std::visit(v, value));

    value = 1.0;
    ASSERT_EQ(2, std::visit(v, value));

    value = "foo";
    ASSERT_EQ(3, std::visit(v, value));
}
