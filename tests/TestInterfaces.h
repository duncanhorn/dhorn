/*
 * Duncan Horn
 *
 * TestInterfaces.h
 *
 * A set of interface declarations used for tests
 */
#pragma once

#include <Unknwn.h>

namespace dhorn::tests
{
    __interface __declspec(uuid("{56C9CB0F-534C-42D5-B297-9D77E71D908C}"))
    IBase :
        public IUnknown
    {
        STDMETHOD_(std::size_t, RefCount)(void);
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
}
