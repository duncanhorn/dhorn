/*
 * Duncan Horn
 *
 * WeakRefTests.cpp
 *
 * Tests for the com_traits.h header
 */

#include <atomic>
#include <dhorn/winrt/weak_ref.h>
#include <gtest/gtest.h>

#include "object_counter.h"

struct __declspec(uuid("390715DE-54CB-4713-B7B7-6DFBFF1D3BB9"))
ITest :
    public IInspectable
{
};

class WeakReferenceImpl final :
    public IWeakReference,
    public dhorn::tests::object_counter
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
        assert(result != 1ul);
        return result;
    }

    IFACEMETHODIMP_(ULONG) Release() noexcept override
    {
        auto result = --this->_refCount;
        if (result == 0)
        {
            delete this;
        }

        assert(result != 0u - 1);
        return result;
    }

    IFACEMETHODIMP QueryInterface(REFIID iid, void** result) noexcept override
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

class TestImpl :
    public ITest,
    public IWeakReferenceSource,
    public dhorn::tests::object_counter
{
public:
    TestImpl()
    {
        this->_data.attach(new WeakReferenceImpl(this));
    }

    virtual ~TestImpl() {}

    // IUnknown
    IFACEMETHODIMP_(ULONG) AddRef() noexcept override
    {
        auto result = this->_data->IncrementRefCount();
        assert(result != 1ul);
        return result;
    }

    IFACEMETHODIMP_(ULONG) Release() noexcept override
    {
        auto result = this->_data->DecrementRefCount();
        if (result == 0)
        {
            delete this;
        }

        assert(result != 0ul - 1);
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

    dhorn::com::com_ptr<WeakReferenceImpl> _data;
};

// Used to ensure that we can construct a weak_ref<TestImpl> from a TestImpl2 instance
class TestImplDerived :
    public TestImpl
{
};

struct WeakRefTests : testing::Test
{
    virtual void SetUp() override
    {
        dhorn::tests::object_counter::reset();
    }

    virtual void TearDown() override
    {
        ASSERT_EQ(static_cast<std::size_t>(0), dhorn::tests::object_counter::instance_count);
    }
};

TEST_F(WeakRefTests, DefaultConstructionTest)
{
    dhorn::winrt::weak_ref<ITest> ref;
    ASSERT_TRUE(ref.expired());
    ASSERT_TRUE(ref.lock() == nullptr);

    dhorn::winrt::weak_ref<TestImpl> ref2;
    ASSERT_TRUE(ref2.expired());
    ASSERT_TRUE(ref2.lock() == nullptr);

    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::winrt::weak_ref<ITest>>);
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<dhorn::winrt::weak_ref<TestImpl>>);
}

TEST_F(WeakRefTests, RawPointerConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, TestImplDerived*>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, TestImpl*>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, ITest*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, IInspectable*>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, TestImplDerived*>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, TestImpl*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, ITest*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, IInspectable*>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, TestImplDerived*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, TestImpl*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, ITest*>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, IInspectable*>));

    dhorn::com::com_ptr<TestImplDerived> comPtr;
    comPtr.attach(new TestImplDerived());
    auto derivedPtr = comPtr.get();
    auto implPtr = static_cast<TestImpl*>(comPtr.get());
    auto interfacePtr = static_cast<ITest*>(comPtr.get());

    dhorn::winrt::weak_ref<ITest> interfaceRef(interfacePtr);
    dhorn::winrt::weak_ref<ITest> interfaceRef2(implPtr);
    dhorn::winrt::weak_ref<ITest> interfaceRef3(derivedPtr);
    dhorn::winrt::weak_ref<TestImpl> implRef(implPtr);
    dhorn::winrt::weak_ref<TestImpl> implRef2(derivedPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(derivedPtr);

    ASSERT_FALSE(interfaceRef.expired());
    ASSERT_FALSE(interfaceRef2.expired());
    ASSERT_FALSE(interfaceRef3.expired());
    ASSERT_FALSE(implRef.expired());
    ASSERT_FALSE(implRef2.expired());
    ASSERT_FALSE(derivedRef.expired());

    ASSERT_TRUE(interfacePtr == interfaceRef.lock());
    ASSERT_TRUE(interfacePtr == interfaceRef2.lock());
    ASSERT_TRUE(interfacePtr == interfaceRef3.lock());
    ASSERT_TRUE(implPtr == implRef.lock());
    ASSERT_TRUE(implPtr == implRef2.lock());
    ASSERT_TRUE(derivedPtr == derivedRef.lock());

    ASSERT_EQ(1ul, comPtr->RefCount());
}

TEST_F(WeakRefTests, ComPtrConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::com::com_ptr<TestImplDerived>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::com::com_ptr<TestImpl>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::com::com_ptr<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::com::com_ptr<IInspectable>&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::com::com_ptr<TestImplDerived>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::com::com_ptr<TestImpl>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::com::com_ptr<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::com::com_ptr<IInspectable>&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::com::com_ptr<TestImplDerived>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::com::com_ptr<TestImpl>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::com::com_ptr<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::com::com_ptr<IInspectable>&>));

    dhorn::com::com_ptr<TestImplDerived> derivedPtr;
    derivedPtr.attach(new TestImplDerived());
    dhorn::com::com_ptr<TestImpl> implPtr = derivedPtr;
    dhorn::com::com_ptr<ITest> interfacePtr = implPtr;

    dhorn::winrt::weak_ref<ITest> interfaceRef(interfacePtr);
    dhorn::winrt::weak_ref<ITest> interfaceRef2(implPtr);
    dhorn::winrt::weak_ref<ITest> interfaceRef3(derivedPtr);
    dhorn::winrt::weak_ref<TestImpl> implRef(implPtr);
    dhorn::winrt::weak_ref<TestImpl> implRef2(derivedPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(derivedPtr);

    ASSERT_FALSE(interfaceRef.expired());
    ASSERT_FALSE(interfaceRef2.expired());
    ASSERT_FALSE(interfaceRef3.expired());
    ASSERT_FALSE(implRef.expired());
    ASSERT_FALSE(implRef2.expired());
    ASSERT_FALSE(derivedRef.expired());

    ASSERT_TRUE(interfacePtr == interfaceRef.lock());
    ASSERT_TRUE(interfacePtr == interfaceRef2.lock());
    ASSERT_TRUE(interfacePtr == interfaceRef3.lock());
    ASSERT_TRUE(implPtr == implRef.lock());
    ASSERT_TRUE(implPtr == implRef2.lock());
    ASSERT_TRUE(derivedPtr == derivedRef.lock());

    ASSERT_EQ(3ul, implPtr->RefCount());
}

TEST_F(WeakRefTests, CopyConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<IInspectable>&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<IInspectable>&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<IInspectable>&>));

    dhorn::com::com_ptr<TestImplDerived> comPtr;
    comPtr.attach(new TestImplDerived());

    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(comPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRefCopy(derivedRef);
    ASSERT_TRUE(comPtr == derivedRef.lock());
    ASSERT_TRUE(comPtr == derivedRefCopy.lock());

    dhorn::winrt::weak_ref<TestImpl> implRef(derivedRef);
    dhorn::winrt::weak_ref<TestImpl> implRefCopy(implRef);
    ASSERT_TRUE(comPtr == implRef.lock());
    ASSERT_TRUE(comPtr == implRefCopy.lock());

    dhorn::winrt::weak_ref<ITest> testRef(implRef);
    dhorn::winrt::weak_ref<ITest> testRefCopy(testRef);
    ASSERT_TRUE(comPtr == testRef.lock());
    ASSERT_TRUE(comPtr == testRefCopy.lock());

    // Copy from null
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmpty;
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmptyCopy(derivedEmpty);
    ASSERT_TRUE(derivedEmpty.expired());

    dhorn::winrt::weak_ref<TestImpl> implEmpty(derivedEmptyCopy);
    dhorn::winrt::weak_ref<TestImpl> implEmptyCopy(implEmpty);
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());

    dhorn::winrt::weak_ref<ITest> testEmpty(implEmptyCopy);
    dhorn::winrt::weak_ref<ITest> testEmptyCopy(testEmpty);
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());
}

TEST_F(WeakRefTests, MoveConstructionTest)
{
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<IInspectable>&&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<IInspectable>&&>));

    ASSERT_TRUE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_constructible_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<IInspectable>&&>));

    dhorn::com::com_ptr<TestImplDerived> comPtr;
    comPtr.attach(new TestImplDerived());

    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(comPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRefCopy(std::move(derivedRef));
    ASSERT_TRUE(derivedRef.expired());
    ASSERT_TRUE(comPtr == derivedRefCopy.lock());

    dhorn::winrt::weak_ref<TestImpl> implRef(std::move(derivedRefCopy));
    ASSERT_TRUE(derivedRefCopy.expired());
    ASSERT_TRUE(comPtr == implRef.lock());

    dhorn::winrt::weak_ref<TestImpl> implRefCopy(std::move(implRef));
    ASSERT_TRUE(implRef.expired());
    ASSERT_TRUE(comPtr == implRefCopy.lock());

    dhorn::winrt::weak_ref<ITest> testRef(std::move(implRefCopy));
    ASSERT_TRUE(implRefCopy.expired());
    ASSERT_TRUE(comPtr == testRef.lock());

    dhorn::winrt::weak_ref<ITest> testRefCopy(std::move(testRef));
    ASSERT_TRUE(testRef.expired());
    ASSERT_TRUE(comPtr == testRefCopy.lock());

    // Move from null
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmpty;
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmptyCopy(std::move(derivedEmpty));
    ASSERT_TRUE(derivedEmptyCopy.expired());

    dhorn::winrt::weak_ref<TestImpl> implEmpty(std::move(derivedEmptyCopy));
    dhorn::winrt::weak_ref<TestImpl> implEmptyCopy(std::move(implEmpty));
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());

    dhorn::winrt::weak_ref<ITest> testEmpty(std::move(implEmptyCopy));
    dhorn::winrt::weak_ref<ITest> testEmptyCopy(std::move(testEmpty));
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());
}

TEST_F(WeakRefTests, CopyAssignmentTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, const dhorn::winrt::weak_ref<IInspectable>&>));

    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, const dhorn::winrt::weak_ref<IInspectable>&>));

    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<TestImplDerived>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<TestImpl>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<ITest>&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, const dhorn::winrt::weak_ref<IInspectable>&>));

    dhorn::com::com_ptr<TestImplDerived> comPtr;
    comPtr.attach(new TestImplDerived());

    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(comPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRefCopy;
    dhorn::winrt::weak_ref<TestImpl> implRef;
    dhorn::winrt::weak_ref<TestImpl> implRefCopy;
    dhorn::winrt::weak_ref<ITest> testRef;
    dhorn::winrt::weak_ref<ITest> testRefCopy;

    // Try and prevent optimizations
    ASSERT_TRUE(derivedRefCopy.expired());
    ASSERT_TRUE(implRef.expired());
    ASSERT_TRUE(implRefCopy.expired());
    ASSERT_TRUE(testRefCopy.expired());
    ASSERT_TRUE(testRef.expired());

    derivedRefCopy = derivedRef;
    ASSERT_TRUE(comPtr == derivedRef.lock());
    ASSERT_TRUE(comPtr == derivedRefCopy.lock());

    implRef = derivedRefCopy;
    ASSERT_TRUE(comPtr == derivedRefCopy.lock());
    ASSERT_TRUE(comPtr == implRef.lock());

    implRefCopy = implRef;
    ASSERT_TRUE(comPtr == implRef.lock());
    ASSERT_TRUE(comPtr == implRefCopy.lock());

    testRef = implRef;
    testRefCopy = testRef;
    ASSERT_TRUE(comPtr == testRef.lock());
    ASSERT_TRUE(comPtr == testRefCopy.lock());

    // Copy from null
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmpty;
    dhorn::winrt::weak_ref<TestImplDerived> derivedEmptyCopy;
    dhorn::winrt::weak_ref<TestImpl> implEmpty;
    dhorn::winrt::weak_ref<TestImpl> implEmptyCopy;
    dhorn::winrt::weak_ref<ITest> testEmpty;
    dhorn::winrt::weak_ref<ITest> testEmptyCopy;

    // Try and prevent optimizations
    ASSERT_TRUE(derivedEmptyCopy.expired());
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());

    derivedEmptyCopy = derivedEmpty;
    ASSERT_TRUE(derivedEmptyCopy.expired());

    implEmpty = derivedEmptyCopy;
    implEmptyCopy = implEmpty;
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());

    testEmpty = implEmptyCopy;
    testEmptyCopy = testEmpty;
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());
}

TEST_F(WeakRefTests, MoveAssignmentTest)
{
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<ITest>, dhorn::winrt::weak_ref<IInspectable>&&>));

    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImpl>, dhorn::winrt::weak_ref<IInspectable>&&>));

    ASSERT_TRUE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<TestImplDerived>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<TestImpl>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<ITest>&&>));
    ASSERT_FALSE((std::is_assignable_v<dhorn::winrt::weak_ref<TestImplDerived>, dhorn::winrt::weak_ref<IInspectable>&&>));

    dhorn::com::com_ptr<TestImplDerived> comPtr;
    comPtr.attach(new TestImplDerived());

    dhorn::winrt::weak_ref<TestImplDerived> derivedRef(comPtr);
    dhorn::winrt::weak_ref<TestImplDerived> derivedRefCopy;
    dhorn::winrt::weak_ref<TestImpl> implRef;
    dhorn::winrt::weak_ref<TestImpl> implRefCopy;
    dhorn::winrt::weak_ref<ITest> testRef;
    dhorn::winrt::weak_ref<ITest> testRefCopy;

    // Try and prevent optimizations
    ASSERT_TRUE(derivedRefCopy.expired());
    ASSERT_TRUE(implRef.expired());
    ASSERT_TRUE(implRefCopy.expired());
    ASSERT_TRUE(testRef.expired());
    ASSERT_TRUE(testRefCopy.expired());

    derivedRefCopy = std::move(derivedRef);
    ASSERT_TRUE(derivedRef.expired());
    ASSERT_TRUE(comPtr == derivedRefCopy.lock());

    implRef = std::move(derivedRefCopy);
    ASSERT_TRUE(derivedRefCopy.expired());
    ASSERT_TRUE(comPtr == implRef.lock());

    implRefCopy = std::move(implRef);
    ASSERT_TRUE(implRef.expired());
    ASSERT_TRUE(comPtr == implRefCopy.lock());

    testRef = std::move(implRefCopy);
    ASSERT_TRUE(implRefCopy.expired());
    ASSERT_TRUE(comPtr == testRef.lock());

    testRefCopy = std::move(testRef);
    ASSERT_TRUE(testRef.expired());
    ASSERT_TRUE(comPtr == testRefCopy.lock());

    // Move from null
    dhorn::winrt::weak_ref<TestImpl> derivedEmpty;
    dhorn::winrt::weak_ref<TestImpl> derivedEmptyCopy;
    dhorn::winrt::weak_ref<TestImpl> implEmpty;
    dhorn::winrt::weak_ref<TestImpl> implEmptyCopy;
    dhorn::winrt::weak_ref<ITest> testEmpty;
    dhorn::winrt::weak_ref<ITest> testEmptyCopy;

    // Try and prevent optimizations
    ASSERT_TRUE(derivedEmptyCopy.expired());
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());

    derivedRefCopy = std::move(derivedRef);
    ASSERT_TRUE(derivedRefCopy.expired());

    implEmpty = std::move(derivedRefCopy);
    implEmptyCopy = std::move(implEmpty);
    ASSERT_TRUE(implEmpty.expired());
    ASSERT_TRUE(implEmptyCopy.expired());

    testEmpty = std::move(implEmptyCopy);
    testEmptyCopy = std::move(testEmpty);
    ASSERT_TRUE(testEmpty.expired());
    ASSERT_TRUE(testEmptyCopy.expired());
}

TEST_F(WeakRefTests, ResetTest)
{
    dhorn::com::com_ptr<TestImpl> comPtr;
    comPtr.attach(new TestImpl());

    dhorn::winrt::weak_ref<TestImpl> implRef(comPtr);
    dhorn::winrt::weak_ref<ITest> testRef(comPtr);

    ASSERT_FALSE(implRef.expired());
    ASSERT_FALSE(testRef.expired());

    implRef.reset();
    testRef.reset();

    ASSERT_TRUE(implRef.expired());
    ASSERT_TRUE(testRef.expired());
    ASSERT_EQ(nullptr, implRef.lock().get());
    ASSERT_EQ(nullptr, testRef.lock().get());
}

TEST_F(WeakRefTests, SwapTest)
{
    dhorn::com::com_ptr<TestImpl> comPtr;
    comPtr.attach(new TestImpl());

    dhorn::winrt::weak_ref<TestImpl> implRef1(comPtr);
    dhorn::winrt::weak_ref<TestImpl> implRef2;
    implRef1.swap(implRef2);
    ASSERT_TRUE(implRef1.expired());
    ASSERT_FALSE(implRef2.expired());
    ASSERT_TRUE(comPtr == implRef2.lock());

    dhorn::winrt::weak_ref<ITest> testRef1(comPtr);
    dhorn::winrt::weak_ref<ITest> testRef2;
    testRef1.swap(testRef2);
    ASSERT_TRUE(testRef1.expired());
    ASSERT_FALSE(testRef2.expired());
    ASSERT_TRUE(comPtr == testRef2.lock());
}

TEST_F(WeakRefTests, SizeTests)
{
    ASSERT_EQ(sizeof(void*), sizeof(dhorn::winrt::weak_ref<ITest>));
    ASSERT_EQ(sizeof(void*) * 2, sizeof(dhorn::winrt::weak_ref<TestImpl>));
}

TEST_F(WeakRefTests, ResolveNonDestroyedTest)
{
    dhorn::com::com_ptr<TestImpl> test;
    test.attach(new TestImpl());

    dhorn::winrt::weak_ref<TestImpl> weak(test);
    ASSERT_FALSE(weak.expired());
    ASSERT_TRUE(weak.lock() == test.get());

    dhorn::winrt::weak_ref<ITest> weak2(test);
    ASSERT_FALSE(weak2.expired());
    ASSERT_TRUE(weak2.lock() == test.get());

    ASSERT_EQ(1ul, test->RefCount());

    auto ptr = weak.lock();
    ASSERT_EQ(2ul, test->RefCount());

    auto ptr2 = weak2.lock();
    ASSERT_EQ(3ul, test->RefCount());
}

TEST_F(WeakRefTests, ResolveDestroyedTest)
{
    dhorn::winrt::weak_ref<TestImpl> weak;
    dhorn::winrt::weak_ref<ITest> weak2;
    {
        dhorn::com::com_ptr<TestImpl> test;
        test.attach(new TestImpl());

        weak = dhorn::winrt::as_weak(test);
        weak2 = weak;
    }

    ASSERT_TRUE(weak.expired());
    ASSERT_TRUE(weak.lock() == nullptr);

    ASSERT_TRUE(weak2.expired());
    ASSERT_TRUE(weak2.lock() == nullptr);
}
