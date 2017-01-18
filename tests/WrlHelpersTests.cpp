/*
 * Duncan Horn
 *
 * WrlHelpersTests.cpp
 *
 * Tests for wrl_utility.h
 */
#include "stdafx.h"

#include <dhorn/wrl/wrl_utility.h>

#include "TestInterfaces.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    class ImplementsFewUnknown : public dhorn::wrl::implements<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        Unk0,
        Unk1,
        Unk2,
        Unk3>
    {
    public:

        IFACEMETHODIMP_(int) unk_0() { return 0; }
        IFACEMETHODIMP_(int) unk_1() { return 1; }
        IFACEMETHODIMP_(int) unk_2() { return 2; }
        IFACEMETHODIMP_(int) unk_3() { return 3; }
    };

    class ImplementsAllUnknown : public dhorn::wrl::implements<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        Unk0,
        Unk1,
        Unk2,
        Unk3,
        Unk4,
        Unk5,
        Unk6,
        Unk7,
        Unk8,
        Unk9,
        Unk10>
    {
    public:

        IFACEMETHODIMP_(int) unk_0() { return 0; }
        IFACEMETHODIMP_(int) unk_1() { return 1; }
        IFACEMETHODIMP_(int) unk_2() { return 2; }
        IFACEMETHODIMP_(int) unk_3() { return 3; }
        IFACEMETHODIMP_(int) unk_4() { return 4; }
        IFACEMETHODIMP_(int) unk_5() { return 5; }
        IFACEMETHODIMP_(int) unk_6() { return 6; }
        IFACEMETHODIMP_(int) unk_7() { return 7; }
        IFACEMETHODIMP_(int) unk_8() { return 8; }
        IFACEMETHODIMP_(int) unk_9() { return 9; }
        IFACEMETHODIMP_(int) unk_10() { return 10; }
    };

    class ImplementsFewUnknownRuntimeClass WrlFinal : public dhorn::wrl::runtime_class<
        ImplementsFewUnknown>
    {
    };

    class ImplementsAllUnknownRuntimeClass WrlFinal : public dhorn::wrl::runtime_class<
        ImplementsAllUnknown>
    {
    };



    class ImplementsFewInspectable : public dhorn::wrl::implements<
        Insp0,
        Insp1,
        Insp2,
        Insp3>
    {
    public:

        IFACEMETHODIMP_(int) insp_0() { return 0; }
        IFACEMETHODIMP_(int) insp_1() { return 1; }
        IFACEMETHODIMP_(int) insp_2() { return 2; }
        IFACEMETHODIMP_(int) insp_3() { return 3; }
    };

    class ImplementsAllInspectable : public dhorn::wrl::implements<
        Insp0,
        Insp1,
        Insp2,
        Insp3,
        Insp4,
        Insp5,
        Insp6,
        Insp7,
        Insp8,
        Insp9,
        Insp10>
    {
    public:

        IFACEMETHODIMP_(int) insp_0() { return 0; }
        IFACEMETHODIMP_(int) insp_1() { return 1; }
        IFACEMETHODIMP_(int) insp_2() { return 2; }
        IFACEMETHODIMP_(int) insp_3() { return 3; }
        IFACEMETHODIMP_(int) insp_4() { return 4; }
        IFACEMETHODIMP_(int) insp_5() { return 5; }
        IFACEMETHODIMP_(int) insp_6() { return 6; }
        IFACEMETHODIMP_(int) insp_7() { return 7; }
        IFACEMETHODIMP_(int) insp_8() { return 8; }
        IFACEMETHODIMP_(int) insp_9() { return 9; }
        IFACEMETHODIMP_(int) insp_10() { return 10; }
    };

    class ImplementsFewInspectableRuntimeClass WrlFinal : public dhorn::wrl::runtime_class<
        ImplementsFewInspectable>
    {
    };

    class ImplementsAllInspectableRuntimeClass WrlFinal : public dhorn::wrl::runtime_class<
        ImplementsAllInspectable>
    {
    };



    TEST_CLASS(WrlHelpersTests)
    {
        TEST_METHOD(ImplementsFewUnknownTest)
        {
            auto ptr = Microsoft::WRL::Make<ImplementsFewUnknownRuntimeClass>();
            Assert::IsNotNull(ptr.Get());

            Assert::AreEqual(0, ptr->unk_0());
            Assert::AreEqual(1, ptr->unk_1());
            Assert::AreEqual(2, ptr->unk_2());
            Assert::AreEqual(3, ptr->unk_3());
        }

        TEST_METHOD(ImplementsFewInspectableTest)
        {
            auto ptr = Microsoft::WRL::Make<ImplementsFewInspectableRuntimeClass>();
            Assert::IsNotNull(ptr.Get());

            Assert::AreEqual(0, ptr->insp_0());
            Assert::AreEqual(1, ptr->insp_1());
            Assert::AreEqual(2, ptr->insp_2());
            Assert::AreEqual(3, ptr->insp_3());
        }

        TEST_METHOD(ImplementsAllUnknownTest)
        {
            auto ptr = Microsoft::WRL::Make<ImplementsAllUnknownRuntimeClass>();
            Assert::IsNotNull(ptr.Get());

            Assert::AreEqual(0, ptr->unk_0());
            Assert::AreEqual(1, ptr->unk_1());
            Assert::AreEqual(2, ptr->unk_2());
            Assert::AreEqual(3, ptr->unk_3());
            Assert::AreEqual(4, ptr->unk_4());
            Assert::AreEqual(5, ptr->unk_5());
            Assert::AreEqual(6, ptr->unk_6());
            Assert::AreEqual(7, ptr->unk_7());
            Assert::AreEqual(8, ptr->unk_8());
            Assert::AreEqual(9, ptr->unk_9());
            Assert::AreEqual(10, ptr->unk_10());
        }
    };
}
