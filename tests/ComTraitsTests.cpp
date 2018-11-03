/*
 * Duncan Horn
 *
 * ComTraitsTests.cpp
 *
 * Tests for the com_traits.h header
 */

#include <dhorn/com/com_traits.h>
#include <gtest/gtest.h>

#include "TestInterfaces.h"

TEST(AnyUnknownTests, IUnknownTest)
{
    ASSERT_TRUE(dhorn::com::any_unknown_v<IUnknown>);
}

TEST(AnyUnknownTests, SingleDerivedTest)
{
    ASSERT_TRUE(dhorn::com::any_unknown_v<IBase>);
}

TEST(AnyUnknownTests, MultipleDerivedTest)
{
    ASSERT_TRUE(dhorn::com::any_unknown_v<IFoo>);
    ASSERT_TRUE(dhorn::com::any_unknown_v<IBar>);
}

TEST(AnyUnknownTests, MultipleInheritanceTest)
{
    ASSERT_TRUE(dhorn::com::any_unknown_v<IFooBar>);
}

TEST(AnyUnknownTests, NonIUnknownTest)
{
    ASSERT_FALSE(dhorn::com::any_unknown_v<std::string>);
}

TEST(AnyUnknownTests, MultipleTypeTest)
{
    ASSERT_TRUE((dhorn::com::any_unknown_v<IBase, IFoo, IBar>));
    ASSERT_TRUE((dhorn::com::any_unknown_v<IFoo, std::string, std::wstring>));
    ASSERT_TRUE((dhorn::com::any_unknown_v<std::string, IFoo, std::wstring>));
    ASSERT_TRUE((dhorn::com::any_unknown_v<std::string, std::wstring, IFoo>));

    ASSERT_FALSE((dhorn::com::any_unknown_v<std::string, std::wstring, int>));
}

TEST(AllUnknownTests, IUnknownTest)
{
    ASSERT_TRUE(dhorn::com::all_unknown_v<IUnknown>);
}

TEST(AllUnknownTests, SingleDerivedTest)
{
    ASSERT_TRUE(dhorn::com::all_unknown_v<IBase>);
}

TEST(AllUnknownTests, MultipleDerivedTest)
{
    ASSERT_TRUE(dhorn::com::all_unknown_v<IFoo>);
    ASSERT_TRUE(dhorn::com::all_unknown_v<IBar>);
}

TEST(AllUnknownTests, MultipleInheritanceTest)
{
    ASSERT_TRUE(dhorn::com::all_unknown_v<IFooBar>);
}

TEST(AllUnknownTests, NonIUnknownTest)
{
    ASSERT_FALSE(dhorn::com::all_unknown_v<std::string>);
}

TEST(AllUnknownTests, MultipleTypeTest)
{
    ASSERT_TRUE((dhorn::com::all_unknown_v<IBase, IFoo, IBar>));

    ASSERT_FALSE((dhorn::com::all_unknown_v<std::string, IFoo, IBar>));
    ASSERT_FALSE((dhorn::com::all_unknown_v<IFoo, std::string, IFoo>));
    ASSERT_FALSE((dhorn::com::all_unknown_v<IFoo, IBar, std::string>));
    ASSERT_FALSE((dhorn::com::all_unknown_v<std::string, std::wstring, int>));
}

TEST(ComTraitsTests, HasIidTest)
{
    ASSERT_FALSE(dhorn::com::has_iid_v<std::string>);
    ASSERT_TRUE(dhorn::com::has_iid_v<IUnknown>);

    struct __declspec(uuid("7D7C05B6-75ED-4B8E-8631-DFBD3084CFED")) HasUuid : public IUnknown {};
    struct NoUuid : public IUnknown {};

    ASSERT_TRUE(dhorn::com::has_iid_v<HasUuid>);
    ASSERT_FALSE(dhorn::com::has_iid_v<NoUuid>);
}
