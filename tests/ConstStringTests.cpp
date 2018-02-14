/*
 * Duncan Horn
 *
 * CRTPBaseTests.cpp
 *
 * Tests for the crtp_base.h header
 */
#include "stdafx.h"

#include <dhorn/experimental/const_string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace dhorn::experimental;
using namespace std::literals;

namespace dhorn::tests
{
    TEST_CLASS(ConstStringTests)
    {
#pragma region Constructor Tests

        TEST_METHOD(DefaultConstructorTest)
        {
            constexpr const_string<8> s;
            Assert::AreEqual(static_cast<std::size_t>(8), s.size());
            static_assert(s.size() == 8);
            static_assert(s[0] == '\0');
            static_assert(s[8] == '\0');

            for (std::size_t i = 0; i < 8; ++i)
            {
                Assert::AreEqual('\0', s[i]);
            }
        }

        TEST_METHOD(CharacterSplatConstructorTest)
        {
            constexpr const_string<8> s('a');
            Assert::AreEqual(static_cast<std::size_t>(8), s.size());
            static_assert(s.size() == 8);
            static_assert(s[0] == 'a');
            static_assert(s[8] == '\0');

            for (std::size_t i = 0; i < 8; ++i)
            {
                Assert::AreEqual('a', s[i]);
            }
        }

        TEST_METHOD(NullTerminatedCStringConstructorTest)
        {
            constexpr const_string<6> s("foobar");
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');

            // These should all fail to compile
            //constexpr const_string<5> s1("foobar");
            //constexpr const_string<7> s2("foobar");
        }

        TEST_METHOD(ExplicitLengthCStringConstructorTest)
        {
            constexpr const_string<3> s("foobar", 3);
            Assert::AreEqual(static_cast<std::size_t>(3), s.size());
            static_assert(s.size() == 3);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == '\0');

            // These should all fail to compile
            //constexpr const_string<4> s1("foobar", 3);
            //constexpr const_string<2> s2("foobar", 3);
        }

        TEST_METHOD(StringViewConstructorTest)
        {
            constexpr const_string<6> s("foobar"sv);
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');
        }

        TEST_METHOD(CopyConstructorTest)
        {
            constexpr const_string<6> s("foobar");
            constexpr const_string<6> s_copy(s);
            Assert::AreEqual(static_cast<std::size_t>(6), s_copy.size());
            static_assert(s_copy.size() == 6);
            static_assert(s_copy[0] == 'f');
            static_assert(s_copy[1] == 'o');
            static_assert(s_copy[2] == 'o');
            static_assert(s_copy[3] == 'b');
            static_assert(s_copy[4] == 'a');
            static_assert(s_copy[5] == 'r');
            static_assert(s_copy[6] == '\0');
        }

#pragma endregion



#pragma region Assignment Tests

        TEST_METHOD(NullTerminatedCStringAssignmentOperatorTest)
        {
            auto make_string = []() constexpr
            {
                const_string<6> s;
                s = "foobar";
                return s;
            };

            constexpr const_string<6> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');
        }

        TEST_METHOD(NullTerminatedCStringAssignTest)
        {
            auto make_string = []() constexpr
            {
                const_string<6> s;
                s.assign("foobar");
                return s;
            };

            constexpr const_string<6> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');
        }

        TEST_METHOD(StringViewAssignmentOperatorTest)
        {
            auto make_string = []() constexpr
            {
                const_string<6> s;
                s = "foobar"sv;
                return s;
            };

            constexpr const_string<6> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');
        }

        TEST_METHOD(StringViewAssignTest)
        {
            auto make_string = []() constexpr
            {
                const_string<6> s;
                s.assign("foobar"sv);
                return s;
            };

            constexpr const_string<6> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(6), s.size());
            static_assert(s.size() == 6);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == 'b');
            static_assert(s[4] == 'a');
            static_assert(s[5] == 'r');
            static_assert(s[6] == '\0');
        }

        TEST_METHOD(AssignCharacterSplatTest)
        {
            auto make_string = []() constexpr
            {
                const_string<8> s;
                s.assign('a');
                return s;
            };

            constexpr const_string<8> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(8), s.size());
            static_assert(s.size() == 8);
            static_assert(s[0] == 'a');
            static_assert(s[8] == '\0');

            for (std::size_t i = 0; i < 8; ++i)
            {
                Assert::AreEqual('a', s[i]);
            }
        }

        TEST_METHOD(ExplicitLengthCStringAssignTest)
        {
            auto make_string = []() constexpr
            {
                const_string<3> s;
                s.assign("foobar", 3);
                return s;
            };

            constexpr const_string<3> s = make_string();
            Assert::AreEqual(static_cast<std::size_t>(3), s.size());
            static_assert(s.size() == 3);
            static_assert(s[0] == 'f');
            static_assert(s[1] == 'o');
            static_assert(s[2] == 'o');
            static_assert(s[3] == '\0');
        }

#pragma endregion



#pragma region Element Access Tests

        TEST_METHOD(IndexOperatorTest)
        {
            const_string<6> s = "foobar";
            s[5] = 'z';
            Assert::AreEqual('z', s[5]);

            const const_string<6> s_const = s;
            Assert::IsFalse(std::is_assignable_v<decltype(s_const[0]), char>);
        }

        TEST_METHOD(FrontTest)
        {
            const_string<6> s = "foobar";
            Assert::AreEqual('f', s.front());
            s.front() = 'g';
            Assert::AreEqual('g', s.front());

            constexpr const const_string<6> s_const = "foobar";
            static_assert(s_const.front() == 'f');
            Assert::IsFalse(std::is_assignable_v<decltype(s_const.front()), char>);
        }

        TEST_METHOD(BackTest)
        {
            const_string<6> s = "foobar";
            Assert::AreEqual('r', s.back());
            s.back() = 'z';
            Assert::AreEqual('z', s.back());

            constexpr const const_string<6> s_const = "foobar";
            static_assert(s_const.back() == 'r');
            Assert::IsFalse(std::is_assignable_v<decltype(s_const.back()), char>);
        }

        TEST_METHOD(DataCStrTest)
        {
            const_string<6> s = "foobar";
            auto ptr = s.data();
            ptr[5] = 'z';
            Assert::AreEqual(0, ::strcmp(s.c_str(), "foobaz"));
            Assert::IsTrue(std::is_assignable_v<decltype(*s.data()), char>);
            Assert::IsFalse(std::is_assignable_v<decltype(*s.c_str()), char>);

            const const_string<6> s_const = s;
            Assert::IsFalse(std::is_assignable_v<decltype(*s_const.data()), char>);
            Assert::IsFalse(std::is_assignable_v<decltype(*s_const.c_str()), char>);
        }

        TEST_METHOD(OperatorStringViewTest)
        {
            constexpr const_string<6> s = "foobar";
            constexpr auto sv = static_cast<std::string_view>(s);
            static_assert(sv.length() == 6);
            static_assert(sv[0] == 'f');
            static_assert(sv[1] == 'o');
            static_assert(sv[2] == 'o');
            static_assert(sv[3] == 'b');
            static_assert(sv[4] == 'a');
            static_assert(sv[5] == 'r');
        }

#pragma endregion



#pragma region Operations

        TEST_METHOD(AppendCharactersTest)
        {

        }

#pragma endregion
    };
}
