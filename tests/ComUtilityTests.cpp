/*
 * Duncan Horn
 *
 * ComPtrTests.cpp
 *
 * Tests for the com_utility.h header
 */
#include "stdafx.h"

#include <dhorn/com/com_utility.h>

#include "TestInterfaces.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(IsUnknownTests)
    {
        TEST_METHOD(IUnknownTest)
        {
            Assert::IsTrue(dhorn::com::is_unknown<IUnknown>::value);
        }

        TEST_METHOD(SingleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::is_unknown<IBase>::value);
        }

        TEST_METHOD(MultipleDerivedTest)
        {
            Assert::IsTrue(dhorn::com::is_unknown<IFoo>::value);
            Assert::IsTrue(dhorn::com::is_unknown<IBar>::value);
        }

        TEST_METHOD(VirtualInheritanceTest)
        {
            Assert::IsTrue(dhorn::com::is_unknown<IFooBar>::value);
        }

        TEST_METHOD(NonIUnknownTest)
        {
            Assert::IsFalse(dhorn::com::is_unknown<std::string>::value);
        }

        TEST_METHOD(MultipleTypeTest)
        {
            Assert::IsTrue(dhorn::com::is_unknown<IBase, IFoo, IBar>::value);
            Assert::IsTrue(dhorn::com::is_unknown<std::string, IFoo>::value);
            Assert::IsTrue(dhorn::com::is_unknown<IFoo, std::string>::value);

            Assert::IsFalse(dhorn::com::is_unknown<std::string, std::wstring, int>::value);
        }
    };
}
