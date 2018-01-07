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
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*u8"\u007F"));
                Assert::AreEqual(static_cast<std::size_t>(2), traits_type::size(*u8"\u07FF"));
                Assert::AreEqual(static_cast<std::size_t>(3), traits_type::size(*u8"\uFFFF"));
                Assert::AreEqual(static_cast<std::size_t>(4), traits_type::size(*u8"\U0010FFFF"));
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
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
                }

                unsigned char invalid[] = { 0xF8 };
                try
                {
                    traits_type::next(reinterpret_cast<char *>(invalid), nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0xF8u);
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

                    Assert::IsTrue(std::equal(buffer, buffer + 4, pair.first));
                }

                try
                {
                    char buffer[4];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
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
            using traits_type = dhorn::experimental::details::utf16_traits;

            TEST_METHOD(SizeTest)
            {
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*u"\uD7FF"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*u"\uE000"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*u"\uFFFF"));
                Assert::AreEqual(static_cast<std::size_t>(2), traits_type::size(*u"\U00010000"));
                Assert::AreEqual(static_cast<std::size_t>(2), traits_type::size(*u"\U0010FFFF"));
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
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char16_t invalid[] = { 0xDC00 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0xDC00u);
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

                    Assert::IsTrue(std::equal(buffer, buffer + 2, pair.first));
                }

                try
                {
                    char16_t buffer[2];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
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
            using traits_type = dhorn::experimental::details::utf32_traits;

            TEST_METHOD(SizeTest)
            {
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*U"\uD7FF"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*U"\uE000"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*U"\uFFFF"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*U"\U00010000"));
                Assert::AreEqual(static_cast<std::size_t>(1), traits_type::size(*U"\U0010FFFF"));
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
                    Assert::IsTrue(traits_type::next(pair.first, nullptr) == pair.second);
                }

                char32_t invalid[] = { 0x110000 };
                try
                {
                    traits_type::next(invalid, nullptr);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0x110000u);
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

                    Assert::IsTrue(std::equal(buffer, buffer + 1, pair.first));
                }

                try
                {
                    char32_t buffer[1];
                    traits_type::write(0x1FFFFFu, buffer);
                    Assert::Fail(L"Expected an exception");
                }
                catch (dhorn::experimental::bad_utf_encoding &e)
                {
                    Assert::IsTrue(e.bad_value() == 0x1FFFFFu);
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
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(static_cast<std::size_t>(0), str.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(sizeof(buff1) - 1, str1.length());
                Assert::AreEqual(sizeof(buff1) - 1, str1.size());
                Assert::AreEqual(sizeof(buff1) - 1, strlen(str1.c_str()));
                Assert::AreEqual(0, strcmp(buff1, str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(sizeof(buff2) - 7, str2.length());
                Assert::AreEqual(sizeof(buff2) - 1, str2.size());
                Assert::AreEqual(sizeof(buff2) - 1, strlen(str2.c_str()));
                Assert::AreEqual(0, strcmp(buff2, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                char buff1_utf8[] = u8"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str1.length());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str1.size());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, strlen(str1.c_str()));
                Assert::AreEqual(0, strcmp(buff1_utf8, str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(sizeof(buff2_utf8) - 7, str2.length());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, str2.size());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, strlen(str2.c_str()));
                Assert::AreEqual(0, strcmp(buff2_utf8, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                char buff1_utf8[] = u8"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str1.length());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str1.size());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, strlen(str1.c_str()));
                Assert::AreEqual(0, strcmp(buff1_utf8, str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(sizeof(buff2_utf8) - 7, str2.length());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, str2.size());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, strlen(str2.c_str()));
                Assert::AreEqual(0, strcmp(buff2_utf8, str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                Assert::AreEqual(s1.length(), str1.length());
                Assert::AreEqual(s1.length(), str1.size());
                Assert::AreEqual(s1.length(), strlen(str1.c_str()));
                Assert::AreEqual(0, strcmp(s1.c_str(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                Assert::AreEqual(s2.length() - 6, str2.length());
                Assert::AreEqual(s2.length(), str2.size());
                Assert::AreEqual(s2.length(), strlen(str2.c_str()));
                Assert::AreEqual(0, strcmp(s2.c_str(), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                Assert::IsTrue(s == str.c_str());
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                Assert::AreEqual(std::size(test_array), s1.length());
                Assert::AreEqual(str.size(), s1.size());
                Assert::IsTrue(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                Assert::AreEqual(static_cast<std::size_t>(0), str1.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str1.size());
                Assert::AreEqual(std::size(test_string) - 9, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                string_type str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                Assert::AreEqual(str32.length(), str2.length());
                Assert::AreEqual(std::size(buffer_utf8) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                string_type str1 = test_string;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(std::size(test_string) - 9, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                str = str32;
                Assert::AreEqual(str32.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buff1[] = u8"This is a test";
                str = buff1;
                Assert::AreEqual(sizeof(buff1) - 1, str.length());
                Assert::AreEqual(sizeof(buff1) - 1, str.size());
                Assert::AreEqual(sizeof(buff1) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff1, str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(sizeof(buff2) - 7, str.length());
                Assert::AreEqual(sizeof(buff2) - 1, str.size());
                Assert::AreEqual(sizeof(buff2) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff2, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char16_t buff1[] = u"This is a test";
                char buff1_utf8[] = u8"This is a test";
                str = buff1;
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str.length());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str.size());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff1_utf8, str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(sizeof(buff2_utf8) - 7, str.length());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, str.size());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff2_utf8, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char32_t buff1[] = U"This is a test";
                char buff1_utf8[] = u8"This is a test";
                str = buff1;
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str.length());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, str.size());
                Assert::AreEqual(sizeof(buff1_utf8) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff1_utf8, str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char buff2_utf8[] = u8"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(sizeof(buff2_utf8) - 7, str.length());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, str.size());
                Assert::AreEqual(sizeof(buff2_utf8) - 1, strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(buff2_utf8, str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                Assert::AreEqual(s1.length(), str.length());
                Assert::AreEqual(s1.length(), str.size());
                Assert::AreEqual(s1.length(), strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(s1.c_str(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                Assert::AreEqual(s2.length() - 6, str.length());
                Assert::AreEqual(s2.length(), str.size());
                Assert::AreEqual(s2.length(), strlen(str.c_str()));
                Assert::AreEqual(0, strcmp(s2.c_str(), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(strSize, str.size());
                EnsureCorrectString(str);

                char expected[] = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(static_cast<std::size_t>(4), str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
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

                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                Assert::AreEqual(std::size(test_array), str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u8"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
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
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u8"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u8"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u8"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = u8"test - ";
                std::basic_string<char> other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u8"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u8"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::IsTrue(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    Assert::IsTrue(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr--);
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
                    Assert::IsFalse(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                Assert::IsTrue(itr == end);
                Assert::AreEqual(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == str);
                Assert::IsFalse(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf8);
                Assert::IsTrue(test_string_utf8 == str);
                Assert::IsFalse(str != test_string_utf8);
                Assert::IsFalse(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf16);
                Assert::IsTrue(test_string_utf16 == str);
                Assert::IsFalse(str != test_string_utf16);
                Assert::IsFalse(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf32);
                Assert::IsTrue(test_string_utf32 == str);
                Assert::IsFalse(str != test_string_utf32);
                Assert::IsFalse(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                Assert::IsFalse(str < str);
                Assert::IsFalse(str > str);
                Assert::IsTrue(str <= str);
                Assert::IsTrue(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = u8"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
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
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(static_cast<std::size_t>(0), str.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1_utf16) - 1, str1.length());
                Assert::AreEqual(std::size(buff1_utf16) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2_utf16) - 2, str2.length());
                Assert::AreEqual(std::size(buff2_utf16) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1) - 1, str1.length());
                Assert::AreEqual(std::size(buff1) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2) - 2, str2.length());
                Assert::AreEqual(std::size(buff2) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1_utf16) - 1, str1.length());
                Assert::AreEqual(std::size(buff1_utf16) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2_utf16) - 2, str2.length());
                Assert::AreEqual(std::size(buff2_utf16) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::begin(buff2_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                Assert::AreEqual(s1.length(), str1.length());
                Assert::AreEqual(s1.length(), str1.size());
                Assert::IsTrue(std::equal(s1.begin(), s1.end(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                Assert::AreEqual(s2.length() - 6, str2.length());
                Assert::AreEqual(s2.length() - 5, str2.size());
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                Assert::IsTrue(std::equal(s.begin(), s.end(), str.c_str()));
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                Assert::AreEqual(std::size(test_array), s1.length());
                Assert::AreEqual(str.size(), s1.size());
                Assert::IsTrue(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                Assert::AreEqual(static_cast<std::size_t>(0), str1.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str1.size());
                Assert::AreEqual(std::size(test_string) - 2, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                Assert::AreEqual(str32.length(), str2.length());
                Assert::AreEqual(std::size(buffer_utf16) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimization

                string_type str1 = test_string;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimization

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(std::size(test_string) - 2, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf32_string str32 = buffer_utf32;
                str = str32;
                Assert::AreEqual(str32.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1_utf16) - 1, str.length());
                Assert::AreEqual(std::size(buff1_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2_utf16) - 2, str.length());
                Assert::AreEqual(std::size(buff2_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char16_t buff1[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1) - 1, str.length());
                Assert::AreEqual(std::size(buff1) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2) - 2, str.length());
                Assert::AreEqual(std::size(buff2) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1_utf16) - 1, str.length());
                Assert::AreEqual(std::size(buff1_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2_utf16) - 2, str.length());
                Assert::AreEqual(std::size(buff2_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::begin(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimization

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                Assert::AreEqual(s1.length(), str.length());
                Assert::AreEqual(s1.length(), str.size());
                Assert::IsTrue(std::equal(s1.begin(), s1.end(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                Assert::AreEqual(s2.length() - 6, str.length());
                Assert::AreEqual(s2.length() - 5, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(strSize, str.size());
                EnsureCorrectString(str);

                char16_t expected[] = u"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(static_cast<std::size_t>(4), str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
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

                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                Assert::AreEqual(std::size(test_array), str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
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
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = u"test - ";
                std::basic_string<char16_t> other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = u"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = u"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::IsTrue(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    Assert::IsTrue(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr--);
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
                    Assert::IsFalse(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                Assert::IsTrue(itr == end);
                Assert::AreEqual(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == str);
                Assert::IsFalse(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf8);
                Assert::IsTrue(test_string_utf8 == str);
                Assert::IsFalse(str != test_string_utf8);
                Assert::IsFalse(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf16);
                Assert::IsTrue(test_string_utf16 == str);
                Assert::IsFalse(str != test_string_utf16);
                Assert::IsFalse(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf32);
                Assert::IsTrue(test_string_utf32 == str);
                Assert::IsFalse(str != test_string_utf32);
                Assert::IsFalse(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                Assert::IsFalse(str < str);
                Assert::IsFalse(str > str);
                Assert::IsTrue(str <= str);
                Assert::IsTrue(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = u"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
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
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

#pragma region Constructor Tests

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(static_cast<std::size_t>(0), str.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1_utf32) - 1, str1.length());
                Assert::AreEqual(std::size(buff1_utf32) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2_utf32) - 1, str2.length());
                Assert::AreEqual(std::size(buff2_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1_utf32) - 1, str1.length());
                Assert::AreEqual(std::size(buff1_utf32) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2_utf32) - 1, str2.length());
                Assert::AreEqual(std::size(buff2_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(std::size(buff1) - 1, str1.length());
                Assert::AreEqual(std::size(buff1) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(std::size(buff2) - 1, str2.length());
                Assert::AreEqual(std::size(buff2) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(StdStringConstructorTest)
            {
                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                string_type str1 = s1;
                Assert::AreEqual(s1.length(), str1.length());
                Assert::AreEqual(s1.length(), str1.size());
                Assert::IsTrue(std::equal(s1.begin(), s1.end(), str1.c_str()));
                EnsureCorrectString(str1);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = s2;
                Assert::AreEqual(s2.length() - 6, str2.length());
                Assert::AreEqual(s2.length() - 6, str2.size());
                EnsureCorrectString(str2);
            }

            TEST_METHOD(IteratorConstructorTest)
            {
                // We aren't testing the validity of the utf_string::iterator yet, so we have to rely on std::string
                // here
                std::string s = "This is a string";
                string_type str(s.begin(), s.end());
                Assert::IsTrue(std::equal(s.begin(), s.end(), str.c_str()));
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(UtfStringIteratorConstructorTest)
            {
                string_type str = test_string;
                string_type s1(str.begin(), str.end());
                Assert::AreEqual(std::size(test_array), s1.length());
                Assert::AreEqual(str.size(), s1.size());
                Assert::IsTrue(std::equal(std::begin(test_array), std::end(test_array), s1.begin()));
            }

            TEST_METHOD(CopyConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                string_type str1 = test_string;
                string_type str2(std::move(str1));

                Assert::AreEqual(static_cast<std::size_t>(0), str1.length());
                Assert::AreEqual(static_cast<std::size_t>(0), str1.size());
                Assert::AreEqual(std::size(test_string) - 1, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                string_type str2 = str16;
                Assert::AreEqual(str16.length(), str2.length());
                Assert::AreEqual(std::size(buffer_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

#pragma endregion

#pragma region Assignment Tests

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                string_type str1 = test_string;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                string_type str = test_string;

                str = str;
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str2.length()); // Prevent optimizations

                {
                    string_type str1 = test_string;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(std::size(test_string) - 1, str2.length());
                Assert::AreEqual(std::size(test_string) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::experimental::utf8_string str8 = buffer_utf8;
                str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::experimental::utf16_string str16 = buffer_utf16;
                str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(std::size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1_utf32) - 1, str.length());
                Assert::AreEqual(std::size(buff1_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2_utf32) - 1, str.length());
                Assert::AreEqual(std::size(buff2_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1_utf32) - 1, str.length());
                Assert::AreEqual(std::size(buff1_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2_utf32) - 1, str.length());
                Assert::AreEqual(std::size(buff2_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                char32_t buff1[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(std::size(buff1) - 1, str.length());
                Assert::AreEqual(std::size(buff1) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(std::size(buff2) - 1, str.length());
                Assert::AreEqual(std::size(buff2) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(static_cast<std::size_t>(3), str.length()); // Prevent optimizations

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                Assert::AreEqual(s1.length(), str.length());
                Assert::AreEqual(s1.length(), str.size());
                Assert::IsTrue(std::equal(s1.begin(), s1.end(), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                Assert::AreEqual(s2.length() - 6, str.length());
                Assert::AreEqual(s2.length() - 6, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = test_string;
                auto len = str.length();
                auto strSize = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(strSize, str.size());
                EnsureCorrectString(str);

                char32_t expected[] = U"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(static_cast<std::size_t>(4), str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
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

                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                Assert::AreEqual(std::size(test_array), str.length());
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = U"test - ";
                dhorn::experimental::utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
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
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = U"test - ";
                const char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = U"test - ";
                const char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = U"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(strSize * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    strSize = str.size();
                }

                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = test_string;
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(std::size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendStdStringTest)
            {
                string_type str = U"test - ";
                std::basic_string<char32_t> other = U"\u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = U"test - ";
                str = str + other;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
                EnsureCorrectString(str);

                str = other;
                other = U"test - ";
                str = other + str;
                Assert::AreEqual(std::size(test_string) - 1, str.size());
                Assert::AreEqual(std::size(test_string) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(test_string), std::end(test_string), str.c_str()));
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
                    Assert::IsTrue(test_array[i] == *itr);
                    ++itr;
                }
            }

            TEST_METHOD(ForwardIteratorPreIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 1; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *++itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostIncrementTest)
            {
                string_type str = test_string;

                auto itr = str.begin();
                for (std::size_t i = 0; i < std::size(test_array); ++i)
                {
                    Assert::IsTrue(test_array[i] == *itr++);
                }
            }

            TEST_METHOD(ForwardIteratorDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    --itr;
                    Assert::IsTrue(test_array[i - 1] == *itr);
                }
            }

            TEST_METHOD(ForwardIteratorPreDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *--itr);
                }
            }

            TEST_METHOD(ForwardIteratorPostDecrementTest)
            {
                string_type str = test_string;

                auto itr = str.end();
                --itr;
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr--);
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
                    Assert::IsFalse(itr == end);
                    ++rangeSize;
                    ++itr;
                }

                Assert::IsTrue(itr == end);
                Assert::AreEqual(std::size(test_array), rangeSize);
            }

            TEST_METHOD(ReverseIteratorTest)
            {
                string_type str = test_string;

                auto itr = str.rbegin();
                for (std::size_t i = std::size(test_array); i > 0; --i)
                {
                    Assert::IsTrue(test_array[i - 1] == *itr);
                    ++itr;
                }
            }

#pragma endregion

#pragma region Equality Tests

            TEST_METHOD(SelfEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == str);
                Assert::IsFalse(str != str);
            }

            TEST_METHOD(Utf8StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf8_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf8_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf8StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf8);
                Assert::IsTrue(test_string_utf8 == str);
                Assert::IsFalse(str != test_string_utf8);
                Assert::IsFalse(test_string_utf8 != str);

                auto sub = test_string_utf8 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(StdStringEqualityTest)
            {
                string_type str = test_string;

                std::string same = test_string_utf8;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                std::string sub(std::begin(test_string_utf8), std::end(test_string_utf8) - 3);
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf16_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf16_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf16StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf16);
                Assert::IsTrue(test_string_utf16 == str);
                Assert::IsFalse(str != test_string_utf16);
                Assert::IsFalse(test_string_utf16 != str);

                auto sub = test_string_utf16 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringEqualityTest)
            {
                string_type str = test_string;

                dhorn::experimental::utf32_string same = test_string;
                Assert::IsTrue(str == same);
                Assert::IsTrue(same == str);
                Assert::IsFalse(str != same);
                Assert::IsFalse(same != str);

                dhorn::experimental::utf32_string sub(str.begin(), --str.end());
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

            TEST_METHOD(Utf32StringLiteralEqualityTest)
            {
                string_type str = test_string;

                Assert::IsTrue(str == test_string_utf32);
                Assert::IsTrue(test_string_utf32 == str);
                Assert::IsFalse(str != test_string_utf32);
                Assert::IsFalse(test_string_utf32 != str);

                auto sub = test_string_utf32 + 1;
                Assert::IsFalse(str == sub);
                Assert::IsFalse(sub == str);
                Assert::IsTrue(str != sub);
                Assert::IsTrue(sub != str);
            }

#pragma endregion

#pragma region Comparison Tests

            TEST_METHOD(SelfComparisonTest)
            {
                string_type str = test_string;

                Assert::IsFalse(str < str);
                Assert::IsFalse(str > str);
                Assert::IsTrue(str <= str);
                Assert::IsTrue(str >= str);
            }

            TEST_METHOD(Utf8StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u8"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u8"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u8"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf8_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf16StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = u"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = u"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = u"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf16_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char16_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
                }
            }

            TEST_METHOD(Utf32StringComparisonTest)
            {
                string_type str = U"test_1";

                // equal string comparison
                {
                    // Raw pointer comparison
                    auto equal = U"test_1";
                    Assert::IsFalse(str < equal);
                    Assert::IsFalse(equal < str);
                    Assert::IsFalse(str > equal);
                    Assert::IsFalse(equal > str);
                    Assert::IsTrue(str <= equal);
                    Assert::IsTrue(equal <= str);
                    Assert::IsTrue(str >= equal);
                    Assert::IsTrue(equal >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string equalUtfStr = equal;
                    Assert::IsFalse(str < equalUtfStr);
                    Assert::IsFalse(equalUtfStr < str);
                    Assert::IsFalse(str > equalUtfStr);
                    Assert::IsFalse(equalUtfStr > str);
                    Assert::IsTrue(str <= equalUtfStr);
                    Assert::IsTrue(equalUtfStr <= str);
                    Assert::IsTrue(str >= equalUtfStr);
                    Assert::IsTrue(equalUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> equalStr = equal;
                    Assert::IsFalse(str < equalStr);
                    Assert::IsFalse(equalStr < str);
                    Assert::IsFalse(str > equalStr);
                    Assert::IsFalse(equalStr > str);
                    Assert::IsTrue(str <= equalStr);
                    Assert::IsTrue(equalStr <= str);
                    Assert::IsTrue(str >= equalStr);
                    Assert::IsTrue(equalStr >= str);
                }

                // substring comparison
                {
                    // Raw pointer comparison
                    auto sub = U"test_";
                    Assert::IsFalse(str < sub);
                    Assert::IsTrue(sub < str);
                    Assert::IsTrue(str > sub);
                    Assert::IsFalse(sub > str);
                    Assert::IsFalse(str <= sub);
                    Assert::IsTrue(sub <= str);
                    Assert::IsTrue(str >= sub);
                    Assert::IsFalse(sub >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string subUtfStr = sub;
                    Assert::IsFalse(str < subUtfStr);
                    Assert::IsTrue(subUtfStr < str);
                    Assert::IsTrue(str > subUtfStr);
                    Assert::IsFalse(subUtfStr > str);
                    Assert::IsFalse(str <= subUtfStr);
                    Assert::IsTrue(subUtfStr <= str);
                    Assert::IsTrue(str >= subUtfStr);
                    Assert::IsFalse(subUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> subStr = sub;
                    Assert::IsFalse(str < subStr);
                    Assert::IsTrue(subStr < str);
                    Assert::IsTrue(str > subStr);
                    Assert::IsFalse(subStr > str);
                    Assert::IsFalse(str <= subStr);
                    Assert::IsTrue(subStr <= str);
                    Assert::IsTrue(str >= subStr);
                    Assert::IsFalse(subStr >= str);
                }

                // superstring comparison
                {
                    // Raw pointer comparison
                    auto super = U"test_12";
                    Assert::IsTrue(str < super);
                    Assert::IsFalse(super < str);
                    Assert::IsFalse(str > super);
                    Assert::IsTrue(super > str);
                    Assert::IsTrue(str <= super);
                    Assert::IsFalse(super <= str);
                    Assert::IsFalse(str >= super);
                    Assert::IsTrue(super >= str);

                    // utf8_string comparison
                    dhorn::experimental::utf32_string superUtfStr = super;
                    Assert::IsTrue(str < superUtfStr);
                    Assert::IsFalse(superUtfStr < str);
                    Assert::IsFalse(str > superUtfStr);
                    Assert::IsTrue(superUtfStr > str);
                    Assert::IsTrue(str <= superUtfStr);
                    Assert::IsFalse(superUtfStr <= str);
                    Assert::IsFalse(str >= superUtfStr);
                    Assert::IsTrue(superUtfStr >= str);

                    // basic_string comparison
                    std::basic_string<char32_t> superStr = super;
                    Assert::IsTrue(str < superStr);
                    Assert::IsFalse(superStr < str);
                    Assert::IsFalse(str > superStr);
                    Assert::IsTrue(superStr > str);
                    Assert::IsTrue(str <= superStr);
                    Assert::IsFalse(superStr <= str);
                    Assert::IsFalse(str >= superStr);
                    Assert::IsTrue(superStr >= str);
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
                Assert::IsTrue(str.empty());

                str = u8"foo";
                Assert::IsFalse(str.empty());

                dhorn::experimental::utf16_string str2 = u"bar";
                Assert::IsFalse(str2.empty());

                str2 = u"";
                Assert::IsTrue(str2.empty());
            }

            TEST_METHOD(ReserveAndCapacityTest)
            {
                dhorn::experimental::utf16_string str;
                str.reserve(10);
                Assert::IsTrue(str.capacity() >= 10);

                char16_t buff[] = u"This is a string much greater than 10 characters";
                str = buff;
                Assert::IsTrue(str.capacity() >= (std::size(buff) - 1));

                str.reserve(std::size(buff) + 10);
                Assert::IsTrue(str.capacity() >= (std::size(buff) + 10));
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::experimental::utf8_string str1 = "foo";
                dhorn::experimental::utf8_string str2 = "bar";
                str1.swap(str2);

                Assert::AreEqual(0, strcmp(str1.c_str(), "bar"));
                Assert::AreEqual(0, strcmp(str2.c_str(), "foo"));
            }
        };
    }
}

#pragma warning(pop)
