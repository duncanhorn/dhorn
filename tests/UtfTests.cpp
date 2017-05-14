/*
 * Duncan Horn
 *
 * UtfTests.cpp
 *
 * Tests for the utf.h functions and types
 */
#include "stdafx.h"

#include <dhorn/utf.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UtfTests)
        {
#pragma region code_point_size Tests

            TEST_METHOD(Utf8CodePointSizeTest)
            {
                // Test the ASCII characters (requires one code unit)
                for (int ch = 0x00; ch < 0x80; ++ch)
                {
                    Assert::AreEqual(1u, utf_traits<char>::code_point_size(static_cast<char>(ch)));
                }

                // Test the characters that require two code units
                for (int ch = 0xC0; ch < 0xE0; ++ch)
                {
                    Assert::AreEqual(2u, utf_traits<char>::code_point_size(static_cast<char>(ch)));
                }

                // Test the characters that require three code units
                for (int ch = 0xE0; ch < 0xF0; ++ch)
                {
                    Assert::AreEqual(3u, utf_traits<char>::code_point_size(static_cast<char>(ch)));
                }

                // Test the characters that require four code units
                for (int ch = 0xF0; ch < 0xF7; ++ch)
                {
                    Assert::AreEqual(4u, utf_traits<char>::code_point_size(static_cast<char>(ch)));
                }

                // Test the char32_t overloads at the "boundaries"
                Assert::AreEqual(1u, utf_traits<char>::code_point_size(U'\u0000'));
                Assert::AreEqual(1u, utf_traits<char>::code_point_size(U'\u007F'));

                Assert::AreEqual(2u, utf_traits<char>::code_point_size(U'\u0080'));
                Assert::AreEqual(2u, utf_traits<char>::code_point_size(U'\u07FF'));

                Assert::AreEqual(3u, utf_traits<char>::code_point_size(U'\u0800'));
                Assert::AreEqual(3u, utf_traits<char>::code_point_size(U'\uFFFF'));

                Assert::AreEqual(4u, utf_traits<char>::code_point_size(U'\U00010000'));
                Assert::AreEqual(4u, utf_traits<char>::code_point_size(U'\U0010FFFF'));
            }

            TEST_METHOD(Utf16CodePointSizeTest)
            {
                // Test the characters that require only one code unit
                for (int ch = 0; ch < 0xD800; ++ch)
                {
                    Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(static_cast<char16_t>(ch)));
                }

                for (int ch = 0xE000; ch < 0xFFFF; ++ch)
                {
                    Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(static_cast<char16_t>(ch)));
                }

                // Test the characters that require one code unit
                for (int ch = 0xD800; ch < 0xDC00; ++ch)
                {
                    Assert::AreEqual(2u, utf_traits<char16_t>::code_point_size(static_cast<char16_t>(ch)));
                }

                // Test the char32_t overloads at the "boundaries"
                Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(U'\u0000'));
                Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(U'\uD7FF'));

                Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(U'\uE000'));
                Assert::AreEqual(1u, utf_traits<char16_t>::code_point_size(U'\uFFFF'));

                Assert::AreEqual(2u, utf_traits<char16_t>::code_point_size(U'\U00010000'));
                Assert::AreEqual(2u, utf_traits<char16_t>::code_point_size(U'\U0010FFFF'));
            }

            TEST_METHOD(Utf32CodePointSizeTest)
            {
                // Test the ASCII characters
                for (char32_t ch = 0; ch < 128; ++ch)
                {
                    Assert::AreEqual(1u, utf_traits<char32_t>::code_point_size(ch));
                }

                // Always returns 1, so there's no real point in wasting our time testing...
            }

#pragma endregion



#pragma region is_initial_code_unit Tests

            TEST_METHOD(Utf8IsInitialCodeUnitTest)
            {
                // True tests
                for (int ch = 0x00; ch < 0x80; ++ch)
                {
                    Assert::IsTrue(utf_traits<char>::is_initial_code_unit(static_cast<char>(ch)));
                }

                for (int ch = 0xC0; ch < 0xE0; ++ch)
                {
                    Assert::IsTrue(utf_traits<char>::is_initial_code_unit(static_cast<char>(ch)));
                }

                for (int ch = 0xE0; ch < 0xF0; ++ch)
                {
                    Assert::IsTrue(utf_traits<char>::is_initial_code_unit(static_cast<char>(ch)));
                }

                for (int ch = 0xF0; ch < 0xF7; ++ch)
                {
                    Assert::IsTrue(utf_traits<char>::is_initial_code_unit(static_cast<char>(ch)));
                }

                // False tests
                for (int ch = 0x80; ch < 0xC0; ++ch)
                {
                    Assert::IsFalse(utf_traits<char>::is_initial_code_unit(static_cast<char>(ch)));
                }
            }

            TEST_METHOD(Utf16IsInitialCodeUnitTest)
            {
                // True tests
                for (int ch = 0; ch < 0xD800; ++ch)
                {
                    Assert::IsTrue(utf_traits<char16_t>::is_initial_code_unit(static_cast<char16_t>(ch)));
                }

                for (int ch = 0xE000; ch < 0xFFFF; ++ch)
                {
                    Assert::IsTrue(utf_traits<char16_t>::is_initial_code_unit(static_cast<char16_t>(ch)));
                }

                for (int ch = 0xD800; ch < 0xDC00; ++ch)
                {
                    Assert::IsTrue(utf_traits<char16_t>::is_initial_code_unit(static_cast<char16_t>(ch)));
                }

                // False tests
                for (int ch = 0xDC00; ch < 0xE000; ++ch)
                {
                    Assert::IsFalse(utf_traits<char16_t>::is_initial_code_unit(static_cast<char16_t>(ch)));
                }
            }

            TEST_METHOD(Utf32IsInitialCodeUnitTest)
            {
                // Test the ASCII characters
                for (char32_t ch = 0; ch < 128; ++ch)
                {
                    Assert::IsTrue(utf_traits<char32_t>::is_initial_code_unit(ch));
                }

                // Always returns true, so there's no real point in wasting our time testing...
            }

#pragma endregion



#pragma region read Tests

            template <typename CharTy>
            void UtfReadTest(const CharTy* str, char32_t expected)
            {
                auto result = utf_traits<CharTy>::read(str);
                Assert::IsTrue(expected == result.first);
                Assert::IsTrue((str + utf_traits<CharTy>::code_point_size(*str)) == result.second);
            }

            TEST_METHOD(Utf8ReadTest)
            {
                // The most interesting tests are the ones at the "boundaries"
                UtfReadTest(u8"\u0000", U'\u0000');
                UtfReadTest(u8"\u007F", U'\u007F');

                UtfReadTest(u8"\u0080", U'\u0080');
                UtfReadTest(u8"\u07FF", U'\u07FF');

                UtfReadTest(u8"\u0800", U'\u0800');
                UtfReadTest(u8"\uFFFF", U'\uFFFF');

                UtfReadTest(u8"\U00010000", U'\U00010000');
                UtfReadTest(u8"\U0010FFFF", U'\U0010FFFF');
            }

            TEST_METHOD(Utf16ReadTest)
            {
                // The most interesting tests are the ones at the "boundaries"
                UtfReadTest(u"\u0000", U'\u0000');
                UtfReadTest(u"\uD7FF", U'\uD7FF');

                UtfReadTest(u"\uE000", U'\uE000');
                UtfReadTest(u"\uFFFF", U'\uFFFF');

                UtfReadTest(u"\U00010000", U'\U00010000');
                UtfReadTest(u"\U0010FFFF", U'\U0010FFFF');
            }

            TEST_METHOD(Utf32ReadTest)
            {
                // Always just returns *str, so testing isn't very useful... just do a few anyway
                UtfReadTest(U"\u0000", U'\u0000');
                UtfReadTest(U"\u007F", U'\u007F');
                UtfReadTest(U"\u0080", U'\u0080');
                UtfReadTest(U"\u07FF", U'\u07FF');
                UtfReadTest(U"\u0800", U'\u0800');
                UtfReadTest(U"\uD7FF", U'\uD7FF');
                UtfReadTest(U"\uE000", U'\uE000');
                UtfReadTest(U"\uFFFF", U'\uFFFF');
                UtfReadTest(U"\U00010000", U'\U00010000');
                UtfReadTest(U"\U0010FFFF", U'\U0010FFFF');
            }

#pragma endregion
        };
    }
}
