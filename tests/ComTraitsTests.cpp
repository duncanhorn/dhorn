/*
 * Duncan Horn
 *
 * ComTraitsTests.cpp
 *
 * Tests for the com_traits.h header
 */
#include "stdafx.h"

#include <dhorn/com/com_traits.h>

#include "TestInterfaces.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(AnyUnknownTests)
    {
        TEST_METHOD(IUnknownTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown_v<IUnknown>);
        }

        TEST_METHOD(SingleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown_v<IBase>);
        }

        TEST_METHOD(MultipleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown_v<IFoo>);
            Assert::IsTrue(dhorn::com::any_unknown_v<IBar>);
        }

        TEST_METHOD(MultipleInheritanceTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown_v<IFooBar>);
        }

        TEST_METHOD(NonIUnknownTest)
        {
            Assert::IsFalse(dhorn::com::any_unknown_v<std::string>);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown_v<IBase, IFoo, IBar>);
            Assert::IsTrue(dhorn::com::any_unknown_v<IFoo, std::string, std::wstring>);
            Assert::IsTrue(dhorn::com::any_unknown_v<std::string, IFoo, std::wstring>);
            Assert::IsTrue(dhorn::com::any_unknown_v<std::string, std::wstring, IFoo>);

            Assert::IsFalse(dhorn::com::any_unknown_v<std::string, std::wstring, int>);
        }
    };



    TEST_CLASS(AllUnknownTests)
    {
        TEST_METHOD(IUnknownTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown_v<IUnknown>);
        }

        TEST_METHOD(SingleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown_v<IBase>);
        }

        TEST_METHOD(MultipleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown_v<IFoo>);
            Assert::IsTrue(dhorn::com::all_unknown_v<IBar>);
        }

        TEST_METHOD(MultipleInheritanceTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown_v<IFooBar>);
        }

        TEST_METHOD(NonIUnknownTest)
        {
            Assert::IsFalse(dhorn::com::all_unknown_v<std::string>);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown_v<IBase, IFoo, IBar>);

            Assert::IsFalse(dhorn::com::all_unknown_v<std::string, IFoo, IBar>);
            Assert::IsFalse(dhorn::com::all_unknown_v<IFoo, std::string, IFoo>);
            Assert::IsFalse(dhorn::com::all_unknown_v<IFoo, IBar, std::string>);
            Assert::IsFalse(dhorn::com::all_unknown_v<std::string, std::wstring, int>);
        }
    };



    TEST_CLASS(ComTraitsTests)
    {
        TEST_METHOD(HasIidTest)
        {
            Assert::IsFalse(com::has_iid_v<std::string>);
            Assert::IsTrue(com::has_iid_v<IUnknown>);

            struct __declspec(uuid("7D7C05B6-75ED-4B8E-8631-DFBD3084CFED")) HasUuid : public IUnknown {};
            struct NoUuid : public IUnknown {};

            Assert::IsTrue(com::has_iid_v<HasUuid>);
            Assert::IsFalse(com::has_iid_v<NoUuid>);
        }
    };
}
