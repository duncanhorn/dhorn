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
            Assert::IsTrue(dhorn::com::any_unknown<IUnknown>::value);
        }

        TEST_METHOD(SingleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown<IBase>::value);
        }

        TEST_METHOD(MultipleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown<IFoo>::value);
            Assert::IsTrue(dhorn::com::any_unknown<IBar>::value);
        }

        TEST_METHOD(MultipleInheritanceTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown<IFooBar>::value);
        }

        TEST_METHOD(NonIUnknownTest)
        {
            Assert::IsFalse(dhorn::com::any_unknown<std::string>::value);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            Assert::IsTrue(dhorn::com::any_unknown<IBase, IFoo, IBar>::value);
            Assert::IsTrue(dhorn::com::any_unknown<IFoo, std::string, std::wstring>::value);
            Assert::IsTrue(dhorn::com::any_unknown<std::string, IFoo, std::wstring>::value);
            Assert::IsTrue(dhorn::com::any_unknown<std::string, std::wstring, IFoo>::value);

            Assert::IsFalse(dhorn::com::any_unknown<std::string, std::wstring, int>::value);
        }
    };



    TEST_CLASS(AllUnknownTests)
    {
        TEST_METHOD(IUnknownTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown<IUnknown>::value);
        }

        TEST_METHOD(SingleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown<IBase>::value);
        }

        TEST_METHOD(MultipleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown<IFoo>::value);
            Assert::IsTrue(dhorn::com::all_unknown<IBar>::value);
        }

        TEST_METHOD(MultipleInheritanceTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown<IFooBar>::value);
        }

        TEST_METHOD(NonIUnknownTest)
        {
            Assert::IsFalse(dhorn::com::all_unknown<std::string>::value);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            Assert::IsTrue(dhorn::com::all_unknown<IBase, IFoo, IBar>::value);

            Assert::IsFalse(dhorn::com::all_unknown<std::string, IFoo, IBar>::value);
            Assert::IsFalse(dhorn::com::all_unknown<IFoo, std::string, IFoo>::value);
            Assert::IsFalse(dhorn::com::all_unknown<IFoo, IBar, std::string>::value);
            Assert::IsFalse(dhorn::com::all_unknown<std::string, std::wstring, int>::value);
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
