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
        TEST_CLASS(JSONTests)
        {
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
        };
    }
}
