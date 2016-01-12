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
