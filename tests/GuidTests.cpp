/*
 * Duncan Horn
 *
 * GuidTests.cpp
 *
 * Tests for the guid.h functions
 */
#include "stdafx.h"

#include <dhorn/windows/guid.h>
#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std::literals;

using std::size;

namespace dhorn::tests
{
    TEST_CLASS(GuidTests)
    {
        TEST_METHOD(GuidCompareTest)
        {
            constexpr GUID guids[] =
            {
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x00FF,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0xFF00,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x00FF, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0xFF00, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x000000FF, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x0000FF00, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00FF0000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0xFF000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0xFFFFFFFF, 0xFFFF, 0xFFFF,{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
            };

            for (size_t i = 0; i < size(guids); ++i)
            {
                Assert::IsTrue(windows::guid_compare(guids[i], guids[i]) == 0);

                for (size_t j = i + 1; j < size(guids); ++j)
                {
                    Assert::IsTrue(windows::guid_compare(guids[i], guids[j]) < 0);
                    Assert::IsTrue(windows::guid_compare(guids[j], guids[i]) > 0);
                }
            }
        }

        TEST_METHOD(FastGuidCompareTest)
        {
            constexpr GUID guids[] =
            {
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x0000,{ 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0x00FF,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x0000, 0xFF00,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0x00FF, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00000000, 0xFF00, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x000000FF, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x0000FF00, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0x00FF0000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0xFF000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
                { 0xFFFFFFFF, 0xFFFF, 0xFFFF,{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } },
            };

            for (size_t i = 0; i < size(guids); ++i)
            {
                // Equal GUIDs should always compare equal
                Assert::IsTrue(windows::fast_guid_compare(guids[i], guids[i]) == 0);

                for (size_t j = i + 1; j < size(guids); ++j)
                {
                    // The comparison result shouldn't be predictable, but it should be consistent
                    auto result1 = windows::fast_guid_compare(guids[i], guids[j]);
                    auto result2 = windows::fast_guid_compare(guids[j], guids[i]);
                    Assert::AreNotEqual(0, result1);
                    Assert::AreNotEqual(0, result2);
                    Assert::AreNotEqual(result1 < 0, result2 < 0);
                }
            }

            // Transitivity should hold
            for (size_t i = 0; i < size(guids); ++i)
            {
                for (size_t j = 0; j < size(guids); ++j)
                {
                    if (i == j) continue;
                    auto result1 = windows::fast_guid_compare(guids[i], guids[j]);

                    for (size_t k = 0; k < size(guids); ++k)
                    {
                        if (k == i || k == j) continue;
                        auto result2 = windows::fast_guid_compare(guids[j], guids[k]);

                        if ((result1 < 0) == (result2 < 0))
                        {
                            auto result3 = windows::fast_guid_compare(guids[i], guids[k]);
                            Assert::AreEqual(result1 < 0, result3 < 0);
                        }
                    }
                }
            }
        }

        TEST_METHOD(GuidDefaultConstructorTest)
        {
            constexpr windows::guid guid;

            Assert::AreEqual(0ul, guid.Data1);
            Assert::AreEqual(static_cast<unsigned short>(0), guid.Data2);
            Assert::AreEqual(static_cast<unsigned short>(0), guid.Data3);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.Data4[7]);
        }

        TEST_METHOD(GuidConstructorTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::guid guid(value);

            Assert::AreEqual(0x8e23d663ul, guid.Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.Data4[7]);
        }

        TEST_METHOD(GuidAggregateInitializationTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

            Assert::AreEqual(0x8e23d663ul, guid.Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.Data4[7]);
        }

        TEST_METHOD(GuidAssignmentTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            windows::guid guid;
            guid = value;

            Assert::AreEqual(0x8e23d663ul, guid.Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.Data4[7]);
        }

        TEST_METHOD(GuidOperatorBoolTest)
        {
            windows::guid guid;
            Assert::IsFalse(static_cast<bool>(guid));

            guid.Data3 = 1;
            Assert::IsTrue(static_cast<bool>(guid));
        }

        TEST_METHOD(GuidToStringTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, guid.to_string());
        }

        TEST_METHOD(GuidOStreamTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

            std::stringstream stream;
            stream << guid;
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, stream.str());
        }
    };
}
