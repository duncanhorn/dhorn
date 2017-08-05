/*
 * Duncan Horn
 *
 * TestInterfaces.h
 *
 * A set of interface declarations used for tests
 */
#pragma once

#include <atomic>
#include <dhorn/com/com_utility.h>
#include <cassert>
#include <Unknwn.h>

namespace dhorn::tests
{
    /*
     * Interfaces
     */
#pragma region Interfaces

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

#pragma endregion



    /*
     * Implementations
     */
#pragma region Implementations

    template <typename... Interfaces>
    class UnknownBase :
        public Interfaces...
    {
    public:

        // IUnknown
        virtual ULONG __stdcall AddRef() noexcept override
        {
            auto value = ++this->_refCount;
            assert(value != 1);
            return value;
        }

        virtual ULONG __stdcall Release() noexcept override
        {
            auto value = --this->_refCount;

            if (value == 0)
            {
                delete this;
            }

            return value;
        }

        virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ptr) noexcept override
        {
            return dhorn::com::variadic_query_interface<IUnknown, Interfaces...>(this, riid, ptr);
        }

    protected:

        std::atomic_ulong _refCount{1};
    };

    template <typename... Interfaces>
    class BaseImpl : public UnknownBase<IBase, Interfaces...>
    {
    public:

        // IBase
        virtual size_t __stdcall RefCount() noexcept override
        {
            return this->_refCount.load();
        }
    };

    class FooImpl : public BaseImpl<IFoo>
    {
    public:

        // IFoo
        virtual void __stdcall Foo() noexcept override
        {
        }
    };

    class Barmpl : public BaseImpl<IBar>
    {
    public:

        // IFoo
        virtual void __stdcall Bar() noexcept override
        {
        }
    };

#pragma endregion
}
