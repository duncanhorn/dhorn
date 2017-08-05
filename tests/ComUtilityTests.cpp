/*
 * Duncan Horn
 *
 * ComUtilityTests.cpp
 *
 * Tests for the com_utility.h header
 */
#include "stdafx.h"

#include <dhorn/com/com_utility.h>

#include "TestInterfaces.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(VariadicQueryInterfaceTests)
    {
        TEST_METHOD(UnknownCastTest)
        {
            auto ptr = new UnknownBase<IUnknown>();

            void* result;
            const auto hr = com::variadic_query_interface<IUnknown>(ptr, IID_IUnknown, &result);
            Assert::IsTrue(SUCCEEDED(hr));
            Assert::IsTrue(ptr == result);

            Assert::AreEqual(3ul, ptr->AddRef());
            ptr->Release();
            ptr->Release();
            ptr->Release();
        }
    };
}
