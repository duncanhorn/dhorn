/*
 * Duncan Horn
 *
 * UtfTests.cpp
 *
 * Tests for the utf.h functions and types
 */
#include "stdafx.h"

#include <algorithm>
#include <dhorn/utf.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(UtfTraitsTests)
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
                using Traits = utf_traits<CharTy>;
                auto result = Traits::read(str);
                Assert::IsTrue(expected == result.first);
                Assert::IsTrue((str + Traits::code_point_size(*str)) == result.second);
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



#pragma region write Tests

            template <typename CharTy>
            void UtfWriteTest(const CharTy* expected, char32_t ch)
            {
                using Traits = utf_traits<CharTy>;
                CharTy result[Traits::max_code_point_size];
                auto ptr = Traits::write(result, ch);

                size_t len = ptr - result;
                Assert::AreEqual(Traits::code_point_size(ch), len);

                for (size_t i = 0; i < len; ++i)
                {
                    Assert::IsTrue(expected[i] == result[i]);
                }
            }

            TEST_METHOD(Utf8WriteTest)
            {
                // The most interesting tests are the ones at the "boundaries"
                UtfWriteTest(u8"\u0000", U'\u0000');
                UtfWriteTest(u8"\u007F", U'\u007F');

                UtfWriteTest(u8"\u0080", U'\u0080');
                UtfWriteTest(u8"\u07FF", U'\u07FF');

                UtfWriteTest(u8"\u0800", U'\u0800');
                UtfWriteTest(u8"\uFFFF", U'\uFFFF');

                UtfWriteTest(u8"\U00010000", U'\U00010000');
                UtfWriteTest(u8"\U0010FFFF", U'\U0010FFFF');
            }

            TEST_METHOD(Utf16WriteTest)
            {
                // The most interesting tests are the ones at the "boundaries"
                UtfWriteTest(u"\u0000", U'\u0000');
                UtfWriteTest(u"\uD7FF", U'\uD7FF');

                UtfWriteTest(u"\uE000", U'\uE000');
                UtfWriteTest(u"\uFFFF", U'\uFFFF');

                UtfWriteTest(u"\U00010000", U'\U00010000');
                UtfWriteTest(u"\U0010FFFF", U'\U0010FFFF');
            }

            TEST_METHOD(Utf32WriteTest)
            {
                // Always just returns *str, so testing isn't very useful... just do a few anyway
                UtfWriteTest(U"\u0000", U'\u0000');
                UtfWriteTest(U"\u007F", U'\u007F');
                UtfWriteTest(U"\u0080", U'\u0080');
                UtfWriteTest(U"\u07FF", U'\u07FF');
                UtfWriteTest(U"\u0800", U'\u0800');
                UtfWriteTest(U"\uD7FF", U'\uD7FF');
                UtfWriteTest(U"\uE000", U'\uE000');
                UtfWriteTest(U"\uFFFF", U'\uFFFF');
                UtfWriteTest(U"\U00010000", U'\U00010000');
                UtfWriteTest(U"\U0010FFFF", U'\U0010FFFF');
            }

#pragma endregion

        };



        TEST_CLASS(UtfIteratorTests)
        {
#pragma region Forward Iteration Tests

            static constexpr char test8[] = u8"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";
            static constexpr char16_t test16[] = u"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";
            static constexpr char32_t test32[] = U"\u007F\u0080\u07FF\u0800\uD7FF\uE000\uFFFF\U00010000\U0010FFFF";

            TEST_METHOD(Utf8ForwardIterationTest)
            {
                utf8_iterator begin{ std::begin(test8) };
                utf8_iterator end{ std::end(test8) - 1 };

                size_t index = 0;
                std::for_each(begin, end, [&](char32_t value)
                {
                    Assert::IsTrue(test32[index++] == value);
                });
            }

            TEST_METHOD(Utf16ForwardIterationTest)
            {
                utf16_iterator begin{ std::begin(test16) };
                utf16_iterator end{ std::end(test16) - 1 };

                size_t index = 0;
                std::for_each(begin, end, [&](char32_t value)
                {
                    Assert::IsTrue(test32[index++] == value);
                });
            }

            TEST_METHOD(Utf32ForwardIterationTest)
            {
                utf32_iterator begin{ std::begin(test32) };
                utf32_iterator end{ std::end(test32) - 1 };

                size_t index = 0;
                std::for_each(begin, end, [&](char32_t value)
                {
                    Assert::IsTrue(test32[index++] == value);
                });
            }

#pragma endregion



#pragma region Reverse Iteration Tests

            TEST_METHOD(Utf8ReverseIterationTest)
            {
                utf8_iterator begin{ std::begin(test8) };
                utf8_iterator end{ std::end(test8) - 1 };

                size_t index = std::size(test32) - 1;
                do
                {
                    Assert::IsTrue(test32[index--] == *end--);
                }
                while (begin != end);
            }

            TEST_METHOD(Utf16ReverseIterationTest)
            {
                utf16_iterator begin{ std::begin(test16) };
                utf16_iterator end{ std::end(test16) - 1 };

                size_t index = std::size(test32) - 1;
                do
                {
                    Assert::IsTrue(test32[index--] == *end--);
                }
                while (begin != end);
            }

            TEST_METHOD(Utf32ReverseIterationTest)
            {
                utf32_iterator begin{ std::begin(test32) };
                utf32_iterator end{ std::end(test32) - 1 };

                size_t index = std::size(test32) - 1;
                do
                {
                    Assert::IsTrue(test32[index--] == *end--);
                }
                while (begin != end);
            }

#pragma endregion



#pragma region Simple Increment/Decrement Tests

            // Test strings
            static constexpr char str8[] = u8"abc";
            static constexpr char16_t str16[] = u"abc";
            static constexpr char32_t str32[] = U"abc";

            TEST_METHOD(PreIncrementTest)
            {
                utf8_iterator i8(str8);
                Assert::IsTrue(U'b' == *++i8);
                Assert::IsTrue(U'c' == *++i8);
                Assert::IsTrue(U'\0' == *++i8);

                utf16_iterator i16(str16);
                Assert::IsTrue(U'b' == *++i16);
                Assert::IsTrue(U'c' == *++i16);
                Assert::IsTrue(U'\0' == *++i16);

                utf32_iterator i32(str32);
                Assert::IsTrue(U'b' == *++i32);
                Assert::IsTrue(U'c' == *++i32);
                Assert::IsTrue(U'\0' == *++i32);
            }

            TEST_METHOD(PostIncrementTest)
            {
                utf8_iterator i8(str8);
                Assert::IsTrue(U'a' == *i8++);
                Assert::IsTrue(U'b' == *i8++);
                Assert::IsTrue(U'c' == *i8++);
                Assert::IsTrue(U'\0' == *i8);

                utf16_iterator i16(str16);
                Assert::IsTrue(U'a' == *i16++);
                Assert::IsTrue(U'b' == *i16++);
                Assert::IsTrue(U'c' == *i16++);
                Assert::IsTrue(U'\0' == *i16);

                utf32_iterator i32(str32);
                Assert::IsTrue(U'a' == *i32++);
                Assert::IsTrue(U'b' == *i32++);
                Assert::IsTrue(U'c' == *i32++);
                Assert::IsTrue(U'\0' == *i32);
            }

            TEST_METHOD(PreDecrementTest)
            {
                utf8_iterator i8(str8 + 3);
                Assert::IsTrue(U'c' == *--i8);
                Assert::IsTrue(U'b' == *--i8);
                Assert::IsTrue(U'a' == *--i8);

                utf16_iterator i16(str16 + 3);
                Assert::IsTrue(U'c' == *--i16);
                Assert::IsTrue(U'b' == *--i16);
                Assert::IsTrue(U'a' == *--i16);

                utf32_iterator i32(str32 + 3);
                Assert::IsTrue(U'c' == *--i32);
                Assert::IsTrue(U'b' == *--i32);
                Assert::IsTrue(U'a' == *--i32);
            }

            TEST_METHOD(PostDecrementTest)
            {
                utf8_iterator i8(str8 + 3);
                Assert::IsTrue(U'\0' == *i8--);
                Assert::IsTrue(U'c' == *i8--);
                Assert::IsTrue(U'b' == *i8--);

                utf16_iterator i16(str16 + 3);
                Assert::IsTrue(U'\0' == *i16--);
                Assert::IsTrue(U'c' == *i16--);
                Assert::IsTrue(U'b' == *i16--);

                utf32_iterator i32(str32 + 3);
                Assert::IsTrue(U'\0' == *i32--);
                Assert::IsTrue(U'c' == *i32--);
                Assert::IsTrue(U'b' == *i32--);
            }

#pragma endregion
        };
    }
}
