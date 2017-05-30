/*
 * Duncan Horn
 *
 * WeakRefTests.cpp
 *
 * Tests for the com_traits.h header
 */
#include "stdafx.h"

#include <dhorn/winrt/weak_ref.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    struct __declspec(uuid("390715DE-54CB-4713-B7B7-6DFBFF1D3BB9"))
    ITest :
        public IInspectable
    {
    };

    class TestImpl : public ITest
    {
    public:


    };

    TEST_CLASS(WeakRefTests)
    {
        TEST_METHOD(DefaultConstructionTest)
        {
            winrt::weak_ref<IInspectable> ref;
            Assert::IsTrue(ref.expired());
            Assert::IsTrue(ref.lock() == nullptr);

            Assert::IsTrue(std::is_nothrow_default_constructible_v<winrt::weak_ref<IInspectable>>);
        }

        TEST_METHOD(SizeTests)
        {
            Assert::AreEqual(sizeof(void*), sizeof(winrt::weak_ref<ITest>));
            Assert::AreEqual(sizeof(void*) * 2, sizeof(winrt::weak_ref<TestImpl>));
        }
    };
}
