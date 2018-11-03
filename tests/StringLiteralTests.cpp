/*
 * Duncan Horn
 *
 * StringLiteralTests.cpp
 *
 * Tests for the string_literal.h header
 */
#include "stdafx.h"

#include <dhorn/utility.h>
#include <dhorn/experimental/string_literal.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::experimental;
using namespace dhorn::literals;
using namespace std::literals;

#define CONSTEXPR_ASSERT_EQUALS(expected, value) \
{ \
    constexpr const auto e = (expected); \
    constexpr const auto v = (value); \
    static_assert(e == v); \
    ASSERT_EQ(e, v); \
}

#define CONSTEXPR_ASSERT_TRUE(value) \
{ \
    constexpr bool v = (value); \
    static_assert(v); \
    ASSERT_TRUE(v); \
}

namespace dhorn::tests
{
    TEST_CLASS(StringLiteralTests)
    {
        TEST_METHOD(DefaultConstructorTest)
        {
            constexpr string_literal<0> s;
        }

        TEST_METHOD(ArrayConstructorTest)
        {
            constexpr string_literal<6> s("foobar");
            constexpr string_literal<0> s2 = "";

            // Class template argument deduction test
            // NOTE: Deduction guides for alias templates not supported in C++17
#if !__INTELLISENSE__
            constexpr basic_string_literal o = "foobar";
            constexpr basic_string_literal o2 = "";
#endif
        }

        TEST_METHOD(ElementAccessOperatorTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_EQUALS('f', s[0]);
            CONSTEXPR_ASSERT_EQUALS('o', s[1]);
            CONSTEXPR_ASSERT_EQUALS('o', s[2]);
            CONSTEXPR_ASSERT_EQUALS('b', s[3]);
            CONSTEXPR_ASSERT_EQUALS('a', s[4]);
            CONSTEXPR_ASSERT_EQUALS('r', s[5]);
            CONSTEXPR_ASSERT_EQUALS('\0', s[6]);

            string_literal<6> s2 = "foobar";
            s2[5] = 'z';
            ASSERT_EQ('z', s2[5]);
        }

        TEST_METHOD(AtTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_EQUALS('f', s.at(0));
            CONSTEXPR_ASSERT_EQUALS('o', s.at(1));
            CONSTEXPR_ASSERT_EQUALS('o', s.at(2));
            CONSTEXPR_ASSERT_EQUALS('b', s.at(3));
            CONSTEXPR_ASSERT_EQUALS('a', s.at(4));
            CONSTEXPR_ASSERT_EQUALS('r', s.at(5));
            CONSTEXPR_ASSERT_EQUALS('\0', s.at(6));

            string_literal<6> s2 = "foobar";
            s2.at(5) = 'z';
            ASSERT_EQ('z', s2.at(5));
        }

        TEST_METHOD(FrontTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_EQUALS('f', s.front());

            string_literal<6> s2 = "foobar";
            s2.front() = 'm';
            ASSERT_EQ('m', s2.front());
        }

        TEST_METHOD(BackTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_EQUALS('r', s.back());

            string_literal<6> s2 = "foobar";
            s2.back() = 'z';
            ASSERT_EQ('z', s2.back());
        }

        TEST_METHOD(CStrTest)
        {
            constexpr string_literal<6> s = "foobar";
            constexpr auto cstr = s.c_str();
            CONSTEXPR_ASSERT_EQUALS('f', cstr[0]);
            CONSTEXPR_ASSERT_EQUALS('o', cstr[1]);
            CONSTEXPR_ASSERT_EQUALS('o', cstr[2]);
            CONSTEXPR_ASSERT_EQUALS('b', cstr[3]);
            CONSTEXPR_ASSERT_EQUALS('a', cstr[4]);
            CONSTEXPR_ASSERT_EQUALS('r', cstr[5]);
            CONSTEXPR_ASSERT_EQUALS('\0', cstr[6]);
        }

        TEST_METHOD(DataTest)
        {
            constexpr string_literal<6> s = "foobar";
            constexpr auto data = s.data();
            CONSTEXPR_ASSERT_EQUALS('f', data[0]);
            CONSTEXPR_ASSERT_EQUALS('o', data[1]);
            CONSTEXPR_ASSERT_EQUALS('o', data[2]);
            CONSTEXPR_ASSERT_EQUALS('b', data[3]);
            CONSTEXPR_ASSERT_EQUALS('a', data[4]);
            CONSTEXPR_ASSERT_EQUALS('r', data[5]);
            CONSTEXPR_ASSERT_EQUALS('\0', data[6]);

            string_literal<6> s2 = "foobar";
            s2.data()[5] = 'z';
            ASSERT_EQ('z', s2.back());
        }

        TEST_METHOD(StringViewOperatorTest)
        {
            constexpr string_literal<6> s = "foobar";
            constexpr std::string_view sv = s;
            CONSTEXPR_ASSERT_EQUALS(6_sz, sv.length());
            CONSTEXPR_ASSERT_EQUALS('f', sv[0]);
            CONSTEXPR_ASSERT_EQUALS('o', sv[1]);
            CONSTEXPR_ASSERT_EQUALS('o', sv[2]);
            CONSTEXPR_ASSERT_EQUALS('b', sv[3]);
            CONSTEXPR_ASSERT_EQUALS('a', sv[4]);
            CONSTEXPR_ASSERT_EQUALS('r', sv[5]);
        }

        // std::equal is not constexpr in C++17 :(
        template <typename Itr0, typename Itr1>
        static constexpr bool are_equal(Itr0 begin0, Itr0 end0, Itr1 begin1, Itr1 end1) noexcept
        {
            for (; (begin0 != end0) && (begin1 != end1); )
            {
                if (*begin0++ != *begin1++)
                {
                    return false;
                }
            }

            return (begin0 == end0) && (begin1 == end1);
        }

        TEST_METHOD(ForwardIterationTest)
        {
            constexpr string_literal<6> s = "foobar";
            constexpr std::string_view sv = "foobar"sv;
            CONSTEXPR_ASSERT_TRUE(are_equal(s.begin(), s.end(), sv.begin(), sv.end()));
            CONSTEXPR_ASSERT_EQUALS('f', s.begin()[0]);
            CONSTEXPR_ASSERT_EQUALS('o', s.begin()[1]);
            CONSTEXPR_ASSERT_EQUALS('o', s.begin()[2]);
            CONSTEXPR_ASSERT_EQUALS('b', s.begin()[3]);
            CONSTEXPR_ASSERT_EQUALS('a', s.begin()[4]);
            CONSTEXPR_ASSERT_EQUALS('r', s.begin()[5]);

            string_literal<6> s2 = "foobar";
            std::string_view sv2 = "moobar"sv;
            *s2.begin() = 'm';
            ASSERT_TRUE(std::equal(s2.begin(), s2.end(), sv2.begin(), sv2.end()));
        }

        TEST_METHOD(ReverseIterationTest)
        {
            constexpr string_literal<6> s = "foobar";
            constexpr std::string_view sv = "raboof"sv;
            CONSTEXPR_ASSERT_TRUE(are_equal(s.rbegin(), s.rend(), sv.begin(), sv.end()));
            CONSTEXPR_ASSERT_EQUALS('r', s.rbegin()[0]);
            CONSTEXPR_ASSERT_EQUALS('a', s.rbegin()[1]);
            CONSTEXPR_ASSERT_EQUALS('b', s.rbegin()[2]);
            CONSTEXPR_ASSERT_EQUALS('o', s.rbegin()[3]);
            CONSTEXPR_ASSERT_EQUALS('o', s.rbegin()[4]);
            CONSTEXPR_ASSERT_EQUALS('f', s.rbegin()[5]);

            string_literal<6> s2 = "foobar";
            std::string_view sv2 = "foobaz"sv;
            *s2.rbegin() = 'z';
            ASSERT_TRUE(std::equal(s2.rbegin(), s2.rend(), sv2.rbegin(), sv2.rend()));
        }

        TEST_METHOD(IteratorConversionTest)
        {
            // This is just a compilation test
            string_literal<6> s = "foobar";
            string_literal<6>::iterator it = s.begin();
            string_literal<6>::const_iterator cit = it;
            cit = it + 2;
            ASSERT_EQ('o', *cit);

            string_literal<6>::reverse_iterator rit = s.rbegin();
            string_literal<6>::const_reverse_iterator crit = rit;
            crit = rit + 2;
            ASSERT_EQ('b', *crit);
        }

        TEST_METHOD(SizeLengthTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_EQUALS(6_sz, s.size());
            CONSTEXPR_ASSERT_EQUALS(6_sz, s.length());

#if !__INTELLISENSE__
            constexpr basic_string_literal s2 = "";
            CONSTEXPR_ASSERT_EQUALS(0_sz, s2.size());
            CONSTEXPR_ASSERT_EQUALS(0_sz, s2.length());
#endif
        }

        TEST_METHOD(EmptyTest)
        {
            constexpr string_literal<6> s = "foobar";
            CONSTEXPR_ASSERT_TRUE(!s.empty());

#if !__INTELLISENSE__
            constexpr basic_string_literal s2 = "";
            CONSTEXPR_ASSERT_TRUE(s2.empty());
#endif
        }

        TEST_METHOD(AppendTest)
        {
            constexpr string_literal<3> foo = "foo";
            constexpr string_literal<3> bar = "bar";
            constexpr auto s = foo + bar;
            CONSTEXPR_ASSERT_EQUALS(6_sz, s.length());
            CONSTEXPR_ASSERT_EQUALS('f', s[0]);
            CONSTEXPR_ASSERT_EQUALS('o', s[1]);
            CONSTEXPR_ASSERT_EQUALS('o', s[2]);
            CONSTEXPR_ASSERT_EQUALS('b', s[3]);
            CONSTEXPR_ASSERT_EQUALS('a', s[4]);
            CONSTEXPR_ASSERT_EQUALS('r', s[5]);
            CONSTEXPR_ASSERT_EQUALS('\0', s[6]);

            //constexpr auto s2 = s + "bar";
            //CONSTEXPR_ASSERT_EQUALS(9_sz, s2.length());
            //CONSTEXPR_ASSERT_EQUALS('f', s2[0]);
            //CONSTEXPR_ASSERT_EQUALS('o', s2[1]);
            //CONSTEXPR_ASSERT_EQUALS('o', s2[2]);
            //CONSTEXPR_ASSERT_EQUALS('b', s2[3]);
            //CONSTEXPR_ASSERT_EQUALS('a', s2[4]);
            //CONSTEXPR_ASSERT_EQUALS('r', s2[5]);
            //CONSTEXPR_ASSERT_EQUALS('b', s2[6]);
            //CONSTEXPR_ASSERT_EQUALS('a', s2[7]);
            //CONSTEXPR_ASSERT_EQUALS('r', s2[8]);
            //CONSTEXPR_ASSERT_EQUALS('\0', s2[9]);
        }
    };
}
