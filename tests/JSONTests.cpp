/*
 * Duncan Horn
 *
 * JSONTests.cpp
 *
 * Tests for json.h
 */

#include "stdafx.h"

#include <dhorn/json.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(JsonTests)
        {
#pragma region json_cast Tests

            TEST_METHOD(Int32JsonCastTest)
            {
                utf8_string str = u8"-42";

                auto value = std::make_shared<json_number>(str);
                auto num = json_cast<int32_t>(value.get());
                Assert::AreEqual(-42, num);
            }

            TEST_METHOD(UInt32JsonCastTest)
            {
                utf8_string str = u8"42";

                auto value = std::make_shared<json_number>(str);
                auto num = json_cast<uint32_t>(value.get());
                Assert::AreEqual(42u, num);
            }

            TEST_METHOD(FloatJsonCastTest)
            {
                utf8_string str = u8"42.123";

                auto value = std::make_shared<json_number>(str);
                auto num = json_cast<float>(value.get());
                Assert::AreEqual(42.123f, num);
            }

            TEST_METHOD(DoubleJsonCastTest)
            {
                utf8_string str = u8"42.123";

                auto value = std::make_shared<json_number>(str);
                auto num = json_cast<double>(value.get());
                Assert::AreEqual(42.123, num);
            }

            TEST_METHOD(Utf8StdStringJsonCastTest)
            {
                utf8_string str = u8"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<std::string>(value.get());
                Assert::IsTrue(str == string.c_str());
            }

            TEST_METHOD(Utf16StdStringJsonCastTest)
            {
                utf16_string str = u"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<std::basic_string<char16_t>>(value.get());
                Assert::IsTrue(str == string.c_str());
            }

            TEST_METHOD(Utf32StdStringJsonCastTest)
            {
                utf32_string str = U"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<std::basic_string<char32_t>>(value.get());
                Assert::IsTrue(str == string.c_str());
            }

            TEST_METHOD(Utf8StringJsonCastTest)
            {
                utf8_string str = u8"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<utf8_string>(value.get());
                Assert::IsTrue(str == string);
            }

            TEST_METHOD(Utf16StringJsonCastTest)
            {
                utf8_string str = u8"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<utf16_string>(value.get());
                Assert::IsTrue(str == string);
            }

            TEST_METHOD(Utf32StringJsonCastTest)
            {
                utf8_string str = u8"Test String";

                auto value = std::make_shared<json_string>(str);
                auto string = json_cast<utf32_string>(value.get());
                Assert::IsTrue(str == string);
            }

            TEST_METHOD(StdVectorJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::vector<utf8_string>>(value.get());
                Assert::AreEqual(array.size(), arr.size());
                Assert::IsTrue(arr[0] == str1);
                Assert::IsTrue(arr[1] == str2);
                Assert::IsTrue(arr[2] == str3);
            }

            TEST_METHOD(StdListJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::list<utf8_string>>(value.get());
                Assert::AreEqual(array.size(), arr.size());

                auto itr = arr.begin();
                Assert::IsTrue(*itr == str1);
                ++itr;
                Assert::IsTrue(*itr == str2);
                ++itr;
                Assert::IsTrue(*itr == str3);
            }

            TEST_METHOD(StdForwardListJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::forward_list<utf8_string>>(value.get());

                auto itr = arr.begin();
                Assert::IsTrue(*itr == str1);
                ++itr;
                Assert::IsTrue(*itr == str2);
                ++itr;
                Assert::IsTrue(*itr == str3);

                ++itr;
                Assert::IsTrue(itr == arr.end());
            }

            TEST_METHOD(StdSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::set<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());

                Assert::IsTrue(arr.find(str1) != std::end(arr));
                Assert::IsTrue(arr.find(str2) != std::end(arr));
                Assert::IsTrue(arr.find(str3) != std::end(arr));

                // set does not allow duplicates
                array.push_back(std::make_shared<json_string>(str1));
                value = std::make_shared<json_array>(array);
                arr = json_cast<std::set<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());
            }

            TEST_METHOD(StdMultiSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::multiset<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());

                Assert::IsTrue(arr.find(str1) != std::end(arr));
                Assert::IsTrue(arr.find(str2) != std::end(arr));
                Assert::IsTrue(arr.find(str3) != std::end(arr));

                // multiset allows duplicates
                array.push_back(std::make_shared<json_string>(str1));
                value = std::make_shared<json_array>(array);
                arr = json_cast<std::multiset<utf8_string>>(value.get());
                Assert::AreEqual(4u, arr.size());
            }

            TEST_METHOD(StdUnorderedSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::unordered_set<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());

                Assert::IsTrue(arr.find(str1) != std::end(arr));
                Assert::IsTrue(arr.find(str2) != std::end(arr));
                Assert::IsTrue(arr.find(str3) != std::end(arr));

                // unordered_set does not allow duplicates
                array.push_back(std::make_shared<json_string>(str1));
                value = std::make_shared<json_array>(array);
                arr = json_cast<std::unordered_set<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());
            }

            TEST_METHOD(StdUnorderedMultiSetJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::unordered_multiset<utf8_string>>(value.get());
                Assert::AreEqual(3u, arr.size());

                Assert::IsTrue(arr.find(str1) != std::end(arr));
                Assert::IsTrue(arr.find(str2) != std::end(arr));
                Assert::IsTrue(arr.find(str3) != std::end(arr));

                // unordered_multiset allows duplicates
                array.push_back(std::make_shared<json_string>(str1));
                value = std::make_shared<json_array>(array);
                arr = json_cast<std::unordered_multiset<utf8_string>>(value.get());
                Assert::AreEqual(4u, arr.size());
            }

            TEST_METHOD(StdArrayJsonCastTest)
            {
                auto str1 = u8"String 1";
                auto str2 = u8"String 2";
                auto str3 = u8"String 3";

                std::vector<std::shared_ptr<json_value>> array;
                array.push_back(std::make_shared<json_string>(str1));
                array.push_back(std::make_shared<json_string>(str2));
                array.push_back(std::make_shared<json_string>(str3));

                auto value = std::make_shared<json_array>(array);
                auto arr = json_cast<std::array<utf8_string, 3u>>(value.get());
                Assert::AreEqual(array.size(), arr.size());
                Assert::IsTrue(arr[0] == str1);
                Assert::IsTrue(arr[1] == str2);
                Assert::IsTrue(arr[2] == str3);

                try
                {
                    auto arr2 = json_cast<std::array<utf8_string, 4u>>(value.get());
                    Assert::Fail(L"Expected an exception");
                }
                catch (const json_exception &)
                {
                }
            }

            TEST_METHOD(StdMapFromArraysJsonCastTest)
            {
                using pair_array = std::vector<std::shared_ptr<json_value>>;
                std::vector<pair_array> values =
                {
                    {
                        std::make_shared<json_number>(u8"0"),
                        std::make_shared<json_string>(u8"String 1")
                    },
                    {
                        std::make_shared<json_number>(u8"1"),
                        std::make_shared<json_string>(u8"String 2")
                    },
                    {
                        std::make_shared<json_number>(u8"2"),
                        std::make_shared<json_string>(u8"String 3")
                    }
                };

                std::vector<std::shared_ptr<json_value>> array;
                array.emplace_back(std::make_shared<json_array>(values[0]));
                array.emplace_back(std::make_shared<json_array>(values[1]));
                array.emplace_back(std::make_shared<json_array>(values[2]));

                auto value = std::make_shared<json_array>(array);
                auto map = json_cast<std::map<int, utf8_string>>(value.get());

                Assert::AreEqual(array.size(), map.size());

                for (auto &pair : map)
                {
                    auto &jsonValue = values[pair.first][1];
                    auto jsonString = jsonValue->as<json_string>();
                    Assert::IsTrue(jsonString->str() == pair.second);
                }
            }

#pragma endregion
        };
    }
}
