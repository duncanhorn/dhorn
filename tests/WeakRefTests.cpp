/*
 * Duncan Horn
 *
 * WeakRefTests.cpp
 *
 * Tests for the com_traits.h header
 */
#include "stdafx.h"

#include <atomic>
#include <dhorn/winrt/weak_ref.h>
#include <wrl/client.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    struct __declspec(uuid("390715DE-54CB-4713-B7B7-6DFBFF1D3BB9"))
    ITest :
        public IInspectable
    {
    };

    class WeakReferenceImpl final :
        public IWeakReference
    {
    public:
        WeakReferenceImpl(IInspectable* ptr) :
            _obj(ptr)
        {
        }

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef() noexcept override
        {
            return ++this->_refCount;
        }

        IFACEMETHODIMP_(ULONG) Release() noexcept override
        {
            auto result = --this->_refCount;
            if (result == 0)
            {
                delete this;
            }

            return result;
        }

        IFACEMETHODIMP QueryInterface(REFIID iid, void** result)
        {
            HRESULT hr = E_NOINTERFACE;
            if (iid == __uuidof(IUnknown))
            {
                *result = static_cast<IUnknown*>(this);
                hr = S_OK;
            }
            else if (iid == __uuidof(IWeakReference))
            {
                *result = static_cast<IWeakReference*>(this);
                hr = S_OK;
            }

            if (SUCCEEDED(hr))
            {
                AddRef();
            }

            return hr;
        }

        // IWeakReference
        IFACEMETHODIMP Resolve(REFIID riid, IInspectable** result) noexcept override
        {
            *result = nullptr;
            while (true)
            {
                auto refCount = this->_objRefCount.load();
                if (refCount == 0)
                {
                    // Already destroyed
                    return S_OK;
                }

                if (this->_objRefCount.compare_exchange_strong(refCount, refCount + 1))
                {
                    // Object still exists
                    break;
                }

                // Concurrently changed; try again
            }

            // Object still exists. See if implements the desired interface
            void* tmp;
            const auto hr = this->_obj->QueryInterface(riid, &tmp);

            // No matter what, even if the QI failed, we need to "undo" the earlier AddRef
            --this->_objRefCount;

            if (SUCCEEDED(hr))
            {
                *result = reinterpret_cast<IInspectable*>(tmp);
            }

            return hr;
        }

        // Underlying Object Manipulation
        ULONG IncrementRefCount()
        {
            return ++this->_objRefCount;
        }

        ULONG DecrementRefCount()
        {
            return --this->_objRefCount;
        }

        ULONG RefCount()
        {
            return this->_objRefCount.load();
        }

    private:

        // This object's ref count
        std::atomic_uint32_t _refCount{ 1 };

        // The object we are referencing
        IInspectable* _obj;
        std::atomic_uint32_t _objRefCount{ 1 };
    };

    class TestImpl final :
        public ITest,
        public IWeakReferenceSource
    {
    public:
        TestImpl()
        {
            this->_data.attach(new WeakReferenceImpl(this));
        }

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef() noexcept override
        {
            return this->_data->IncrementRefCount();
        }

        IFACEMETHODIMP_(ULONG) Release() noexcept override
        {
            auto result = this->_data->DecrementRefCount();
            if (result == 0)
            {
                delete this;
            }

            return result;
        }

        IFACEMETHODIMP QueryInterface(REFIID iid, void** result) noexcept override
        {
            HRESULT hr = E_NOINTERFACE;
            if (iid == __uuidof(IUnknown))
            {
                *result = static_cast<IUnknown*>(static_cast<ITest*>(this));
                hr = S_OK;
            }
            else if (iid == __uuidof(IInspectable))
            {
                *result = static_cast<IInspectable*>(this);
                hr = S_OK;
            }
            else if (iid == __uuidof(ITest))
            {
                *result = static_cast<ITest*>(this);
                hr = S_OK;
            }
            else if (iid == __uuidof(IWeakReferenceSource))
            {
                *result = static_cast<IWeakReferenceSource*>(this);
                hr = S_OK;
            }

            if (SUCCEEDED(hr))
            {
                AddRef();
            }

            return hr;
        }

        // IInspectable
        IFACEMETHODIMP GetIids(ULONG*, IID**) noexcept override
        {
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetRuntimeClassName(HSTRING*) noexcept override
        {
            return E_NOTIMPL;
        }

        IFACEMETHODIMP GetTrustLevel(TrustLevel*) noexcept override
        {
            return E_NOTIMPL;
        }

        // IWeakReferenceSource
        IFACEMETHODIMP GetWeakReference(IWeakReference** result) noexcept override
        {
            this->_data.copy_to(result);
            return S_OK;
        }

        // Test Functions
        ULONG RefCount()
        {
            return this->_data->RefCount();
        }

    private:

        com::com_ptr<WeakReferenceImpl> _data;
    };

    TEST_CLASS(WeakRefTests)
    {
        TEST_METHOD(DefaultConstructionTest)
        {
            winrt::weak_ref<ITest> ref;
            Assert::IsTrue(ref.expired());
            Assert::IsTrue(ref.lock() == nullptr);

            winrt::weak_ref<TestImpl> ref2;
            Assert::IsTrue(ref2.expired());
            Assert::IsTrue(ref2.lock() == nullptr);

            Assert::IsTrue(std::is_nothrow_default_constructible_v<winrt::weak_ref<ITest>>);
            Assert::IsTrue(std::is_nothrow_default_constructible_v<winrt::weak_ref<TestImpl>>);
        }

        TEST_METHOD(SizeTests)
        {
            Assert::AreEqual(sizeof(void*), sizeof(winrt::weak_ref<ITest>));
            Assert::AreEqual(sizeof(void*) * 2, sizeof(winrt::weak_ref<TestImpl>));
        }

        TEST_METHOD(ResolveNonDestroyedTest)
        {
            com::com_ptr<TestImpl> test;
            test.attach(new TestImpl());

            winrt::weak_ref<TestImpl> weak(test);
            Assert::IsFalse(weak.expired());
            Assert::IsTrue(weak.lock() == test.get());

            winrt::weak_ref<ITest> weak2(test);
            Assert::IsFalse(weak2.expired());
            Assert::IsTrue(weak2.lock() == test.get());

            Assert::AreEqual(1ul, test->RefCount());

            auto ptr = weak.lock();
            Assert::AreEqual(2ul, test->RefCount());

            auto ptr2 = weak2.lock();
            Assert::AreEqual(3ul, test->RefCount());
        }

        TEST_METHOD(ResolveDestroyedTest)
        {
            winrt::weak_ref<TestImpl> weak;
            winrt::weak_ref<ITest> weak2;
            {
                com::com_ptr<TestImpl> test;
                test.attach(new TestImpl());

                weak = winrt::as_weak(test);
                // TODO: Allow assignment with "down-cast"
            }

            Assert::IsTrue(weak.expired());
            Assert::IsTrue(weak.lock() == nullptr);

            Assert::IsTrue(weak2.expired());
            Assert::IsTrue(weak2.lock() == nullptr);
        }
    };
}
