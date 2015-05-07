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
                std::pair<char *, char32_t> vals[] =
                {
                    { u8"\u007F", 0x007F },
                    { u8"\u07FF", 0x07FF },
                    { u8"\uFFFF", 0x0000FFFF },
                    { u8"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
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
                std::pair<char *, char32_t> vals[] =
                {
                    { u8"\u007F\0\0\0", 0x007F },
                    { u8"\u07FF\0\0", 0x07FF },
                    { u8"\uFFFF\0", 0x0000FFFF },
                    { u8"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    char buffer[4] = {};
                    traits_type::write(pair.second, buffer);

                    Assert::IsTrue(std::equal(buffer, buffer + 4, pair.first));
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

            TEST_METHOD(NextTest)
            {
                std::pair<char16_t *, char32_t> vals[] =
                {
                    { u"\uD7FF", 0x0000D7FF },
                    { u"\uE000", 0x0000E000 },
                    { u"\uFFFF", 0x0000FFFF },
                    { u"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char16_t invalid[] = { 0xDC00 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0xDC00u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                std::pair<char16_t *, char32_t> vals[] =
                {
                    { u"\uD7FF\0", 0x0000D7FF },
                    { u"\uE000\0", 0x0000E000 },
                    { u"\uFFFF\0", 0x0000FFFF },
                    { u"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    char16_t buffer[2] = {};
                    traits_type::write(pair.second, buffer);

                    Assert::IsTrue(std::equal(buffer, buffer + 2, pair.first));
                }

                try
                {
                    char16_t buffer[2];
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
         * utf32_traits Tests
         */
        TEST_CLASS(Utf32TraitsTests)
        {
            using traits_type = dhorn::garbage::utf32_traits;

            TEST_METHOD(SizeTest)
            {
                Assert::AreEqual(1u, traits_type::size(*U"\uD7FF"));
                Assert::AreEqual(1u, traits_type::size(*U"\uE000"));
                Assert::AreEqual(1u, traits_type::size(*U"\uFFFF"));
                Assert::AreEqual(1u, traits_type::size(*U"\U00010000"));
                Assert::AreEqual(1u, traits_type::size(*U"\U0010FFFF"));
            }

            TEST_METHOD(NextTest)
            {
                std::pair<char32_t *, char32_t> vals[] =
                {
                    { U"\uD7FF", 0x0000D7FF },
                    { U"\uE000", 0x0000E000 },
                    { U"\uFFFF", 0x0000FFFF },
                    { U"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char32_t invalid[] = { 0x110000 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0x110000u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                std::pair<char32_t *, char32_t> vals[] =
                {
                    { U"\uD7FF\0", 0x0000D7FF },
                    { U"\uE000\0", 0x0000E000 },
                    { U"\uFFFF\0", 0x0000FFFF },
                    { U"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    char32_t buffer[1] = {};
                    traits_type::write(pair.second, buffer);

                    Assert::IsTrue(std::equal(buffer, buffer + 1, pair.first));
                }

                try
                {
                    char32_t buffer[1];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0x1FFFFFu);
                }
            }
        };



        TEST_CLASS(Utf8StringTests)
        {
            using string_type = dhorn::utf8_string;
        };
    }
}
