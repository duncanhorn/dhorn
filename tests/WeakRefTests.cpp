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
#pragma region Test Types

    struct __declspec(uuid("390715DE-54CB-4713-B7B7-6DFBFF1D3BB9"))
    ITest :
        public IInspectable
    {
    };

    class WeakReferenceImpl final :
        public IWeakReference,
        public object_counter
    {
    public:
        WeakReferenceImpl(IInspectable* ptr) :
            _obj(ptr)
        {
        }

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef() noexcept override
        {
            auto result = ++this->_refCount;
            Assert::AreNotEqual(1u, result);
            return result;
        }

        IFACEMETHODIMP_(ULONG) Release() noexcept override
        {
            auto result = --this->_refCount;
            if (result == 0)
            {
                delete this;
            }

            Assert::AreNotEqual(result, 0u - 1);
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
        public IWeakReferenceSource,
        public object_counter
    {
    public:
        TestImpl()
        {
            this->_data.attach(new WeakReferenceImpl(this));
        }

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef() noexcept override
        {
            auto result = this->_data->IncrementRefCount();
            Assert::AreNotEqual(1ul, result);
            return result;
        }

        IFACEMETHODIMP_(ULONG) Release() noexcept override
        {
            auto result = this->_data->DecrementRefCount();
            if (result == 0)
            {
                delete this;
            }

            Assert::AreNotEqual(result, 0ul - 1);
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

#pragma endregion



    TEST_CLASS(WeakRefTests)
    {
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            object_counter::reset();
        }

        TEST_METHOD_CLEANUP(TestCleanup)
        {
            Assert::AreEqual(0u, object_counter::instance_count);
        }

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

        TEST_METHOD(RawPointerConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, TestImpl*>);
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, ITest*>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<ITest>, IInspectable*>);

            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<TestImpl>, TestImpl*>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, ITest*>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, IInspectable*>);

            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());
            auto interfacePtr = static_cast<ITest*>(comPtr.get());
            auto implPtr = comPtr.get();

            winrt::weak_ref<ITest> interfaceRef(interfacePtr);
            winrt::weak_ref<ITest> interfaceRef2(implPtr);
            winrt::weak_ref<TestImpl> implRef(implPtr);

            Assert::IsFalse(interfaceRef.expired());
            Assert::IsFalse(interfaceRef2.expired());
            Assert::IsFalse(implRef.expired());

            Assert::IsTrue(interfacePtr == interfaceRef.lock());
            Assert::IsTrue(interfacePtr == interfaceRef2.lock());
            Assert::IsTrue(implPtr == implRef.lock());

            Assert::AreEqual(1ul, comPtr->RefCount());
        }

        TEST_METHOD(ComPtrConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, const com::com_ptr<TestImpl>&>);
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, const com::com_ptr<ITest>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<ITest>, const com::com_ptr<IInspectable>&>);

            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<TestImpl>, const com::com_ptr<TestImpl>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, const com::com_ptr<ITest>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, const com::com_ptr<IInspectable>&>);

            com::com_ptr<TestImpl> implPtr;
            implPtr.attach(new TestImpl());
            com::com_ptr<ITest> interfacePtr = implPtr;

            winrt::weak_ref<ITest> interfaceRef(interfacePtr);
            winrt::weak_ref<ITest> interfaceRef2(implPtr);
            winrt::weak_ref<TestImpl> implRef(implPtr);

            Assert::IsFalse(interfaceRef.expired());
            Assert::IsFalse(interfaceRef2.expired());
            Assert::IsFalse(implRef.expired());

            Assert::IsTrue(interfacePtr == interfaceRef.lock());
            Assert::IsTrue(interfacePtr == interfaceRef2.lock());
            Assert::IsTrue(implPtr == implRef.lock());

            Assert::AreEqual(2ul, implPtr->RefCount());
        }

        TEST_METHOD(CopyConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, const winrt::weak_ref<TestImpl>&>);
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, const winrt::weak_ref<ITest>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<ITest>, const winrt::weak_ref<IInspectable>&>);

            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<TestImpl>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<ITest>&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<IInspectable>&>);

            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef(comPtr);
            winrt::weak_ref<TestImpl> implRefCopy(implRef);
            Assert::IsTrue(comPtr == implRef.lock());
            Assert::IsTrue(comPtr == implRefCopy.lock());

            winrt::weak_ref<ITest> testRef(implRef);
            winrt::weak_ref<ITest> testRefCopy(testRef);
            Assert::IsTrue(comPtr == testRef.lock());
            Assert::IsTrue(comPtr == testRefCopy.lock());

            // Copy from null
            winrt::weak_ref<TestImpl> implEmpty;
            winrt::weak_ref<TestImpl> implEmptyCopy(implEmpty);
            Assert::IsTrue(implEmptyCopy.expired());

            winrt::weak_ref<ITest> testEmpty(implEmptyCopy);
            winrt::weak_ref<ITest> testEmptyCopy(testEmpty);
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());
        }

        TEST_METHOD(MoveConstructionTest)
        {
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, winrt::weak_ref<TestImpl>&&>);
            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<ITest>, winrt::weak_ref<ITest>&&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<ITest>, winrt::weak_ref<IInspectable>&&>);

            Assert::IsTrue(std::is_constructible_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<TestImpl>&&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<ITest>&&>);
            Assert::IsFalse(std::is_constructible_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<IInspectable>&&>);

            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef(comPtr);
            winrt::weak_ref<TestImpl> implRefCopy(std::move(implRef));
            Assert::IsTrue(implRef.expired());
            Assert::IsTrue(comPtr == implRefCopy.lock());

            winrt::weak_ref<ITest> testRef(std::move(implRefCopy));
            Assert::IsTrue(implRefCopy.expired());
            Assert::IsTrue(comPtr == testRef.lock());

            winrt::weak_ref<ITest> testRefCopy(std::move(testRef));
            Assert::IsTrue(testRef.expired());
            Assert::IsTrue(comPtr == testRefCopy.lock());

            // Move from null
            winrt::weak_ref<TestImpl> implEmpty;
            winrt::weak_ref<TestImpl> implEmptyCopy(std::move(implEmpty));
            Assert::IsTrue(implEmptyCopy.expired());

            winrt::weak_ref<ITest> testEmpty(std::move(implEmptyCopy));
            winrt::weak_ref<ITest> testEmptyCopy(std::move(testEmpty));
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());
        }

        TEST_METHOD(CopyAssignmentTest)
        {
            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<ITest>, const winrt::weak_ref<TestImpl>&>);
            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<ITest>, const winrt::weak_ref<ITest>&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<ITest>, const winrt::weak_ref<IInspectable>&>);

            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<TestImpl>&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<ITest>&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<TestImpl>, const winrt::weak_ref<IInspectable>&>);

            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef(comPtr);
            winrt::weak_ref<TestImpl> implRefCopy;
            winrt::weak_ref<ITest> testRef;
            winrt::weak_ref<ITest> testRefCopy;

            // Try and prevent optimizations
            Assert::IsTrue(implRefCopy.expired());
            Assert::IsTrue(testRefCopy.expired());
            Assert::IsTrue(testRef.expired());

            implRefCopy = implRef;
            Assert::IsTrue(comPtr == implRef.lock());
            Assert::IsTrue(comPtr == implRefCopy.lock());

            testRef = implRef;
            testRefCopy = testRef;
            Assert::IsTrue(comPtr == testRef.lock());
            Assert::IsTrue(comPtr == testRefCopy.lock());

            // Copy from null
            winrt::weak_ref<TestImpl> implEmpty;
            winrt::weak_ref<TestImpl> implEmptyCopy;
            winrt::weak_ref<ITest> testEmpty;
            winrt::weak_ref<ITest> testEmptyCopy;

            // Try and prevent optimizations
            Assert::IsTrue(implEmptyCopy.expired());
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());

            implEmptyCopy = implEmpty;
            Assert::IsTrue(implEmptyCopy.expired());

            testEmpty = implEmptyCopy;
            testEmptyCopy = testEmpty;
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());
        }

        TEST_METHOD(MoveAssignmentTest)
        {
            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<ITest>, winrt::weak_ref<TestImpl>&&>);
            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<ITest>, winrt::weak_ref<ITest>&&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<ITest>, winrt::weak_ref<IInspectable>&&>);

            Assert::IsTrue(std::is_assignable_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<TestImpl>&&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<ITest>&&>);
            Assert::IsFalse(std::is_assignable_v<winrt::weak_ref<TestImpl>, winrt::weak_ref<IInspectable>&&>);

            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef(comPtr);
            winrt::weak_ref<TestImpl> implRefCopy;
            winrt::weak_ref<ITest> testRef;
            winrt::weak_ref<ITest> testRefCopy;

            // Try and prevent optimizations
            Assert::IsTrue(implRefCopy.expired());
            Assert::IsTrue(testRef.expired());
            Assert::IsTrue(testRefCopy.expired());

            implRefCopy = std::move(implRef);
            Assert::IsTrue(implRef.expired());
            Assert::IsTrue(comPtr == implRefCopy.lock());

            testRef = std::move(implRefCopy);
            Assert::IsTrue(implRefCopy.expired());
            Assert::IsTrue(comPtr == testRef.lock());

            testRefCopy = std::move(testRef);
            Assert::IsTrue(testRef.expired());
            Assert::IsTrue(comPtr == testRefCopy.lock());

            // Move from null
            winrt::weak_ref<TestImpl> implEmpty;
            winrt::weak_ref<TestImpl> implEmptyCopy;
            winrt::weak_ref<ITest> testEmpty;
            winrt::weak_ref<ITest> testEmptyCopy;

            // Try and prevent optimizations
            Assert::IsTrue(implEmptyCopy.expired());
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());

            implEmptyCopy = std::move(implEmpty);
            Assert::IsTrue(implEmptyCopy.expired());

            testEmpty = std::move(implEmptyCopy);
            testEmptyCopy = std::move(testEmpty);
            Assert::IsTrue(testEmpty.expired());
            Assert::IsTrue(testEmptyCopy.expired());
        }

        TEST_METHOD(ResetTest)
        {
            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef(comPtr);
            winrt::weak_ref<ITest> testRef(comPtr);

            Assert::IsFalse(implRef.expired());
            Assert::IsFalse(testRef.expired());

            implRef.reset();
            testRef.reset();

            Assert::IsTrue(implRef.expired());
            Assert::IsTrue(testRef.expired());
            Assert::IsNull(implRef.lock().get());
            Assert::IsNull(testRef.lock().get());
        }

        TEST_METHOD(SwapTest)
        {
            com::com_ptr<TestImpl> comPtr;
            comPtr.attach(new TestImpl());

            winrt::weak_ref<TestImpl> implRef1(comPtr);
            winrt::weak_ref<TestImpl> implRef2;
            implRef1.swap(implRef2);
            Assert::IsTrue(implRef1.expired());
            Assert::IsFalse(implRef2.expired());
            Assert::IsTrue(comPtr == implRef2.lock());

            winrt::weak_ref<ITest> testRef1(comPtr);
            winrt::weak_ref<ITest> testRef2;
            testRef1.swap(testRef2);
            Assert::IsTrue(testRef1.expired());
            Assert::IsFalse(testRef2.expired());
            Assert::IsTrue(comPtr == testRef2.lock());
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
                weak2 = weak;
            }

            Assert::IsTrue(weak.expired());
            Assert::IsTrue(weak.lock() == nullptr);

            Assert::IsTrue(weak2.expired());
            Assert::IsTrue(weak2.lock() == nullptr);
        }
    };
}
