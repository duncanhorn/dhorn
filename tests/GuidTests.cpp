/*
 * Duncan Horn
 *
 * GuidTests.cpp
 *
 * Tests for the guid.h functions
 */

#include <dhorn/windows/guid.h>
#include <gtest/gtest.h>
#include <sstream>

using namespace std::literals;

TEST(GuidTests, GuidCompareTest)
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
        ASSERT_TRUE(dhorn::windows::guid_compare(guids[i], guids[i]) == 0);

        for (std::size_t j = i + 1; j < std::size(guids); ++j)
        {
            ASSERT_TRUE(dhorn::windows::guid_compare(guids[i], guids[j]) < 0);
            ASSERT_TRUE(dhorn::windows::guid_compare(guids[j], guids[i]) > 0);
        }
    }
}

TEST(GuidTests, FastGuidCompareTest)
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
        ASSERT_TRUE(dhorn::windows::fast_guid_compare(guids[i], guids[i]) == 0);

        for (std::size_t j = i + 1; j < std::size(guids); ++j)
        {
            // The comparison result shouldn't be predictable, but it should be consistent
            auto result1 = dhorn::windows::fast_guid_compare(guids[i], guids[j]);
            auto result2 = dhorn::windows::fast_guid_compare(guids[j], guids[i]);
            ASSERT_NE(0, result1);
            ASSERT_NE(0, result2);
            ASSERT_NE(result1 < 0, result2 < 0);
        }
    }

    // Transitivity should hold
    for (std::size_t i = 0; i < std::size(guids); ++i)
    {
        for (std::size_t j = 0; j < std::size(guids); ++j)
        {
            if (i == j) continue;
            auto result1 = dhorn::windows::fast_guid_compare(guids[i], guids[j]);

            for (std::size_t k = 0; k < std::size(guids); ++k)
            {
                if (k == i || k == j) continue;
                auto result2 = dhorn::windows::fast_guid_compare(guids[j], guids[k]);

                if ((result1 < 0) == (result2 < 0))
                {
                    auto result3 = dhorn::windows::fast_guid_compare(guids[i], guids[k]);
                    ASSERT_EQ(result1 < 0, result3 < 0);
                }
            }
        }
    }
}

TEST(GuidTests, GuidDefaultConstructorTest)
{
    constexpr dhorn::windows::guid guid;

    ASSERT_EQ(0ul, guid.get().Data1);
    ASSERT_EQ(static_cast<unsigned short>(0), guid.get().Data2);
    ASSERT_EQ(static_cast<unsigned short>(0), guid.get().Data3);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[0]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[1]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[2]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[3]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[4]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[5]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[6]);
    ASSERT_EQ(static_cast<unsigned char>(0), guid.get().Data4[7]);
}

TEST(GuidTests, GuidConstructorTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    constexpr dhorn::windows::guid guid(value);

    ASSERT_EQ(0x8e23d663ul, guid.get().Data1);
    ASSERT_EQ(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
    ASSERT_EQ(static_cast<unsigned short>(0x4333), guid.get().Data3);
    ASSERT_EQ(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
    ASSERT_EQ(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
    ASSERT_EQ(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
    ASSERT_EQ(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
    ASSERT_EQ(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
    ASSERT_EQ(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
    ASSERT_EQ(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
    ASSERT_EQ(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
}

TEST(GuidTests, GuidAggregateInitializationTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

    ASSERT_EQ(0x8e23d663ul, guid.get().Data1);
    ASSERT_EQ(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
    ASSERT_EQ(static_cast<unsigned short>(0x4333), guid.get().Data3);
    ASSERT_EQ(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
    ASSERT_EQ(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
    ASSERT_EQ(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
    ASSERT_EQ(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
    ASSERT_EQ(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
    ASSERT_EQ(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
    ASSERT_EQ(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
    ASSERT_EQ(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
}

TEST(GuidTests, GuidAssignmentTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    dhorn::windows::guid guid;
    guid = value;

    ASSERT_EQ(0x8e23d663ul, guid.get().Data1);
    ASSERT_EQ(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
    ASSERT_EQ(static_cast<unsigned short>(0x4333), guid.get().Data3);
    ASSERT_EQ(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
    ASSERT_EQ(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
    ASSERT_EQ(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
    ASSERT_EQ(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
    ASSERT_EQ(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
    ASSERT_EQ(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
    ASSERT_EQ(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
    ASSERT_EQ(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
}

TEST(GuidTests, GuidOperatorBoolTest)
{
    ASSERT_FALSE(static_cast<bool>(dhorn::windows::null_guid));

    dhorn::windows::guid guid;
    ASSERT_FALSE(static_cast<bool>(guid));

    guid.get().Data3 = 1;
    ASSERT_TRUE(static_cast<bool>(guid));
}

TEST(GuidTests, GuidToStringTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    ASSERT_EQ("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, guid.to_string());
}

TEST(GuidTests, GuidOStreamTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

    std::stringstream stream;
    stream << guid;
    ASSERT_EQ("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, stream.str());
}

TEST(GuidTests, RefGuidConstructorTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    dhorn::windows::ref_guid guid(value);

    ASSERT_EQ(0x8e23d663ul, guid.get().Data1);
    ASSERT_EQ(static_cast<unsigned short>(0x8cd5), guid.get().Data2);
    ASSERT_EQ(static_cast<unsigned short>(0x4333), guid.get().Data3);
    ASSERT_EQ(static_cast<unsigned char>(0xb8), guid.get().Data4[0]);
    ASSERT_EQ(static_cast<unsigned char>(0x27), guid.get().Data4[1]);
    ASSERT_EQ(static_cast<unsigned char>(0x12), guid.get().Data4[2]);
    ASSERT_EQ(static_cast<unsigned char>(0x1a), guid.get().Data4[3]);
    ASSERT_EQ(static_cast<unsigned char>(0x3f), guid.get().Data4[4]);
    ASSERT_EQ(static_cast<unsigned char>(0x8f), guid.get().Data4[5]);
    ASSERT_EQ(static_cast<unsigned char>(0x7c), guid.get().Data4[6]);
    ASSERT_EQ(static_cast<unsigned char>(0x19), guid.get().Data4[7]);
}

TEST(GuidTests, RefGuidOperatorBoolTest)
{
    dhorn::windows::guid guid;
    dhorn::windows::ref_guid refGuid = guid;
    ASSERT_FALSE(static_cast<bool>(refGuid));

    guid.get().Data3 = 1;
    ASSERT_TRUE(static_cast<bool>(refGuid));
}

TEST(GuidTests, RefGuidToStringTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    dhorn::windows::ref_guid refGuid = guid;
    ASSERT_EQ("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, refGuid.to_string());
}

TEST(GuidTests, RefGuidOStreamTest)
{
    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    dhorn::windows::ref_guid refGuid = guid;

    std::stringstream stream;
    stream << refGuid;
    ASSERT_EQ("{8E23D663-8CD5-4333-B827-121A3F8F7C19}"s, stream.str());
}

TEST(GuidTests, GuidComparisonTest)
{
    constexpr GUID nullGuid = {};
    dhorn::windows::ref_guid nullRefGuid = nullGuid;

    // {8E23D663-8CD5-4333-B827-121A3F8F7C19}
    constexpr GUID value = { 0x8e23d663, 0x8cd5, 0x4333, { 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };
    constexpr dhorn::windows::guid guid = value;
    dhorn::windows::ref_guid refGuid = guid;

    auto compare = [](const auto& a, const auto& b, int result)
    {
        ASSERT_EQ(result == 0, a == b);
        ASSERT_EQ(result == 0, b == a);

        ASSERT_EQ(result != 0, a != b);
        ASSERT_EQ(result != 0, b != a);

        ASSERT_EQ(result < 0, a < b);
        ASSERT_EQ(result > 0, b < a);

        ASSERT_EQ(result <= 0, a <= b);
        ASSERT_EQ(result >= 0, b <= a);

        ASSERT_EQ(result > 0, a > b);
        ASSERT_EQ(result < 0, b > a);

        ASSERT_EQ(result >= 0, a >= b);
        ASSERT_EQ(result <= 0, b >= a);
    };

    compare(dhorn::windows::null_guid, dhorn::windows::null_guid, 0);
    compare(nullRefGuid, nullRefGuid, 0);
    compare(guid, guid, 0);
    compare(refGuid, refGuid, 0);

    compare(nullGuid, dhorn::windows::null_guid, 0);
    compare(nullGuid, nullRefGuid, 0);
    compare(dhorn::windows::null_guid, nullRefGuid, 0);

    compare(value, guid, 0);
    compare(value, refGuid, 0);
    compare(guid, refGuid, 0);

    compare(nullGuid, guid, -1);
    compare(nullGuid, refGuid, -1);
    compare(dhorn::windows::null_guid, value, -1);
    compare(dhorn::windows::null_guid, guid, -1);
    compare(dhorn::windows::null_guid, refGuid, -1);
    compare(nullRefGuid, value, -1);
    compare(nullRefGuid, guid, -1);
    compare(nullRefGuid, refGuid, -1);
}

TEST(GuidTests, GuidHashTest)
{
    constexpr dhorn::windows::guid guid = { 0x8e23d663, 0x8cd5, 0x4333,{ 0xb8, 0x27, 0x12, 0x1a, 0x3f, 0x8f, 0x7c, 0x19 } };

    // This is more of a "does it compile" test since there's no good way to test how good the hash is
    std::hash<dhorn::windows::guid> hasher;
    ASSERT_NE(hasher(dhorn::windows::null_guid), hasher(guid));
}
