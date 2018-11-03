/*
 * Duncan Horn
 *
 * UtfStringTests.cpp
 *
 * Tests for the dhorn utf_string classes, structures, functions, etc.
 */
#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4389) // Signed unsigned mismatch due to char16_t/char32_t being unsigned, but char is signed

#include <dhorn/type_traits.h>
#include <dhorn/experimental/utf_string.h>

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
            using traits_type = dhorn::experimental::details::utf8_traits;

            TEST_METHOD(SizeTest)
            {
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*u8"\u007F"));
                ASSERT_EQ(static_cast<std::size_t>(2), traits_type::size(*u8"\u07FF"));
                ASSERT_EQ(static_cast<std::size_t>(3), traits_type::size(*u8"\uFFFF"));
                ASSERT_EQ(static_cast<std::size_t>(4), traits_type::size(*u8"\U0010FFFF"));
            }

            TEST_METHOD(NextTest)
            {
                std::pair<const char *, char32_t> vals[] =
                {
                    { u8"\u007F", 0x007F },
                    { u8"\u07FF", 0x07FF },
                    { u8"\uFFFF", 0x0000FFFF },
                    { u8"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    ASSERT_TRUE(traits_type::next(pair.first, nullptr) == pair.second);
                }

                unsigned char invalid[] = { 0xF8 };
                try
                {
                    traits_type::next(reinterpret_cast<char *>(invalid), nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0xF8u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                std::pair<const char *, char32_t> vals[] =
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

                    ASSERT_TRUE(std::equal(buffer, buffer + 4, pair.first));
                }

                try
                {
                    char buffer[4];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0x1FFFFFu);
                }
            }
        };



        /*
         * utf16_traits Tests
         */
        TEST_CLASS(Utf16TraitsTests)
        {
            using traits_type = dhorn::experimental::details::utf16_traits;

            TEST_METHOD(SizeTest)
            {
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*u"\uD7FF"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*u"\uE000"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*u"\uFFFF"));
                ASSERT_EQ(static_cast<std::size_t>(2), traits_type::size(*u"\U00010000"));
                ASSERT_EQ(static_cast<std::size_t>(2), traits_type::size(*u"\U0010FFFF"));
            }

            TEST_METHOD(NextTest)
            {
                std::pair<const char16_t *, char32_t> vals[] =
                {
                    { u"\uD7FF", 0x0000D7FF },
                    { u"\uE000", 0x0000E000 },
                    { u"\uFFFF", 0x0000FFFF },
                    { u"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    ASSERT_TRUE(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char16_t invalid[] = { 0xDC00 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0xDC00u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                std::pair<const char16_t *, char32_t> vals[] =
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

                    ASSERT_TRUE(std::equal(buffer, buffer + 2, pair.first));
                }

                try
                {
                    char16_t buffer[2];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0x1FFFFFu);
                }
            }
        };



        /*
         * utf32_traits Tests
         */
        TEST_CLASS(Utf32TraitsTests)
        {
            using traits_type = dhorn::experimental::details::utf32_traits;

            TEST_METHOD(SizeTest)
            {
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*U"\uD7FF"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*U"\uE000"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*U"\uFFFF"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*U"\U00010000"));
                ASSERT_EQ(static_cast<std::size_t>(1), traits_type::size(*U"\U0010FFFF"));
            }

            TEST_METHOD(NextTest)
            {
                std::pair<const char32_t *, char32_t> vals[] =
                {
                    { U"\uD7FF", 0x0000D7FF },
                    { U"\uE000", 0x0000E000 },
                    { U"\uFFFF", 0x0000FFFF },
                    { U"\U0010FFFF", 0x0010FFFF }
                };

                for (auto &pair : vals)
                {
                    ASSERT_TRUE(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char32_t invalid[] = { 0x110000 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0x110000u);
                }
            }

            TEST_METHOD(WriteTest)
            {
                std::pair<const char32_t *, char32_t> vals[] =
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

                    ASSERT_TRUE(std::equal(buffer, buffer + 1, pair.first));
                }

                try
                {
                    char32_t buffer[1];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    ASSERT_TRUE(e.bad_value() == 0x1FFFFFu);
                }
            }
        };


        static const char32_t test_array[] =
            { 't', 'e', 's', 't', ' ', '-', ' ', u'\u1FE7', u'\u09EA', U'\U0010FE2B', u'\u0080' };
        static const char test_string_utf8[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
        static const char16_t test_string_utf16[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
        static const char32_t test_string_utf32[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";


        /*
         * utf8_string Tests
         */
        TEST_CLASS(Utf8StringTests)
        {
            using string_type = dhorn::experimental::utf8_string;

            static constexpr auto &test_string = test_string_utf8;

            inline void EnsureCorrectString(const string_type &str)
            {
                ASSERT_TRUE(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(sizeof(buff1) - 1, str1.length());
                ASSERT_EQ(sizeof(buff1) - 1, str1.size());
                ASSERT_EQ(sizeof(buff1) - 1, strlen(str1.c_str()));
                ASSERT_EQ(0, strcmp(buff1, str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(sizeof(buff2) - 7, str2.length());
                ASSERT_EQ(sizeof(buff2) - 1, str2.size());
                ASSERT_EQ(sizeof(buff2) - 1, strlen(str2.c_str()));
                ASSERT_EQ(0, strcmp(buff2, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                char buff1_utf8[] = u8"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str1.length());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str1.size());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, strlen(str1.c_str()));
                ASSERT_EQ(0, strcmp(buff1_utf8, str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(sizeof(buff2_utf8) - 7, str2.length());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, str2.size());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, strlen(str2.c_str()));
                ASSERT_EQ(0, strcmp(buff2_utf8, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                char buff1_utf8[] = u8"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str1.length());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str1.size());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, strlen(str1.c_str()));
                ASSERT_EQ(0, strcmp(buff1_utf8, str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(sizeof(buff2_utf8) - 7, str2.length());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, str2.size());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, strlen(str2.c_str()));
                ASSERT_EQ(0, strcmp(buff2_utf8, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                ASSERT_EQ(s1.length(), str1.length());
                ASSERT_EQ(s1.length(), str1.size());
                ASSERT_EQ(s1.length(), strlen(str1.c_str()));
                ASSERT_EQ(0, strcmp(s1.c_str(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                ASSERT_EQ(s2.length() - 6, str2.length());
                ASSERT_EQ(s2.length(), str2.size());
                ASSERT_EQ(s2.length(), strlen(str2.c_str()));
                ASSERT_EQ(0, strcmp(s2.c_str(), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                ASSERT_TRUE(s == str.c_str());
                ASSERT_EQ(s.length(), str.length());
                ASSERT_EQ(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                ASSERT_EQ(std::size(test_array), s1.length());
                ASSERT_EQ(str.size(), s1.size());
                ASSERT_TRUE(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                ASSERT_EQ(static_cast<std::size_t>(0), str1.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str1.size());
                ASSERT_EQ(std::size(test_string) - 9, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                string_type str = str16;
                ASSERT_EQ(str16.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf8) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                ASSERT_EQ(str32.length(), str2.length());
                ASSERT_EQ(std::size(buffer_utf8) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                string_type str1 = test_string;
                str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                ASSERT_EQ(std::size(test_string) - 9, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                str = str16;
                ASSERT_EQ(str16.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf8) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                str = str32;
                ASSERT_EQ(str32.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf8) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buff1[] = u8"This is a test";
                str = buff1;
                ASSERT_EQ(sizeof(buff1) - 1, str.length());
                ASSERT_EQ(sizeof(buff1) - 1, str.size());
                ASSERT_EQ(sizeof(buff1) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff1, str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(sizeof(buff2) - 7, str.length());
                ASSERT_EQ(sizeof(buff2) - 1, str.size());
                ASSERT_EQ(sizeof(buff2) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff2, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char16_t buff1[] = u"This is a test";
                char buff1_utf8[] = u8"This is a test";
                str = buff1;
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str.length());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str.size());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff1_utf8, str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(sizeof(buff2_utf8) - 7, str.length());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, str.size());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff2_utf8, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char32_t buff1[] = U"This is a test";
                char buff1_utf8[] = u8"This is a test";
                str = buff1;
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str.length());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, str.size());
                ASSERT_EQ(sizeof(buff1_utf8) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff1_utf8, str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(sizeof(buff2_utf8) - 7, str.length());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, str.size());
                ASSERT_EQ(sizeof(buff2_utf8) - 1, strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(buff2_utf8, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = u8"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                ASSERT_EQ(s1.length(), str.length());
                ASSERT_EQ(s1.length(), str.size());
                ASSERT_EQ(s1.length(), strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(s1.c_str(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                ASSERT_EQ(s2.length() - 6, str.length());
                ASSERT_EQ(s2.length(), str.size());
                ASSERT_EQ(s2.length(), strlen(str.c_str()));
                ASSERT_EQ(0, strcmp(s2.c_str(), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                ASSERT_EQ(len, str.length());
                ASSERT_EQ(strSize, str.size());
                EnsureCorrectString(str);

                char expected[] = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                ASSERT_EQ(static_cast<std::size_t>(4), str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Append Tests

            TEST_METHOD(PushBackTest)
            {
                string_type str;

                for (auto val : test_array)
                {
                    str.push_back(val);
                }

                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                ASSERT_EQ(std::size(test_array), str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto size = str.size();

                for (std::size_t i = 0; i < 20; ++i)
                {
                    str += str;
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }
            }

            TEST_METHOD(AppendUtf8StringLiteralTest)
            {
                string_type str = u8"test - ";
                const char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u8"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u8"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u8"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                for (std::size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                ASSERT_EQ(len + 4, str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = u8"test - ";
                std::basic_string<char> other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u8"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 9, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Iterator Tests

            TEST_METHOD(ForwardIteratorIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr--);
                }
            }

            TEST_METHOD(ForwardIteratorEqualityTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                auto end = str.end();
                std::size_t rangeSize = 0;
                while (itr != end)
                {
                    ASSERT_FALSE(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                ASSERT_TRUE(itr == end);
                ASSERT_EQ(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == str);
                ASSERT_FALSE(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf8);
                ASSERT_TRUE(test_string_utf8 == str);
                ASSERT_FALSE(str != test_string_utf8);
                ASSERT_FALSE(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf16);
                ASSERT_TRUE(test_string_utf16 == str);
                ASSERT_FALSE(str != test_string_utf16);
                ASSERT_FALSE(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf32);
                ASSERT_TRUE(test_string_utf32 == str);
                ASSERT_FALSE(str != test_string_utf32);
                ASSERT_FALSE(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                ASSERT_FALSE(str < str);
                ASSERT_FALSE(str > str);
                ASSERT_TRUE(str <= str);
                ASSERT_TRUE(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

#pragma endregion
        };



        /*
         * utf16_string Tests
         */
        TEST_CLASS(Utf16StringTests)
        {
            using string_type = dhorn::experimental::utf16_string;

            static constexpr auto &test_string = test_string_utf16;

            inline void EnsureCorrectString(const string_type &str)
            {
                ASSERT_TRUE(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1_utf16) - 1, str1.length());
                ASSERT_EQ(std::size(buff1_utf16) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2_utf16) - 2, str2.length());
                ASSERT_EQ(std::size(buff2_utf16) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1) - 1, str1.length());
                ASSERT_EQ(std::size(buff1) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2) - 2, str2.length());
                ASSERT_EQ(std::size(buff2) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2), std::begin(buff2), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1_utf16) - 1, str1.length());
                ASSERT_EQ(std::size(buff1_utf16) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2_utf16) - 2, str2.length());
                ASSERT_EQ(std::size(buff2_utf16) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf16), std::begin(buff2_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                ASSERT_EQ(s1.length(), str1.length());
                ASSERT_EQ(s1.length(), str1.size());
                ASSERT_TRUE(std::equal(s1.begin(), s1.end(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                ASSERT_EQ(s2.length() - 6, str2.length());
                ASSERT_EQ(s2.length() - 5, str2.size());
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                ASSERT_TRUE(std::equal(s.begin(), s.end(), str.c_str()));
                ASSERT_EQ(s.length(), str.length());
                ASSERT_EQ(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                ASSERT_EQ(std::size(test_array), s1.length());
                ASSERT_EQ(str.size(), s1.size());
                ASSERT_TRUE(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                ASSERT_EQ(static_cast<std::size_t>(0), str1.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str1.size());
                ASSERT_EQ(std::size(test_string) - 2, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                ASSERT_EQ(str8.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                ASSERT_EQ(str32.length(), str2.length());
                ASSERT_EQ(std::size(buffer_utf16) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimization

                string_type str1 = test_string;
                str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimization

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                ASSERT_EQ(std::size(test_string) - 2, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                str = str8;
                ASSERT_EQ(str8.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                str = str32;
                ASSERT_EQ(str32.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1_utf16) - 1, str.length());
                ASSERT_EQ(std::size(buff1_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2_utf16) - 2, str.length());
                ASSERT_EQ(std::size(buff2_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char16_t buff1[] = u"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1) - 1, str.length());
                ASSERT_EQ(std::size(buff1) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2) - 2, str.length());
                ASSERT_EQ(std::size(buff2) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1_utf16) - 1, str.length());
                ASSERT_EQ(std::size(buff1_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2_utf16) - 2, str.length());
                ASSERT_EQ(std::size(buff2_utf16) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf16), std::begin(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = u"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                ASSERT_EQ(s1.length(), str.length());
                ASSERT_EQ(s1.length(), str.size());
                ASSERT_TRUE(std::equal(s1.begin(), s1.end(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                ASSERT_EQ(s2.length() - 6, str.length());
                ASSERT_EQ(s2.length() - 5, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                ASSERT_EQ(len, str.length());
                ASSERT_EQ(strSize, str.size());
                EnsureCorrectString(str);

                char16_t expected[] = u"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                ASSERT_EQ(static_cast<std::size_t>(4), str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Append Tests

            TEST_METHOD(PushBackTest)
            {
                string_type str;

                for (auto val : test_array)
                {
                    str.push_back(val);
                }

                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                ASSERT_EQ(std::size(test_array), str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto size = str.size();

                for (std::size_t i = 0; i < 20; ++i)
                {
                    str += str;
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }
            }

            TEST_METHOD(AppendUtf8StringLiteralTest)
            {
                string_type str = u"test - ";
                const char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                for (std::size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                ASSERT_EQ(len + 4, str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = u"test - ";
                std::basic_string<char16_t> other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 2, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Iterator Tests

            TEST_METHOD(ForwardIteratorIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr--);
                }
            }

            TEST_METHOD(ForwardIteratorEqualityTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                auto end = str.end();
                std::size_t rangeSize = 0;
                while (itr != end)
                {
                    ASSERT_FALSE(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                ASSERT_TRUE(itr == end);
                ASSERT_EQ(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == str);
                ASSERT_FALSE(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf8);
                ASSERT_TRUE(test_string_utf8 == str);
                ASSERT_FALSE(str != test_string_utf8);
                ASSERT_FALSE(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf16);
                ASSERT_TRUE(test_string_utf16 == str);
                ASSERT_FALSE(str != test_string_utf16);
                ASSERT_FALSE(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf32);
                ASSERT_TRUE(test_string_utf32 == str);
                ASSERT_FALSE(str != test_string_utf32);
                ASSERT_FALSE(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                ASSERT_FALSE(str < str);
                ASSERT_FALSE(str > str);
                ASSERT_TRUE(str <= str);
                ASSERT_TRUE(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

#pragma endregion
        };



        /*
         * utf32_string Tests
         */
        TEST_CLASS(Utf32StringTests)
        {
            using string_type = dhorn::experimental::utf32_string;

            static constexpr auto &test_string = test_string_utf32;

            inline void EnsureCorrectString(const string_type &str)
            {
                ASSERT_TRUE(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                ASSERT_EQ(static_cast<std::size_t>(0), str.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1_utf32) - 1, str1.length());
                ASSERT_EQ(std::size(buff1_utf32) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2_utf32) - 1, str2.length());
                ASSERT_EQ(std::size(buff2_utf32) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1_utf32) - 1, str1.length());
                ASSERT_EQ(std::size(buff1_utf32) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2_utf32) - 1, str2.length());
                ASSERT_EQ(std::size(buff2_utf32) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                string_type str1 = buff1;
                ASSERT_EQ(std::size(buff1) - 1, str1.length());
                ASSERT_EQ(std::size(buff1) - 1, str1.size());
                ASSERT_TRUE(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                ASSERT_EQ(std::size(buff2) - 1, str2.length());
                ASSERT_EQ(std::size(buff2) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buff2), std::begin(buff2), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                ASSERT_EQ(s1.length(), str1.length());
                ASSERT_EQ(s1.length(), str1.size());
                ASSERT_TRUE(std::equal(s1.begin(), s1.end(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                ASSERT_EQ(s2.length() - 6, str2.length());
                ASSERT_EQ(s2.length() - 6, str2.size());
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                ASSERT_TRUE(std::equal(s.begin(), s.end(), str.c_str()));
                ASSERT_EQ(s.length(), str.length());
                ASSERT_EQ(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                ASSERT_EQ(std::size(test_array), s1.length());
                ASSERT_EQ(str.size(), s1.size());
                ASSERT_TRUE(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                ASSERT_EQ(static_cast<std::size_t>(0), str1.length());
                ASSERT_EQ(static_cast<std::size_t>(0), str1.size());
                ASSERT_EQ(std::size(test_string) - 1, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                ASSERT_EQ(str8.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                string_type str2 = str16;
                ASSERT_EQ(str16.length(), str2.length());
                ASSERT_EQ(std::size(buffer_utf32) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                string_type str1 = test_string;
                str2 = str1;

                ASSERT_EQ(str1.length(), str2.length());
                ASSERT_EQ(str1.size(), str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                ASSERT_EQ(std::size(test_string) - 1, str2.length());
                ASSERT_EQ(std::size(test_string) - 1, str2.size());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                str = str8;
                ASSERT_EQ(str8.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                str = str16;
                ASSERT_EQ(str16.length(), str.length());
                ASSERT_EQ(std::size(buffer_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1_utf32) - 1, str.length());
                ASSERT_EQ(std::size(buff1_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2_utf32) - 1, str.length());
                ASSERT_EQ(std::size(buff2_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1_utf32) - 1, str.length());
                ASSERT_EQ(std::size(buff1_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2_utf32) - 1, str.length());
                ASSERT_EQ(std::size(buff2_utf32) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char32_t buff1[] = U"This is a test";
                str = buff1;
                ASSERT_EQ(std::size(buff1) - 1, str.length());
                ASSERT_EQ(std::size(buff1) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                ASSERT_EQ(std::size(buff2) - 1, str.length());
                ASSERT_EQ(std::size(buff2) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = U"foo";
                ASSERT_EQ(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                ASSERT_EQ(s1.length(), str.length());
                ASSERT_EQ(s1.length(), str.size());
                ASSERT_TRUE(std::equal(s1.begin(), s1.end(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                ASSERT_EQ(s2.length() - 6, str.length());
                ASSERT_EQ(s2.length() - 6, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                ASSERT_EQ(len, str.length());
                ASSERT_EQ(strSize, str.size());
                EnsureCorrectString(str);

                char32_t expected[] = U"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                ASSERT_EQ(static_cast<std::size_t>(4), str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Append Tests

            TEST_METHOD(PushBackTest)
            {
                string_type str;

                for (auto val : test_array)
                {
                    str.push_back(val);
                }

                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                ASSERT_EQ(std::size(test_array), str.length());
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto size = str.size();

                for (std::size_t i = 0; i < 20; ++i)
                {
                    str += str;
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }
            }

            TEST_METHOD(AppendUtf8StringLiteralTest)
            {
                string_type str = U"test - ";
                const char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = U"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = U"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = U"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                for (std::size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    ASSERT_EQ(len * 2, str.length());
                    ASSERT_EQ(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                ASSERT_EQ(len + 4, str.length());
                ASSERT_EQ(std::size(expected) - 1, str.size());
                ASSERT_TRUE(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = U"test - ";
                std::basic_string<char32_t> other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = U"test - ";
                str = other + str;
                ASSERT_EQ(std::size(test_string) - 1, str.size());
                ASSERT_EQ(std::size(test_string) - 1, str.length());
                ASSERT_TRUE(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

#pragma endregion

#pragma region Iterator Tests

            TEST_METHOD(ForwardIteratorIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    ASSERT_TRUE(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr--);
                }
            }

            TEST_METHOD(ForwardIteratorEqualityTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                auto end = str.end();
                std::size_t rangeSize = 0;
                while (itr != end)
                {
                    ASSERT_FALSE(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                ASSERT_TRUE(itr == end);
                ASSERT_EQ(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    ASSERT_TRUE(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == str);
                ASSERT_FALSE(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf8);
                ASSERT_TRUE(test_string_utf8 == str);
                ASSERT_FALSE(str != test_string_utf8);
                ASSERT_FALSE(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf16);
                ASSERT_TRUE(test_string_utf16 == str);
                ASSERT_FALSE(str != test_string_utf16);
                ASSERT_FALSE(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                ASSERT_TRUE(str == same);
                ASSERT_TRUE(same == str);
                ASSERT_FALSE(str != same);
                ASSERT_FALSE(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                ASSERT_TRUE(str == test_string_utf32);
                ASSERT_TRUE(test_string_utf32 == str);
                ASSERT_FALSE(str != test_string_utf32);
                ASSERT_FALSE(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                ASSERT_FALSE(str == sub);
                ASSERT_FALSE(sub == str);
                ASSERT_TRUE(str != sub);
                ASSERT_TRUE(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                ASSERT_FALSE(str < str);
                ASSERT_FALSE(str > str);
                ASSERT_TRUE(str <= str);
                ASSERT_TRUE(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    ASSERT_FALSE(str < equal);
                    ASSERT_FALSE(equal < str);
                    ASSERT_FALSE(str > equal);
                    ASSERT_FALSE(equal > str);
                    ASSERT_TRUE(str <= equal);
                    ASSERT_TRUE(equal <= str);
                    ASSERT_TRUE(str >= equal);
                    ASSERT_TRUE(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    ASSERT_FALSE(str < equalUtfStr);
                    ASSERT_FALSE(equalUtfStr < str);
                    ASSERT_FALSE(str > equalUtfStr);
                    ASSERT_FALSE(equalUtfStr > str);
                    ASSERT_TRUE(str <= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr <= str);
                    ASSERT_TRUE(str >= equalUtfStr);
                    ASSERT_TRUE(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    ASSERT_FALSE(str < equalStr);
                    ASSERT_FALSE(equalStr < str);
                    ASSERT_FALSE(str > equalStr);
                    ASSERT_FALSE(equalStr > str);
                    ASSERT_TRUE(str <= equalStr);
                    ASSERT_TRUE(equalStr <= str);
                    ASSERT_TRUE(str >= equalStr);
                    ASSERT_TRUE(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    ASSERT_FALSE(str < sub);
                    ASSERT_TRUE(sub < str);
                    ASSERT_TRUE(str > sub);
                    ASSERT_FALSE(sub > str);
                    ASSERT_FALSE(str <= sub);
                    ASSERT_TRUE(sub <= str);
                    ASSERT_TRUE(str >= sub);
                    ASSERT_FALSE(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    ASSERT_FALSE(str < subUtfStr);
                    ASSERT_TRUE(subUtfStr < str);
                    ASSERT_TRUE(str > subUtfStr);
                    ASSERT_FALSE(subUtfStr > str);
                    ASSERT_FALSE(str <= subUtfStr);
                    ASSERT_TRUE(subUtfStr <= str);
                    ASSERT_TRUE(str >= subUtfStr);
                    ASSERT_FALSE(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    ASSERT_FALSE(str < subStr);
                    ASSERT_TRUE(subStr < str);
                    ASSERT_TRUE(str > subStr);
                    ASSERT_FALSE(subStr > str);
                    ASSERT_FALSE(str <= subStr);
                    ASSERT_TRUE(subStr <= str);
                    ASSERT_TRUE(str >= subStr);
                    ASSERT_FALSE(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    ASSERT_TRUE(str < super);
                    ASSERT_FALSE(super < str);
                    ASSERT_FALSE(str > super);
                    ASSERT_TRUE(super > str);
                    ASSERT_TRUE(str <= super);
                    ASSERT_FALSE(super <= str);
                    ASSERT_FALSE(str >= super);
                    ASSERT_TRUE(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    ASSERT_TRUE(str < superUtfStr);
                    ASSERT_FALSE(superUtfStr < str);
                    ASSERT_FALSE(str > superUtfStr);
                    ASSERT_TRUE(superUtfStr > str);
                    ASSERT_TRUE(str <= superUtfStr);
                    ASSERT_FALSE(superUtfStr <= str);
                    ASSERT_FALSE(str >= superUtfStr);
                    ASSERT_TRUE(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    ASSERT_TRUE(str < superStr);
                    ASSERT_FALSE(superStr < str);
                    ASSERT_FALSE(str > superStr);
                    ASSERT_TRUE(superStr > str);
                    ASSERT_TRUE(str <= superStr);
                    ASSERT_FALSE(superStr <= str);
                    ASSERT_FALSE(str >= superStr);
                    ASSERT_TRUE(superStr >= str);
                }
            }

#pragma endregion
        };



        /*
         * Common utf_string Tests
         */
        TEST_CLASS(CommonUtfStringTests)
        {
            TEST_METHOD(EmptyTest)
            {
                dhorn::experimental::utf8_string str;
                ASSERT_TRUE(str.empty());

                str = u8"foo";
                ASSERT_FALSE(str.empty());

                dhorn::experimental::utf16_string str2 = u"bar";
                ASSERT_FALSE(str2.empty());

                str2 = u"";
                ASSERT_TRUE(str2.empty());
            }

            TEST_METHOD(ReserveAndCapacityTest)
            {
                dhorn::experimental::utf16_string str;
                str.reserve(10);
                ASSERT_TRUE(str.capacity() >= 10);

                char16_t buff[] = u"This is a string much greater than 10 characters";
                str = buff;
                ASSERT_TRUE(str.capacity() >= (std::size(buff) - 1));

                str.reserve(std::size(buff) + 10);
                ASSERT_TRUE(str.capacity() >= (std::size(buff) + 10));
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::experimental::utf8_string str1 = "foo";
                dhorn::experimental::utf8_string str2 = "bar";
                str1.swap(str2);

                ASSERT_EQ(0, strcmp(str1.c_str(), "bar"));
                ASSERT_EQ(0, strcmp(str2.c_str(), "foo"));
            }
        };
    }
}

#pragma warning(pop)
