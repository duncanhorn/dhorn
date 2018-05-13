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

            for (std::size_t i = 0; i < std::size(guids); ++i)
            {
                Assert::IsTrue(windows::guid_compare(guids[i], guids[i]) == 0);

                for (std::size_t j = i + 1; j < std::size(guids); ++j)
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

            for (std::size_t i = 0; i < std::size(guids); ++i)
            {
                // Equal GUIDs should always compare equal
                Assert::IsTrue(windows::fast_guid_compare(guids[i], guids[i]) == 0);

                for (std::size_t j = i + 1; j < std::size(guids); ++j)
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
            for (std::size_t i = 0; i < std::size(guids); ++i)
            {
                for (std::size_t j = 0; j < std::size(guids); ++j)
                {
                    if (i == j) continue;
                    auto result1 = windows::fast_guid_compare(guids[i], guids[j]);

                    for (std::size_t k = 0; k < std::size(guids); ++k)
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

            Assert::AreEqual(0ul, guid.get().Data1);
            Assert::AreEqual(static_cast<unsigned short>(0), guid.get().Data2);
            Assert::AreEqual(static_cast<unsigned short>(0), guid.get().Data3);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0), guid.get().Data4[7]);
        }

        TEST_METHOD(GuidConstructorTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::guid guid(value);

            Assert::AreEqual(0x8e23d663ul, guid.get().Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.get().Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
        }

        TEST_METHOD(GuidAggregateInitializationTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

            Assert::AreEqual(0x8e23d663ul, guid.get().Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.get().Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
        }

        TEST_METHOD(GuidAssignmentTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            windows::guid guid;
            guid = value;

            Assert::AreEqual(0x8e23d663ul, guid.get().Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.get().Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
        }

        TEST_METHOD(GuidOperatorBoolTest)
        {
            Assert::IsFalse(static_cast<bool>(windows::null_guid));

            windows::guid guid;
            Assert::IsFalse(static_cast<bool>(guid));

            guid.get().Data3 = 1;
            Assert::IsTrue(static_cast<bool>(guid));
        }

        TEST_METHOD(GuidToStringTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, guid.to_string());
        }

        TEST_METHOD(GuidOStreamTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

            std::stringstream stream;
            stream << guid;
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, stream.str());
        }

        TEST_METHOD(RefGuidConstructorTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::ref_guid guid(value);

            Assert::AreEqual(0x8e23d663ul, guid.get().Data1);
            Assert::AreEqual(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
            Assert::AreEqual(static_cast<unsigned short>(0x4333), guid.get().Data3);
            Assert::AreEqual(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
            Assert::AreEqual(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
            Assert::AreEqual(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
            Assert::AreEqual(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
            Assert::AreEqual(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
            Assert::AreEqual(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
            Assert::AreEqual(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
            Assert::AreEqual(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
        }

        TEST_METHOD(RefGuidOperatorBoolTest)
        {
            windows::guid guid;
            windows::ref_guid refGuid = guid;
            Assert::IsFalse(static_cast<bool>(refGuid));

            guid.get().Data3 = 1;
            Assert::IsTrue(static_cast<bool>(refGuid));
        }

        TEST_METHOD(RefGuidToStringTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::ref_guid refGuid = guid;
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, refGuid.to_string());
        }

        TEST_METHOD(RefGuidOStreamTest)
        {
            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::ref_guid refGuid = guid;

            std::stringstream stream;
            stream << refGuid;
            Assert::AreEqual("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, stream.str());
        }

        TEST_METHOD(GuidComparisonTest)
        {
            constexpr GUID nullGuid = {};
            constexpr windows::ref_guid nullRefGuid = nullGuid;

            // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
            constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
            constexpr windows::guid guid = value;
            constexpr windows::ref_guid refGuid = guid;

            auto compare = [](const auto& a, const auto& b, int result)
            {
                Assert::AreEqual(result == 0, a == b);
                Assert::AreEqual(result == 0, b == a);

                Assert::AreEqual(result != 0, a != b);
                Assert::AreEqual(result != 0, b != a);

                Assert::AreEqual(result < 0, a < b);
                Assert::AreEqual(result > 0, b < a);

                Assert::AreEqual(result <= 0, a <= b);
                Assert::AreEqual(result >= 0, b <= a);

                Assert::AreEqual(result > 0, a > b);
                Assert::AreEqual(result < 0, b > a);

                Assert::AreEqual(result >= 0, a >= b);
                Assert::AreEqual(result <= 0, b >= a);
            };

            compare(windows::null_guid, windows::null_guid, 0);
            compare(nullRefGuid, nullRefGuid, 0);
            compare(guid, guid, 0);
            compare(refGuid, refGuid, 0);

            compare(nullGuid, windows::null_guid, 0);
            compare(nullGuid, nullRefGuid, 0);
            compare(windows::null_guid, nullRefGuid, 0);

            compare(value, guid, 0);
            compare(value, refGuid, 0);
            compare(guid, refGuid, 0);

            compare(nullGuid, guid, -1);
            compare(nullGuid, refGuid, -1);
            compare(windows::null_guid, value, -1);
            compare(windows::null_guid, guid, -1);
            compare(windows::null_guid, refGuid, -1);
            compare(nullRefGuid, value, -1);
            compare(nullRefGuid, guid, -1);
            compare(nullRefGuid, refGuid, -1);
        }

        TEST_METHOD(GuidHashTest)
        {
            constexpr windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

            // This is more of a "does it compile" test since there's no good way to test how good the hash is
            std::hash<windows::guid> hasher;
            Assert::AreNotEqual(hasher(windows::null_guid), hasher(guid));
        }
    };
}
