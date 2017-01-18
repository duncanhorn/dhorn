/*
 * Duncan Horn
 *
 * TestInterfaces.h
 *
 * A set of interface declarations used for tests
 */
#pragma once

#include <inspectable.h>

#define DECLARE_UNKNOWN(val, iid)   \
__interface __declspec(uuid(iid))   \
Unk##val : public IUnknown          \
{                                   \
    STDMETHOD_(int, unk_##val)();   \
}

#define DECLARE_INSPECTABLE(val, iid)   \
__interface __declspec(uuid(iid))       \
Insp##val : public IInspectable         \
{                                       \
    STDMETHOD_(int, insp_##val)();      \
}

namespace dhorn::tests
{
    __interface __declspec(uuid("{56C9CB0F-534C-42D5-B297-9D77E71D908C}"))
    IBase :
        public IUnknown
    {
        STDMETHOD_(size_t, RefCount)(void);
    };

    __interface __declspec(uuid("{FAB826E0-BB52-4CD5-83B1-C401D5386568}"))
    IFoo :
        public IBase
    {
        STDMETHOD_(void, Foo)();
    };

    __interface __declspec(uuid("{D8C6769D-72E5-4CF0-B65F-348C40044EAA}"))
    IBar :
        public IBase
    {
        STDMETHOD_(void, Bar)();
    };

    __interface __declspec(uuid("46284361-1D10-43B3-93B6-5928FA0F2E7D"))
    IFooBar :
        public IFoo,
        public IBar
    {
    };



    DECLARE_UNKNOWN(0, "4FB44B3E-2E49-40C2-89B3-7EB0876BEA69");
    DECLARE_UNKNOWN(1, "90C741F3-3670-4426-9A06-5C20AA4B7FD7");
    DECLARE_UNKNOWN(2, "53534407-0C7F-48A2-BCB8-31BBEED00B80");
    DECLARE_UNKNOWN(3, "1C7C16A3-53DB-4617-A83A-4D5D03D4EF4D");
    DECLARE_UNKNOWN(4, "24A51486-5C0A-42FC-A19B-4AAD74C1A848");
    DECLARE_UNKNOWN(5, "BAB2C82D-BFD9-44E2-B99D-6F24D144CD52");
    DECLARE_UNKNOWN(6, "C93EC1B9-5455-4EF2-8DE3-4BE28C318826");
    DECLARE_UNKNOWN(7, "E60DD514-7B61-45D2-AD17-65BC291A3D92");
    DECLARE_UNKNOWN(8, "F34EC67D-C248-4CF6-A2D3-52AA1115F1F6");
    DECLARE_UNKNOWN(9, "B2259F3B-B5CE-4B29-877E-B8C1796D310E");
    DECLARE_UNKNOWN(10, "9099C241-0FC0-44CA-8DED-BAED8C3BE74F");

    DECLARE_INSPECTABLE(0, "BB48A7C9-C39D-4F54-959B-E1604AE8CDC0");
    DECLARE_INSPECTABLE(1, "74DC9823-AE93-4380-BFE3-A94975CFC38E");
    DECLARE_INSPECTABLE(2, "F9F90067-E13C-4476-8312-6EFBA6B17116");
    DECLARE_INSPECTABLE(3, "52ED3039-386C-4F03-BC12-710B4BB28869");
    DECLARE_INSPECTABLE(4, "15B24CC2-8D85-433F-BF3B-E892BE68CE7F");
    DECLARE_INSPECTABLE(5, "63BF9338-69E8-4869-914E-A4A437556C31");
    DECLARE_INSPECTABLE(6, "6C7B82FA-FE26-4901-A8AC-92ADBD56CF92");
    DECLARE_INSPECTABLE(7, "EF570823-FC50-4AC9-B2E1-875E638726B3");
    DECLARE_INSPECTABLE(8, "B5D65AC9-5787-434F-9345-AC71167A5FAF");
    DECLARE_INSPECTABLE(9, "FE187B8C-4664-4083-A8F2-BE45CB920F00");
    DECLARE_INSPECTABLE(10, "B932A141-7743-4336-B215-28F13FD52597");
}
