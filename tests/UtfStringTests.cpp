/*
 * UtfStringTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn utf_string classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/utf_string.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        /*
         * utf8_traits Tests
         */
        TEST_CLASS(Utf8TraitsTests)
        {
            using traits_type = dhorn::garbage::utf8_traits;

            TEST_METHOD(SizeTest)
            {
                unsigned char bytes_1[] = { 0x7F };
                unsigned char bytes_2[] = { 0xDF, 0xBF };
                unsigned char bytes_3[] = { 0xEF, 0xBF, 0xBF };
                unsigned char bytes_4[] = { 0xF7, 0xBF, 0xBF, 0xBF };
                unsigned char invalid[] = { 0xF8 };

                Assert::AreEqual(traits_type::size(*bytes_1), 1u);
                Assert::AreEqual(traits_type::size(*bytes_2), 2u);
                Assert::AreEqual(traits_type::size(*bytes_3), 3u);
                Assert::AreEqual(traits_type::size(*bytes_4), 4u);
                Assert::AreEqual(traits_type::size(*invalid), 0u);
            }

            TEST_METHOD(NextTest)
            {
                unsigned char bytes_1[] = { 0x7F };
                unsigned char bytes_2[] = { 0xDF, 0xBF };
                unsigned char bytes_3[] = { 0xEF, 0xBF, 0xBF };
                unsigned char bytes_4[] = { 0xF7, 0xBF, 0xBF, 0xBF };
                unsigned char invalid[] = { 0xF8 };

                Assert::AreEqual(traits_type::next(bytes_1, nullptr), 0x7Fu);
                Assert::AreEqual(traits_type::next(bytes_2, nullptr), 0x7FFu);
                Assert::AreEqual(traits_type::next(bytes_3, nullptr), 0xFFFFu);
                Assert::AreEqual(traits_type::next(bytes_4, nullptr), 0x1FFFFFu);

                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::AreEqual(e.bad_value(), 0xF8u);
                }
            }
        };
    }
}
