/*
 * UtfStringTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn utf_string classes, structures, functions, etc.
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include <dhorn/type_traits.h>
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
                Assert::AreEqual(1u, traits_type::size(*u8"\u007F"));
                Assert::AreEqual(2u, traits_type::size(*u8"\u07FF"));
                Assert::AreEqual(3u, traits_type::size(*u8"\uFFFF"));
                Assert::AreEqual(4u, traits_type::size(*u8"\U0010FFFF"));
            }

            TEST_METHOD(NextTest)
            {
                unsigned char bytes[][4] =
                {
                    { 0x7F, 0x00, 0x00, 0x00 },
                    { 0xDF, 0xBF, 0x00, 0x00 },
                    { 0xEF, 0xBF, 0xBF, 0x00 },
                    { 0xF4, 0x8F, 0xBF, 0xBF }
                };

                char32_t vals[] = { 0x7Fu, 0x7FFu, 0xFFFFu, 0x10FFFFu };

                for (size_t i = 0; i < array_size(bytes); ++i)
                {
                    Assert::IsTrue(traits_type::next(reinterpret_cast<char *>(bytes[i]), nullptr) == vals[i]);
                }

                unsigned char invalid[] = { 0xF8 };

                try
                {
                    traits_type::next(reinterpret_cast<char *>(invalid), nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0xF8u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                unsigned char bytes[][4] =
                {
                    { 0x7F, 0x00, 0x00, 0x00 },
                    { 0xDF, 0xBF, 0x00, 0x00 },
                    { 0xEF, 0xBF, 0xBF, 0x00 },
                    { 0xF4, 0x8F, 0xBF, 0xBF }
                };

                char32_t vals[] = { 0x7Fu, 0x7FFu, 0xFFFFu, 0x10FFFFu };

                for (size_t i = 0; i < array_size(bytes); ++i)
                {
                    unsigned char buffer[4] = {};
                    traits_type::write(vals[i], reinterpret_cast<char *>(buffer));

                    Assert::IsTrue(std::equal(buffer, buffer + 4, bytes[i]));
                }

                try
                {
                    char buffer[4];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0x1FFFFFu);
                }
            }
        };



        /*
         * utf16_traits Tests
         */
        TEST_CLASS(Utf16TraitsTests)
        {
            using traits_type = dhorn::garbage::utf16_traits;

            TEST_METHOD(SizeTest)
            {
                Assert::AreEqual(1u, traits_type::size(*u"\uD7FF"));
                Assert::AreEqual(1u, traits_type::size(*u"\uE000"));
                Assert::AreEqual(1u, traits_type::size(*u"\uFFFF"));
                Assert::AreEqual(2u, traits_type::size(*u"\U00010000"));
                Assert::AreEqual(2u, traits_type::size(*u"\U0010FFFF"));
            }

        };
    }
}
