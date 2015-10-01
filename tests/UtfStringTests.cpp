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



        /*
         * utf8_string Tests
         */
        TEST_CLASS(Utf8StringTests)
        {
            using string_type = dhorn::utf8_string;

            inline void EnsureCorrectString(_In_ const string_type &str)
            {
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());
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
                string_type str(std::begin(s), std::end(s));
                Assert::IsTrue(s == str.c_str());
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                char buffer[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                char buffer[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2(std::move(str1));

                Assert::AreEqual(0u, str1.length());
                Assert::AreEqual(0u, str1.size());
                Assert::AreEqual(dhorn::array_size(buffer) - 9, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf16_string str16 = buffer_utf16;
                string_type str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                Assert::AreEqual(str32.length(), str2.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf8) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u8"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimizations

                char buffer[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                char buffer[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str = buffer;

                str = str;
                Assert::AreEqual(dhorn::array_size(buffer) - 9, str.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u8"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimizations

                char buffer[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                {
                    string_type str1 = buffer;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(dhorn::array_size(buffer) - 9, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf16_string str16 = buffer_utf16;
                str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf32_string str32 = buffer_utf32;
                str = str32;
                Assert::AreEqual(str32.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf8) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf8), std::end(buffer_utf8), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u8"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

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
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

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
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

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
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

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
                string_type str = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(size, str.size());
                EnsureCorrectString(str);

                char expected[] = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(4u, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(PushBackTest)
            {
                char buff[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t arr[] = { 't', 'e', 's', 't', ' ', '-', ' ', u'\u1FE7', u'\u09EA', U'\U0010FE2B', u'\u0080' };
                string_type str;

                for (auto val : arr)
                {
                    str.push_back(val);
                }

                Assert::IsTrue(std::equal(std::begin(buff), std::end(buff), str.c_str()));
                Assert::AreEqual(dhorn::array_size(arr), str.length());
                Assert::AreEqual(dhorn::array_size(buff) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u8"test - ";
                utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u8"test - ";
                utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u8"test - ";
                utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 20; ++i)
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
                char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u8"test - ";
                char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u8"test - ";
                char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 9, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }

                char expected[] = u8"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = u8"test - \u1FE7\u09EA\U0010FE2B\u0080";
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }
        };



        /*
         * utf16_string Tests
         */
        TEST_CLASS(Utf16StringTests)
        {
            using string_type = dhorn::utf16_string;

            inline void EnsureCorrectString(_In_ const string_type &str)
            {
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 2, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2) - 2, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 2, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 1, str2.size());
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
                Assert::IsTrue(std::equal(std::begin(s1), std::end(s1), str1.c_str()));
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
                string_type str(std::begin(s), std::end(s));
                Assert::IsTrue(std::equal(std::begin(s), std::end(s), str.c_str()));
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                char16_t buffer[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                char16_t buffer[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2(std::move(str1));

                Assert::AreEqual(0u, str1.length());
                Assert::AreEqual(0u, str1.size());
                Assert::AreEqual(dhorn::array_size(buffer) - 2, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf32_string str32 = buffer_utf32;
                string_type str2 = str32;
                Assert::AreEqual(str32.length(), str2.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf16) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = u"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimization

                char16_t buffer[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                char16_t buffer[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str = buffer;

                str = str;
                Assert::AreEqual(dhorn::array_size(buffer) - 2, str.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = u"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimization

                char16_t buffer[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                {
                    string_type str1 = buffer;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(dhorn::array_size(buffer) - 2, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimization

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf8_string str8 = buffer_utf8;
                str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf32_string str32 = buffer_utf32;
                str = str32;
                Assert::AreEqual(str32.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf16), std::end(buffer_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimization

                char buff1[] = u8"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::end(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 2, str.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::end(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimization

                char16_t buff1[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2) - 2, str.length());
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimization

                char32_t buff1[] = U"This is a test";
                char16_t buff1_utf16[] = u"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf16), std::begin(buff1_utf16), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char16_t buff2_utf16[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 2, str.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf16) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf16), std::begin(buff2_utf16), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = u"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimization

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                Assert::AreEqual(s1.length(), str.length());
                Assert::AreEqual(s1.length(), str.size());
                Assert::IsTrue(std::equal(std::begin(s1), std::end(s1), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                Assert::AreEqual(s2.length() - 6, str.length());
                Assert::AreEqual(s2.length() - 5, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(size, str.size());
                EnsureCorrectString(str);

                char16_t expected[] = u"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(4u, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(PushBackTest)
            {
                char16_t buff[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t arr[] = { 't', 'e', 's', 't', ' ', '-', ' ', u'\u1FE7', u'\u09EA', U'\U0010FE2B', u'\u0080' };
                string_type str;

                for (auto val : arr)
                {
                    str.push_back(val);
                }

                Assert::IsTrue(std::equal(std::begin(buff), std::end(buff), str.c_str()));
                Assert::AreEqual(dhorn::array_size(arr), str.length());
                Assert::AreEqual(dhorn::array_size(buff) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = u"test - ";
                utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = u"test - ";
                utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = u"test - ";
                utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 20; ++i)
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
                char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = u"test - ";
                char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = u"test - ";
                char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 2, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }

                char16_t expected[] = u"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = u"test - \u1FE7\u09EA\U0010FE2B\u0080";
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }
        };



        /*
        * utf32_string Tests
        */
        TEST_CLASS(Utf32StringTests)
        {
            using string_type = dhorn::utf32_string;

            inline void EnsureCorrectString(_In_ const string_type &str)
            {
                Assert::IsTrue(str.c_str()[str.size()] == '\0');
            }

            TEST_METHOD(DefaultConstructorTest)
            {
                string_type str;

                Assert::AreEqual(0u, str.length());
                Assert::AreEqual(0u, str.size());
            }

            TEST_METHOD(Utf8StringLiteralConstructorTest)
            {
                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf16StringLiteralConstructorTest)
            {
                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str1.c_str()));
                EnsureCorrectString(str1);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(Utf32StringLiteralConstructorTest)
            {
                char32_t buff1[] = U"This is a test";
                string_type str1 = buff1;
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str1.length());
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str1.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str1.c_str()));
                EnsureCorrectString(str1);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                string_type str2 = buff2;
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str2.length());
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str2.size());
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
                Assert::IsTrue(std::equal(std::begin(s1), std::end(s1), str1.c_str()));
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
                string_type str(std::begin(s), std::end(s));
                Assert::IsTrue(std::equal(std::begin(s), std::end(s), str.c_str()));
                Assert::AreEqual(s.length(), str.length());
                Assert::AreEqual(s.length(), str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                char32_t buffer[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(MoveConstructorTest)
            {
                char32_t buffer[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                string_type str2(std::move(str1));

                Assert::AreEqual(0u, str1.length());
                Assert::AreEqual(0u, str1.size());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertConstructorTest)
            {
                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf8_string str8 = buffer_utf8;
                string_type str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf16_string str16 = buffer_utf16;
                string_type str2 = str16;
                Assert::AreEqual(str16.length(), str2.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf32) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignmentTest)
            {
                string_type str2 = U"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimizations

                char32_t buffer[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str1 = buffer;
                str2 = str1;

                Assert::AreEqual(str1.length(), str2.length());
                Assert::AreEqual(str1.size(), str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str1.c_str()));
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str1);
                EnsureCorrectString(str2);
            }

            TEST_METHOD(CopyAssignSelfTest)
            {
                char32_t buffer[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                string_type str = buffer;

                str = str;
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(MoveAssignmentTest)
            {
                string_type str2 = U"foo";
                Assert::AreEqual(3u, str2.length()); // Prevent optimizations

                char32_t buffer[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                {
                    string_type str1 = buffer;
                    str2 = std::move(str1);
                }

                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.length());
                Assert::AreEqual(dhorn::array_size(buffer) - 1, str2.size());
                Assert::IsTrue(std::equal(std::begin(buffer), std::end(buffer), str2.c_str()));
                EnsureCorrectString(str2);
            }

            TEST_METHOD(ConvertAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                char buffer_utf8[] = u8"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char16_t buffer_utf16[] = u"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t buffer_utf32[] = U"Just a test - \u1FE7\u09EA\U0010FE2B\u0080";

                dhorn::utf8_string str8 = buffer_utf8;
                str = str8;
                Assert::AreEqual(str8.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);

                dhorn::utf16_string str16 = buffer_utf16;
                str = str16;
                Assert::AreEqual(str16.length(), str.length());
                Assert::AreEqual(dhorn::array_size(buffer_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buffer_utf32), std::end(buffer_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf8StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                char buff1[] = u8"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::end(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char buff2[] = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::end(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf16StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                char16_t buff1[] = u"This is a test";
                char32_t buff1_utf32[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1_utf32), std::begin(buff1_utf32), str.c_str()));
                EnsureCorrectString(str);

                char16_t buff2[] = u"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                char32_t buff2_utf32[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff2_utf32) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2_utf32), std::begin(buff2_utf32), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(Utf32StringLiteralAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                char32_t buff1[] = U"This is a test";
                str = buff1;
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff1) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff1), std::begin(buff1), str.c_str()));
                EnsureCorrectString(str);

                char32_t buff2[] = U"Gimme some utf-16 characters! \u1FE7\U0010C55A\u0080";
                str = buff2;
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str.length());
                Assert::AreEqual(dhorn::array_size(buff2) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(buff2), std::begin(buff2), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(StdStringAssignmentTest)
            {
                string_type str = U"foo";
                Assert::AreEqual(3u, str.length()); // Prevent optimizations

                // NOTE: std::wstring is purposefully not tested since it purposefully is not supported
                std::string s1 = u8"This is a test";
                str = s1;
                Assert::AreEqual(s1.length(), str.length());
                Assert::AreEqual(s1.length(), str.size());
                Assert::IsTrue(std::equal(std::begin(s1), std::end(s1), str.c_str()));
                EnsureCorrectString(str);

                std::string s2 = u8"Gimme some utf-8 characters! \u1FE7\U0010C55A\u0080";
                str = s2;
                Assert::AreEqual(s2.length() - 6, str.length());
                Assert::AreEqual(s2.length() - 6, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AssignSelfAsStringLiteralTest)
            {
                string_type str = U"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                str = str.c_str();
                Assert::AreEqual(len, str.length());
                Assert::AreEqual(size, str.size());
                EnsureCorrectString(str);

                char32_t expected[] = U"\u1FE7\u09EA\U0010FE2B\u0080";
                str = str.c_str() + 7;
                Assert::AreEqual(4u, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(PushBackTest)
            {
                char32_t buff[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";
                char32_t arr[] = { 't', 'e', 's', 't', ' ', '-', ' ', u'\u1FE7', u'\u09EA', U'\U0010FE2B', u'\u0080' };
                string_type str;

                for (auto val : arr)
                {
                    str.push_back(val);
                }

                Assert::IsTrue(std::equal(std::begin(buff), std::end(buff), str.c_str()));
                Assert::AreEqual(dhorn::array_size(arr), str.length());
                Assert::AreEqual(dhorn::array_size(buff) - 1, str.size());
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf8StringTest)
            {
                string_type str = U"test - ";
                utf8_string other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringTest)
            {
                string_type str = U"test - ";
                utf16_string other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringTest)
            {
                string_type str = U"test - ";
                utf32_string other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfTest)
            {
                string_type str = U"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 20; ++i)
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
                char *other = u8"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf16StringLiteralTest)
            {
                string_type str = U"test - ";
                char16_t *other = u"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendUtf32StringLiteralTest)
            {
                string_type str = U"test - ";
                char32_t *other = U"\u1FE7\u09EA\U0010FE2B\u0080";
                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080";

                str += other;
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.length());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }

            TEST_METHOD(AppendSelfAsStringLiteralTest)
            {
                string_type str = U"test - \u1FE7\u09EA\U0010FE2B\u0080";
                auto len = str.length();
                auto size = str.size();

                for (size_t i = 0; i < 10; ++i)
                {
                    str += str.c_str();
                    Assert::AreEqual(len * 2, str.length());
                    Assert::AreEqual(size * 2, str.size());
                    EnsureCorrectString(str);

                    len = str.length();
                    size = str.size();
                }

                char32_t expected[] = U"test - \u1FE7\u09EA\U0010FE2B\u0080\u1FE7\u09EA\U0010FE2B\u0080";
                str = U"test - \u1FE7\u09EA\U0010FE2B\u0080";
                len = str.length();

                str += str.c_str() + 7;
                Assert::AreEqual(len + 4, str.length());
                Assert::AreEqual(dhorn::array_size(expected) - 1, str.size());
                Assert::IsTrue(std::equal(std::begin(expected), std::end(expected), str.c_str()));
                EnsureCorrectString(str);
            }
        };



        /*
         * Common utf_string Tests
         */
        TEST_CLASS(CommonUtfStringTests)
        {
            TEST_METHOD(EmptyTest)
            {
                dhorn::utf8_string str;
                Assert::IsTrue(str.empty());

                str = u8"foo";
                Assert::IsFalse(str.empty());

                dhorn::utf16_string str2 = u"bar";
                Assert::IsFalse(str2.empty());

                str2 = u"";
                Assert::IsTrue(str2.empty());
            }

            TEST_METHOD(ReserveAndCapacityTest)
            {
                dhorn::utf16_string str;
                str.reserve(10);
                Assert::IsTrue(str.capacity() >= 10);

                char16_t buff[] = u"This is a string much greater than 10 characters";
                str = buff;
                Assert::IsTrue(str.capacity() >= (dhorn::array_size(buff) - 1));

                str.reserve(dhorn::array_size(buff) + 10);
                Assert::IsTrue(str.capacity() >= (dhorn::array_size(buff) + 10));
            }

            TEST_METHOD(SwapTest)
            {
                dhorn::utf8_string str1 = "foo";
                dhorn::utf8_string str2 = "bar";
                str1.swap(str2);

                Assert::AreEqual(0, strcmp(str1.c_str(), "bar"));
                Assert::AreEqual(0, strcmp(str2.c_str(), "foo"));
            }
        };
    }
}
