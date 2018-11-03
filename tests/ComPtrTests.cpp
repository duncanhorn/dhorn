/*
 * Duncan Horn
 *
 * ComPtrTests.cpp
 *
 * Tests for the throwing dhorn::com::com_ptr
 */

#include <dhorn/com/com_ptr.h>
#include <gtest/gtest.h>

__interface __declspec(uuid("{56C9CB0F-534C-42D5-B297-9D77E71D908C}"))
IBase : public IUnknown
{
    STDMETHOD_(ULONG, RefCount)();
    STDMETHOD_(ULONG, AddRefCount)();
    STDMETHOD_(ULONG, QueryCount)();
};

__interface __declspec(uuid("{FAB826E0-BB52-4CD5-83B1-C401D5386568}"))
IFoo : public IBase
{
};

__interface __declspec(uuid("{D8C6769D-72E5-4CF0-B65F-348C40044EAA}"))
IBar : public IBase
{
};

class Base :
    public IBase
{
public:
    Base()
    {
    }

    virtual ~Base()
    {
        assert(count == 0);
    }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        if (riid == __uuidof(IUnknown))
        {
            *ppvObject = static_cast<IUnknown *>(this);
            this->AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IBase))
        {
            *ppvObject = static_cast<IBase *>(this);
            this->AddRef();
            return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        if (count <= 0)
        {
            return E_NOT_VALID_STATE;
        }

        ++addRefCount;
        return ++count;
    }

    STDMETHOD_(ULONG, Release)()
    {
        auto val = --count;

        if (val == 0)
        {
            delete this;
        }

        return val;
    }

    // IBase
    STDMETHOD_(ULONG, RefCount)()
    {
        return count;
    }

    STDMETHOD_(ULONG, AddRefCount)()
    {
        return addRefCount;
    }

    // Tests run on single thread, so no need to synchronize
    ULONG count = 1;
    ULONG addRefCount = 0;
};

class Foo :
    public Base,
    public IFoo
{
public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        ++this->_queryCount;

        if (riid == __uuidof(IFoo))
        {
            *ppvObject = static_cast<IFoo *>(this);
            this->AddRef();
            return S_OK;
        }

        return Base::QueryInterface(riid, ppvObject);
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        return Base::AddRef();
    }

    STDMETHOD_(ULONG, Release)()
    {
        return Base::Release();
    }

    // IBase
    STDMETHOD_(ULONG, RefCount)()
    {
        return Base::RefCount();
    }

    STDMETHOD_(ULONG, AddRefCount)()
    {
        return Base::AddRefCount();
    }

    STDMETHOD_(ULONG, QueryCount)()
    {
        return this->_queryCount;
    }

private:

    ULONG _queryCount = 0;
};

class Bar :
    public Base,
    public IBar
{
public:

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        ++this->_queryCount;

        if (riid == __uuidof(IBar))
        {
            *ppvObject = static_cast<IBar *>(this);
            this->AddRef();
            return S_OK;
        }

        return Base::QueryInterface(riid, ppvObject);
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        return Base::AddRef();
    }

    STDMETHOD_(ULONG, Release)()
    {
        return Base::Release();
    }

    // IBase
    STDMETHOD_(ULONG, RefCount)()
    {
        return Base::RefCount();
    }

    STDMETHOD_(ULONG, AddRefCount)()
    {
        return Base::AddRefCount();
    }

    STDMETHOD_(ULONG, QueryCount)()
    {
        return this->_queryCount;
    }

private:

    ULONG _queryCount = 0;
};

class FooBar :
    public Base,
    public IFoo,
    public IBar
{
public:

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        ++this->_queryCount;

        if (riid == __uuidof(IFoo))
        {
            *ppvObject = static_cast<IFoo *>(this);
            this->AddRef();
            return S_OK;
        }
        else if (riid == __uuidof(IBar))
        {
            *ppvObject = static_cast<IBar *>(this);
            this->AddRef();
            return S_OK;
        }

        return Base::QueryInterface(riid, ppvObject);
    }

    STDMETHOD_(ULONG, AddRef)()
    {
        return Base::AddRef();
    }

    STDMETHOD_(ULONG, Release)()
    {
        return Base::Release();
    }

    // IBase
    STDMETHOD_(ULONG, RefCount)()
    {
        return Base::RefCount();
    }

    STDMETHOD_(ULONG, AddRefCount)()
    {
        return Base::AddRefCount();
    }

    STDMETHOD_(ULONG, QueryCount)()
    {
        return this->_queryCount;
    }

private:

    ULONG _queryCount = 0;
};

// Useful aliases for tests
using foo_ptr = dhorn::com::com_ptr<Foo>;
using bar_ptr = dhorn::com::com_ptr<Bar>;
using foobar_ptr = dhorn::com::com_ptr<FooBar>;

using foo_interface_ptr = dhorn::com::com_ptr<IFoo>;
using bar_interface_ptr = dhorn::com::com_ptr<IBar>;
using base_interface_ptr = dhorn::com::com_ptr<IBase>;
using unknown_interface_ptr = dhorn::com::com_ptr<IUnknown>;

// Helper Method For Setup, etc.
template <typename Func>
static void DoTest(Func&& func)
{
    auto foo = new Foo();
    auto bar = new Bar();
    auto foobar = new FooBar();

    func(foo, bar, foobar);

    ASSERT_EQ(1ul, foo->RefCount());
    ASSERT_EQ(1ul, bar->RefCount());
    ASSERT_EQ(1ul, foobar->RefCount());

    foo->Release();
    bar->Release();
    foobar->Release();
}

template <typename TestTy>
static void DoPointerTest(const TestTy& test)
{
    DoTest([&](Foo* foo, Bar* bar, FooBar* foobar)
    {
        // Template arguments: To, From, InputPointer
        test.template run<Foo>(foo);
        test.template run<Bar>(bar);
        test.template run<FooBar>(foobar);

        test.template run<IFoo>(foo);
        test.template run<IBar>(bar);
        test.template run<IBase, IBase, IFoo>(foobar);
        test.template run<IUnknown, IUnknown, IBar>(foobar);
    });
}

template <typename TestTy>
static void DoDownCastPointerTest(const TestTy& test)
{
    DoTest([&](Foo* foo, Bar* bar, FooBar* foobar)
    {
        // Template arguments: To, From, InputPointer
        test.template run<IFoo, Foo>(foo);
        test.template run<IBar, Bar>(bar);
        test.template run<IFoo, FooBar>(foobar);
        test.template run<IBar, FooBar>(foobar);

        test.template run<IBase, IFoo>(foo);
        test.template run<IBase, IBar>(bar);
        test.template run<IUnknown, IFoo, IFoo>(foobar);
        test.template run<IUnknown, IBar, IBar>(foobar);
        test.template run<IUnknown, IBase, IFoo>(foobar);
    });
}

template <typename TestTy>
static void DoPointerTraitsTest(const TestTy& test)
{
    // Template arguments: To, From
    test.template run<Foo>();
    test.template run<Bar>();
    test.template run<FooBar>();

    test.template run<IFoo>();
    test.template run<IBar>();
    test.template run<IBase>();
    test.template run<IUnknown>();
}

template <typename TestTy>
static void DoDownCastPointerTraitsTest(const TestTy& test)
{
    // Template arguments: To, From
    test.template run<IFoo, Foo>();
    test.template run<IBar, Bar>();
    test.template run<IFoo, FooBar>();
    test.template run<IBar, FooBar>();

    test.template run<IBase, IFoo>();
    test.template run<IBase, IBar>();
    test.template run<IUnknown, IFoo>();
    test.template run<IUnknown, IBar>();
    test.template run<IUnknown, IBase>();
}

// Null/Default Construction
TEST(ComPtrTests, DefaultAndNullConstructionTest)
{
    dhorn::com::com_ptr<IUnknown> unk;
    dhorn::com::com_ptr<IBase> base;
    dhorn::com::com_ptr<IFoo> foo(nullptr);
    dhorn::com::com_ptr<IBar> bar(nullptr);
    dhorn::com::com_ptr<FooBar> foobar(nullptr);

    ASSERT_FALSE(unk);
    ASSERT_FALSE(base);
    ASSERT_FALSE(foo);
    ASSERT_FALSE(bar);
    ASSERT_FALSE(foobar);
}

TEST(ComPtrTests, DefaultAndNullConstructionNoExceptTest)
{
    ASSERT_TRUE(std::is_nothrow_default_constructible_v<foo_ptr>);
    ASSERT_TRUE((std::is_nothrow_constructible_v<foo_ptr, std::nullptr_t>));
}

// Pointer Construction
struct PointerConstructionTester
{
    template <typename Ty, typename CastTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<Ty> comPtr(static_cast<CastTy*>(ptr));
        ASSERT_EQ(2ul, ptr->RefCount());

        // Construction should add ref with no QIs
        ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, PointerConstructionTest)
{
    DoPointerTest(PointerConstructionTester{});
}

TEST(ComPtrTests, DownCastPointerConstructionTest)
{
    DoDownCastPointerTest(PointerConstructionTester{});
}

struct NullPointerConstructionTester
{
    template <typename ComPtrTy, typename PtrTy = ComPtrTy>
    void run() const
    {
        PtrTy* ptr = nullptr;
        dhorn::com::com_ptr<ComPtrTy> comPtr(ptr);
        ASSERT_FALSE(comPtr);
    }
};

TEST(ComPtrTests, NullPointerConstructionTest)
{
    DoPointerTraitsTest(NullPointerConstructionTester{});
}

TEST(ComPtrTests, NullDownCastPointerConstructionTest)
{
    DoDownCastPointerTraitsTest(NullPointerConstructionTester{});
}

struct PointerConstructionNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, FromTy*>));
    }
};

TEST(ComPtrTests, PointerConstructionNoExceptTest)
{
    DoPointerTraitsTest(PointerConstructionNoExceptTester{});
}

TEST(ComPtrTests, DownCastPointerConstructionNoExceptTest)
{
    DoDownCastPointerTraitsTest(PointerConstructionNoExceptTester{});
}

// Copy Construction
struct CopyConstructionTester
{
    template <typename Ty, typename OtherTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<OtherTy> comPtr(ptr);
        dhorn::com::com_ptr<Ty> copyPtr(comPtr);
        ASSERT_EQ(3ul, ptr->RefCount());

        // Construction should add ref with no QIs
        ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, CopyConstructionTest)
{
    DoPointerTest(CopyConstructionTester{});
}

TEST(ComPtrTests, DownCastCopyConstructionTest)
{
    DoDownCastPointerTest(CopyConstructionTester{});
}

struct NullCopyConstructionTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        dhorn::com::com_ptr<FromTy> ptr;
        dhorn::com::com_ptr<Ty> ptrCopy(ptr);
        ASSERT_FALSE(ptrCopy);
    }
};

TEST(ComPtrTests, NullCopyConstructionTest)
{
    DoPointerTraitsTest(NullCopyConstructionTester{});
}

TEST(ComPtrTests, NullDownCastCopyConstructionTest)
{
    DoDownCastPointerTraitsTest(NullCopyConstructionTester{});
}

struct CopyConstructionNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>>));
    }
};

TEST(ComPtrTests, CopyConstructionNoExceptTest)
{
    DoPointerTraitsTest(CopyConstructionNoExceptTester{});
}

TEST(ComPtrTests, DownCastCopyConstructionNoExceptTest)
{
    DoDownCastPointerTraitsTest(CopyConstructionNoExceptTester{});
}

// Move Construction
struct MoveConstructionTester
{
    template <typename Ty, typename OtherTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<OtherTy> comPtr(ptr);
        dhorn::com::com_ptr<Ty> movedPtr(std::move(comPtr));

        ASSERT_EQ(2ul, ptr->RefCount());
        ASSERT_FALSE(comPtr);
        ASSERT_TRUE(movedPtr);

        // One AddRef for the moved-from, but not for the moved-to. No QIs
        ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, MoveConstructionTest)
{
    DoPointerTest(MoveConstructionTester{});
}

TEST(ComPtrTests, MoveDownCastConstructionTest)
{
    DoDownCastPointerTest(MoveConstructionTester{});
}

struct NullMoveConstructionTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        dhorn::com::com_ptr<FromTy> ptr;
        dhorn::com::com_ptr<Ty> ptrMove(std::move(ptr));
        ASSERT_FALSE(ptrMove);
    }
};

TEST(ComPtrTests, NullMoveConstructionTest)
{
    DoPointerTraitsTest(NullMoveConstructionTester{});
}

TEST(ComPtrTests, NullDownCastMoveConstructionTest)
{
    DoDownCastPointerTraitsTest(NullMoveConstructionTester{});
}

struct MoveConstructionNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>&&>));
    }
};

TEST(ComPtrTests, MoveConstructionNoExceptTest)
{
    DoPointerTraitsTest(MoveConstructionNoExceptTester{});
}

TEST(ComPtrTests, MoveDownCastConstructionNoExceptTest)
{
    DoDownCastPointerTraitsTest(MoveConstructionNoExceptTester{});
}

// Null Assignment
template <typename Ty, typename PtrTy>
static void NullAssignmentTest_Impl(PtrTy* ptr)
{
    dhorn::com::com_ptr<Ty> comPtr(ptr);
    comPtr = nullptr;
    ASSERT_FALSE(comPtr);
}

TEST(ComPtrTests, NullAssignmentTest)
{
    DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
    {
        NullAssignmentTest_Impl<Foo>(foo);
        NullAssignmentTest_Impl<Bar>(bar);
        NullAssignmentTest_Impl<FooBar>(foobar);

        NullAssignmentTest_Impl<IFoo>(foo);
        NullAssignmentTest_Impl<IBar>(bar);
        NullAssignmentTest_Impl<IBase, IFoo>(foobar);
        NullAssignmentTest_Impl<IUnknown, IBar>(foobar);
    });
}

TEST(ComPtrTests, NullAssignmentNoExceptTest)
{
    ASSERT_TRUE((std::is_nothrow_assignable_v<foo_ptr, std::nullptr_t>));
}

// Pointer Assignment
struct PointerAssignmentTester
{
    template <typename Ty, typename CastTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<Ty> comPtr;
        ASSERT_FALSE(comPtr);

        comPtr = static_cast<CastTy*>(ptr);
        ASSERT_TRUE(comPtr);
        ASSERT_EQ(2ul, ptr->RefCount());

        // Construction should add ref with no QIs
        ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, PointerAssignmentTest)
{
    DoPointerTest(PointerAssignmentTester{});
}

TEST(ComPtrTests, DownCastPointerAssignmentTest)
{
    DoDownCastPointerTest(PointerAssignmentTester{});
}

struct NullPointerAssignmentTester
{
    template <typename ComPtrTy, typename PtrTy = ComPtrTy>
    void run() const
    {
        dhorn::com::com_ptr<ComPtrTy> comPtr;
        ASSERT_FALSE(comPtr);

        PtrTy* ptr = nullptr;
        comPtr = ptr;
        ASSERT_FALSE(comPtr);
    }
};

TEST(ComPtrTests, NullPointerAssignmentTest)
{
    DoPointerTraitsTest(NullPointerAssignmentTester{});
}

TEST(ComPtrTests, NullDownCastPointerAssignmentTest)
{
    DoDownCastPointerTraitsTest(NullPointerAssignmentTester{});
}

struct PointerAssignmentNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, FromTy*>));
    }
};

TEST(ComPtrTests, PointerAssignmentNoExceptTest)
{
    DoPointerTraitsTest(PointerAssignmentNoExceptTester{});
}

TEST(ComPtrTests, DownCastPointerAssignmentNoExceptTest)
{
    DoDownCastPointerTraitsTest(PointerAssignmentNoExceptTester{});
}

// Copy Assignment
struct CopyAssignmentTester
{
    template <typename Ty, typename OtherTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<OtherTy> comPtr(ptr);
        dhorn::com::com_ptr<Ty> copyPtr;
        ASSERT_FALSE(copyPtr);

        copyPtr = comPtr;
        ASSERT_TRUE(comPtr);
        ASSERT_EQ(3ul, ptr->RefCount());

        // Construction should add ref with no QIs
        ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, CopyAssignmentTest)
{
    DoPointerTest(CopyAssignmentTester{});
}

TEST(ComPtrTests, DownCastCopyAssignmentTest)
{
    DoDownCastPointerTest(CopyAssignmentTester{});
}

struct NullCopyAssignmentTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        dhorn::com::com_ptr<FromTy> ptr;
        dhorn::com::com_ptr<Ty> comPtr;
        ASSERT_FALSE(comPtr);

        comPtr = ptr;
        ASSERT_FALSE(comPtr);
    }
};

TEST(ComPtrTests, NullCopyAssignmentTest)
{
    DoPointerTraitsTest(NullCopyAssignmentTester{});
}

TEST(ComPtrTests, NullDownCastCopyAssignmentTest)
{
    DoDownCastPointerTraitsTest(NullCopyAssignmentTester{});
}

struct CopyAssignmentNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>>));
    }
};

TEST(ComPtrTests, CopyAssignmentNoExceptTest)
{
    DoPointerTraitsTest(CopyAssignmentNoExceptTester{});
}

TEST(ComPtrTests, DownCastCopyAssignmentNoExceptTest)
{
    DoDownCastPointerTraitsTest(CopyAssignmentNoExceptTester{});
}

// Move Assignment
struct MoveAssignmentTester
{
    template <typename Ty, typename OtherTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<OtherTy> comPtr(ptr);
        dhorn::com::com_ptr<Ty> movedPtr;
        ASSERT_FALSE(movedPtr);

        movedPtr = std::move(comPtr);
        ASSERT_EQ(2ul, ptr->RefCount());
        ASSERT_FALSE(comPtr);
        ASSERT_TRUE(movedPtr);

        // AddRef for the moved-from, but not the moved-to; no QIs
        ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, MoveAssignmentTest)
{
    DoPointerTest(MoveAssignmentTester{});
}

TEST(ComPtrTests, DownCastMoveAssignmentTest)
{
    DoDownCastPointerTest(MoveAssignmentTester{});
}

struct NullMoveAssignmentTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        dhorn::com::com_ptr<FromTy> ptr;
        dhorn::com::com_ptr<Ty> ptrMove;
        ASSERT_FALSE(ptrMove);

        ptrMove = std::move(ptr);
        ASSERT_FALSE(ptrMove);
    }
};

TEST(ComPtrTests, NullMoveAssignmentTest)
{
    DoPointerTraitsTest(NullMoveAssignmentTester{});
}

TEST(ComPtrTests, NullDownCastMoveAssignmentTest)
{
    DoDownCastPointerTraitsTest(NullMoveAssignmentTester{});
}

struct MoveAssignmentNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE((std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>&&>));
    }
};

TEST(ComPtrTests, MoveAssignmentNoExceptTest)
{
    DoPointerTraitsTest(MoveAssignmentNoExceptTester{});
}

TEST(ComPtrTests, DownCastMoveAssignmentNoExceptTest)
{
    DoDownCastPointerTraitsTest(MoveAssignmentNoExceptTester{});
}

// Self Assignment
TEST(ComPtrTests, SelfAssignmentTest)
{
    auto foobar = new FooBar();
    {
        // Easier to spot errors if the ref count is 1
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        foobar->Release();
        ASSERT_EQ(1ul, foobar->RefCount());

        comPtr = comPtr;
        ASSERT_EQ(1ul, foobar->RefCount());

        foobar->AddRef();
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, NullResetTest)
{
    auto foobar = new FooBar();

    dhorn::com::com_ptr<FooBar> comPtr(foobar);
    ASSERT_EQ(2ul, foobar->RefCount());

    comPtr.reset();
    ASSERT_FALSE(comPtr);
    ASSERT_EQ(1ul, foobar->RefCount());

    comPtr = foobar;
    ASSERT_EQ(2ul, foobar->RefCount());

    comPtr.reset(nullptr);
    ASSERT_FALSE(comPtr);
    ASSERT_EQ(1ul, foobar->RefCount());

    // Calling reset when null should be fine
    comPtr.reset();
    ASSERT_FALSE(comPtr);

    // Null non-std::nullptr_t should also be fine
    comPtr.reset(static_cast<FooBar*>(nullptr));
    ASSERT_FALSE(comPtr);

    foobar->Release();
}

TEST(ComPtrTests, NullResetNoExceptTest)
{
    dhorn::com::com_ptr<IFoo> comPtr;
    ASSERT_TRUE(noexcept(comPtr.reset()));
    ASSERT_TRUE(noexcept(comPtr.reset(nullptr)));
}

struct ResetTester
{
    template <typename Ty, typename CastTy = Ty, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<Ty> comPtr;
        ASSERT_FALSE(comPtr);

        comPtr.reset(static_cast<CastTy*>(ptr));
        ASSERT_TRUE(comPtr);
        ASSERT_EQ(2ul, ptr->RefCount());

        comPtr.reset(static_cast<CastTy*>(ptr));
        ASSERT_TRUE(comPtr);
        ASSERT_EQ(2ul, ptr->RefCount());

        // One AddRef for each reset; no QIs
        ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
        ASSERT_EQ(qiCount, ptr->QueryCount());
    }
};

TEST(ComPtrTests, ResetTest)
{
    DoPointerTest(ResetTester{});
}

TEST(ComPtrTests, DownCastResetTest)
{
    DoDownCastPointerTest(ResetTester{});
}

struct ResetNoExceptTester
{
    template <typename Ty, typename FromTy = Ty>
    void run() const
    {
        ASSERT_TRUE(noexcept(std::declval<dhorn::com::com_ptr<Ty>>().reset(std::declval<FromTy*>())));
    }
};

TEST(ComPtrTests, ResetNoExceptTest)
{
    DoPointerTraitsTest(ResetNoExceptTester{});
}

TEST(ComPtrTests, DownCastResetNoExceptTest)
{
    DoDownCastPointerTraitsTest(ResetNoExceptTester{});
}

TEST(ComPtrTests, DereferenceOperatorTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, (*comPtr).RefCount());
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, OperatorArrowTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, comPtr->RefCount());
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, AddressOfOperatorTest)
{
    auto foobar = new FooBar();

    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, comPtr->RefCount());

        // operator& should release
        auto ptr = &comPtr;
        ASSERT_FALSE(comPtr);
        ASSERT_EQ(1ul, foobar->RefCount());
        ASSERT_TRUE(*ptr == nullptr);

        ASSERT_TRUE((std::is_same_v<decltype(ptr), FooBar**>));
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, AttachTest)
{
    auto foobar = new FooBar();

    dhorn::com::com_ptr<FooBar> comPtr;

    // Attach should not AddRef
    comPtr.attach(foobar);
    ASSERT_TRUE(comPtr);
    ASSERT_EQ(1ul, foobar->RefCount());

    foobar->AddRef();
    ASSERT_EQ(2ul, foobar->RefCount());

    // Calling attach again should Release, but not AddRef the input
    comPtr.attach(foobar);
    ASSERT_EQ(1ul, foobar->RefCount());
}

TEST(ComPtrTests, DetachTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, foobar->RefCount());

        auto ptr = comPtr.detach();
        ASSERT_TRUE(ptr == foobar);
        ASSERT_FALSE(comPtr);
    }

    ASSERT_EQ(2ul, foobar->RefCount());
    foobar->Release();
    foobar->Release();
}

TEST(ComPtrTests, SwapTest)
{
    IFoo* foo = new Foo();
    IBar* bar = new Bar();

    {
        dhorn::com::com_ptr<IBase> ptr1(foo);
        {
            dhorn::com::com_ptr<IBase> ptr2(bar);

            ptr1.swap(ptr2);
            ASSERT_EQ(2ul, foo->RefCount());
            ASSERT_EQ(2ul, bar->RefCount());

            ASSERT_TRUE(ptr1.get() == bar);
            ASSERT_TRUE(ptr2.get() == foo);
        }

        ASSERT_EQ(1ul, foo->RefCount());
        ASSERT_EQ(2ul, bar->RefCount());
    }

    ASSERT_EQ(1ul, foo->RefCount());
    ASSERT_EQ(1ul, bar->RefCount());
    foo->Release();
    bar->Release();
}

TEST(ComPtrTests, SwapSelfTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, foobar->RefCount());

        comPtr.swap(comPtr);
        ASSERT_EQ(2ul, foobar->RefCount());
        ASSERT_TRUE(comPtr.get() == foobar);
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, ReleaseAndGetAddressOfTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, comPtr->RefCount());

        auto ptr = comPtr.release_and_get_address_of();
        ASSERT_FALSE(comPtr);
        ASSERT_EQ(1ul, foobar->RefCount());
        ASSERT_TRUE(*ptr == nullptr);

        ASSERT_TRUE((std::is_same_v<decltype(ptr), FooBar**>));
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

TEST(ComPtrTests, AddressOfTest)
{
    auto foobar = new FooBar();
    {
        dhorn::com::com_ptr<FooBar> comPtr(foobar);
        ASSERT_EQ(2ul, foobar->RefCount());

        // address_of does _not_ release
        auto ptr = comPtr.address_of();
        ASSERT_TRUE(comPtr);
        ASSERT_EQ(2ul, foobar->RefCount());
        ASSERT_TRUE(*ptr == foobar);
        ASSERT_TRUE(*ptr == comPtr.get());

        ASSERT_TRUE((std::is_same_v<decltype(ptr), FooBar**>));
    }

    ASSERT_EQ(1ul, foobar->RefCount());
    foobar->Release();
}

template <typename TestTy>
static void DoConversionTest(const TestTy& test)
{
    DoTest([&](Foo* foo, Bar* bar, FooBar* foobar)
    {
        // FooBar: IUnknown -> IFace
        test.template run<true, IUnknown, IUnknown, IFoo>(foobar);
        test.template run<true, IBase, IUnknown, IFoo>(foobar);
        test.template run<true, IFoo, IUnknown, IFoo>(foobar);
        test.template run<true, IBar, IUnknown, IFoo>(foobar);

        // FooBar: IBase -> IFace
        test.template run<true, IUnknown, IBase, IFoo>(foobar);
        test.template run<true, IBase, IBase, IFoo>(foobar);
        test.template run<true, IFoo, IBase, IFoo>(foobar);
        test.template run<true, IBar, IBase, IFoo>(foobar);

        // FooBar: IFoo -> IFace
        test.template run<true, IUnknown, IFoo>(foobar);
        test.template run<true, IBase, IFoo>(foobar);
        test.template run<true, IFoo, IFoo>(foobar);
        test.template run<true, IBar, IFoo>(foobar);

        // FooBar: IBar -> IFace
        test.template run<true, IUnknown, IBar>(foobar);
        test.template run<true, IBase, IBar>(foobar);
        test.template run<true, IFoo, IBar>(foobar);
        test.template run<true, IBar, IBar>(foobar);

        // Foo: IUnknown -> IFace
        test.template run<true, IUnknown, IUnknown, IFoo>(foo);
        test.template run<true, IBase, IUnknown, IFoo>(foo);
        test.template run<true, IFoo, IUnknown, IFoo>(foo);
        test.template run<false, IBar, IUnknown, IFoo>(foo);

        // Foo: IBase -> IFace
        test.template run<true, IUnknown, IBase, IFoo>(foo);
        test.template run<true, IBase, IBase, IFoo>(foo);
        test.template run<true, IFoo, IBase, IFoo>(foo);
        test.template run<false, IBar, IBase, IFoo>(foo);

        // Foo: IFoo -> IFace
        test.template run<true, IUnknown, IFoo>(foo);
        test.template run<true, IBase, IFoo>(foo);
        test.template run<true, IFoo, IFoo>(foo);
        test.template run<false, IBar, IFoo>(foo);

        // Bar: IUnknown -> IFace
        test.template run<true, IUnknown, IUnknown, IBar>(bar);
        test.template run<true, IBase, IUnknown, IBar>(bar);
        test.template run<false, IFoo, IUnknown, IBar>(bar);
        test.template run<true, IBar, IUnknown, IBar>(bar);

        // Bar: IBase -> IFace
        test.template run<true, IUnknown, IBase, IBar>(bar);
        test.template run<true, IBase, IBase, IBar>(bar);
        test.template run<false, IFoo, IBase, IBar>(bar);
        test.template run<true, IBar, IBase, IBar>(bar);

        // Bar: IBar -> IFace
        test.template run<true, IUnknown, IBar>(bar);
        test.template run<true, IBase, IBar>(bar);
        test.template run<false, IFoo, IBar>(bar);
        test.template run<true, IBar, IBar>(bar);
    });
}

template <typename TestTy>
static void DoNoExceptConversionTest(const TestTy& test)
{
    // FooBar -> IFace
    test.template run<true, foobar_ptr, IFoo>();
    test.template run<true, foobar_ptr, IBar>();
    test.template run<false, foobar_ptr, IBase>();
    test.template run<false, foobar_ptr, IUnknown>();

    // Foo -> IFace
    test.template run<true, foo_ptr, IFoo>();
    test.template run<false, foo_ptr, IBar>();
    test.template run<false, foo_ptr, IBase>();
    test.template run<false, foo_ptr, IUnknown>();

    // Bar -> IFace
    test.template run<false, bar_ptr, IFoo>();
    test.template run<true, bar_ptr, IBar>();
    test.template run<false, bar_ptr, IBase>();
    test.template run<false, bar_ptr, IUnknown>();

    // IFoo -> IFace
    test.template run<true, foo_interface_ptr, IFoo>();
    test.template run<false, foo_interface_ptr, IBar>();
    test.template run<true, foo_interface_ptr, IBase>();
    test.template run<true, foo_interface_ptr, IUnknown>();

    // IBar -> IFace
    test.template run<false, bar_interface_ptr, IFoo>();
    test.template run<true, bar_interface_ptr, IBar>();
    test.template run<true, bar_interface_ptr, IBase>();
    test.template run<true, bar_interface_ptr, IUnknown>();

    // IBase -> IFace
    test.template run<false, base_interface_ptr, IFoo>();
    test.template run<false, base_interface_ptr, IBar>();
    test.template run<true, base_interface_ptr, IBase>();
    test.template run<true, base_interface_ptr, IUnknown>();

    // IUnknown -> IFace
    test.template run<false, unknown_interface_ptr, IFoo>();
    test.template run<false, unknown_interface_ptr, IBar>();
    test.template run<false, unknown_interface_ptr, IBase>();
    test.template run<true, unknown_interface_ptr, IUnknown>();
}

struct AsTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();
        constexpr bool shouldQI = !std::is_convertible_v<FromTy*, Ty*>;

        dhorn::com::com_ptr<FromTy> fromPtr(static_cast<CastTy*>(ptr));
        try
        {
            auto comPtr = fromPtr.template as<Ty>();
            ASSERT_TRUE(ShouldSucceed);

            ASSERT_EQ(3ul, ptr->RefCount());

            // Query succeeded. Two AddRefs and QI only if needed
            ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
            ASSERT_EQ(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
        }
        catch (std::system_error& e)
        {
            ASSERT_FALSE(ShouldSucceed);
            ASSERT_FALSE(e.code().category() != dhorn::com::hresult_category());

            ASSERT_EQ(static_cast<int>(E_NOINTERFACE), e.code().value());

            // Query failed. One (initial) AddRef, but should have attempted QI
            ASSERT_TRUE(shouldQI);
            ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
            ASSERT_EQ(qiCount + 1, ptr->QueryCount());
        }

        ASSERT_EQ(2ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, AsTest)
{
    DoConversionTest(AsTester{});
}

struct AsNoExceptTester
{
    template <bool NoExcept, typename FromTy, typename ToTy>
    void run() const
    {
        ASSERT_EQ(NoExcept, noexcept(std::declval<FromTy>().template as<ToTy>()));
    }
};

TEST(ComPtrTests, AsNoExceptTest)
{
    DoNoExceptConversionTest(AsNoExceptTester{});
}

struct TryAsTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();
        constexpr bool shouldQI = !std::is_convertible_v<FromTy*, Ty*>;

        dhorn::com::com_ptr<FromTy> fromPtr(static_cast<CastTy*>(ptr));
        {
            auto comPtr = fromPtr.template try_as<Ty>();

            if (ShouldSucceed)
            {
                ASSERT_EQ(3ul, ptr->RefCount());
                ASSERT_TRUE(comPtr);

                // Query succeeded. Two AddRefs and QI only if needed
                ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
                ASSERT_EQ(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
            }
            else
            {
                ASSERT_EQ(2ul, ptr->RefCount());
                ASSERT_FALSE(comPtr);

                // Query failed. One (initial) AddRef, but should have attempted QI
                ASSERT_TRUE(shouldQI);
                ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
                ASSERT_EQ(qiCount + 1, ptr->QueryCount());
            }
        }

        ASSERT_EQ(2ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, TryAsTest)
{
    DoConversionTest(TryAsTester{});
}

struct CopyToTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();
        constexpr bool shouldQI = !std::is_convertible_v<FromTy*, Ty*>;

        dhorn::com::com_ptr<FromTy> fromPtr(static_cast<CastTy*>(ptr));
        try
        {
            Ty* rawPtr;
            fromPtr.copy_to(&rawPtr);
            ASSERT_TRUE(ShouldSucceed);

            ASSERT_EQ(3ul, ptr->RefCount());
            rawPtr->Release();

            // Query succeeded. Two AddRefs and QI only if needed
            ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
            ASSERT_EQ(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
        }
        catch (std::system_error& e)
        {
            ASSERT_FALSE(ShouldSucceed);
            ASSERT_FALSE(e.code().category() != dhorn::com::hresult_category());

            ASSERT_EQ(static_cast<int>(E_NOINTERFACE), e.code().value());

            // Query failed. One (initial) AddRef, but should have attempted QI
            ASSERT_TRUE(shouldQI);
            ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
            ASSERT_EQ(qiCount + 1, ptr->QueryCount());
        }

        ASSERT_EQ(2ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, CopyToTest)
{
    DoConversionTest(CopyToTester{});
}

struct CopyToNoExceptTester
{
    template <bool NoExcept, typename FromTy, typename ToTy>
    void run() const
    {
        ASSERT_EQ(NoExcept, noexcept(std::declval<FromTy>().copy_to(std::declval<ToTy**>())));
    }
};

TEST(ComPtrTests, CopyToNoExceptTest)
{
    DoNoExceptConversionTest(CopyToNoExceptTester{});
}

struct CopyToIIDTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();

        dhorn::com::com_ptr<FromTy> fromPtr(static_cast<CastTy*>(ptr));
        try
        {
            Ty* rawPtr;
            fromPtr.copy_to(IID_PPV_ARGS(&rawPtr));
            ASSERT_TRUE(ShouldSucceed);

            ASSERT_EQ(3ul, ptr->RefCount());
            rawPtr->Release();

            // Query succeeded. Two AddRefs and always attempt QI
            ASSERT_EQ(addRefCount + 2, ptr->AddRefCount());
            ASSERT_EQ(qiCount + 1, ptr->QueryCount());
        }
        catch (std::system_error& e)
        {
            ASSERT_FALSE(ShouldSucceed);
            ASSERT_FALSE(e.code().category() != dhorn::com::hresult_category());

            ASSERT_EQ(static_cast<int>(E_NOINTERFACE), e.code().value());

            // Query failed. One (initial) AddRef, but should have attempted QI
            ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
            ASSERT_EQ(qiCount + 1, ptr->QueryCount());
        }

        ASSERT_EQ(2ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, CopyToIIDTest)
{
    DoConversionTest(CopyToIIDTester{});
}

struct QueryTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();
        constexpr bool shouldQI = !std::is_convertible_v<FromTy*, Ty*>;

        try
        {
            FromTy* rawPtr = static_cast<CastTy*>(ptr);
            auto comPtr = dhorn::com::query<Ty>(rawPtr);
            ASSERT_TRUE(ShouldSucceed);

            ASSERT_EQ(2ul, ptr->RefCount());

            // Query succeeded. One AddRef and QI only if needed
            ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
            ASSERT_EQ(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
        }
        catch (std::system_error& e)
        {
            ASSERT_FALSE(ShouldSucceed);
            ASSERT_FALSE(e.code().category() != dhorn::com::hresult_category());

            ASSERT_EQ(static_cast<int>(E_NOINTERFACE), e.code().value());

            // Query failed. No AddRefs, but should have attempted QI
            ASSERT_TRUE(shouldQI);
            ASSERT_EQ(addRefCount, ptr->AddRefCount());
            ASSERT_EQ(qiCount + 1, ptr->QueryCount());
        }

        ASSERT_EQ(1ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, QueryTest)
{
    DoConversionTest(QueryTester{});
}

struct QueryNoExceptTester
{
    template <bool NoExcept, typename FromTy, typename ToTy>
    void run() const
    {
        ASSERT_EQ(NoExcept, noexcept(dhorn::com::query<ToTy>(std::declval<typename FromTy::element_type*>())));
    }
};

TEST(ComPtrTests, QueryNoExceptTest)
{
    DoNoExceptConversionTest(QueryNoExceptTester{});
}

struct TryQueryTester
{
    template <bool ShouldSucceed, typename Ty, typename FromTy, typename CastTy = FromTy, typename PtrTy>
    void run(PtrTy* ptr) const
    {
        auto addRefCount = ptr->AddRefCount();
        auto qiCount = ptr->QueryCount();
        constexpr bool shouldQI = !std::is_convertible_v<FromTy*, Ty*>;

        {
            FromTy* rawPtr = static_cast<CastTy*>(ptr);
            auto comPtr = dhorn::com::try_query<Ty>(rawPtr);

            if (ShouldSucceed)
            {
                ASSERT_EQ(2ul, ptr->RefCount());
                ASSERT_TRUE(comPtr);

                // Query succeeded. One AddRef and QI only if needed
                ASSERT_EQ(addRefCount + 1, ptr->AddRefCount());
                ASSERT_EQ(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
            }
            else
            {
                ASSERT_EQ(1ul, ptr->RefCount());
                ASSERT_FALSE(comPtr);

                // Query failed. No AddRefs, but should have attempted QI
                ASSERT_TRUE(shouldQI);
                ASSERT_EQ(addRefCount, ptr->AddRefCount());
                ASSERT_EQ(qiCount + 1, ptr->QueryCount());
            }
        }

        ASSERT_EQ(1ul, ptr->RefCount());
    }
};

TEST(ComPtrTests, TryQueryTest)
{
    DoConversionTest(TryQueryTester{});
}

template <bool Expect, typename FirstTy, typename SecondTy>
void DoComparisonTest(FirstTy&& first, SecondTy&& second)
{
    ASSERT_EQ(Expect, first == second);
    ASSERT_EQ(Expect, second == first);
    ASSERT_EQ(!Expect, first != second);
    ASSERT_EQ(!Expect, second != first);
}

TEST(ComPtrTests, SameTypeComparisonTest)
{
    dhorn::com::com_ptr<IFoo> foo;
    ASSERT_TRUE(foo == foo);
    ASSERT_FALSE(foo != foo);

    dhorn::com::com_ptr<IFoo> otherFoo;
    DoComparisonTest<true>(foo, otherFoo);

    auto fooPtr = new Foo();
    DoComparisonTest<false>(foo, fooPtr);

    foo = fooPtr;
    ASSERT_TRUE(foo == foo);
    ASSERT_FALSE(foo != foo);
    DoComparisonTest<false>(foo, otherFoo);
    DoComparisonTest<true>(foo, fooPtr);

    otherFoo = foo;
    DoComparisonTest<true>(foo, otherFoo);

    otherFoo.attach(new Foo());
    DoComparisonTest<false>(foo, otherFoo);

    fooPtr->Release();
}

TEST(ComPtrTests, DifferentTypeComparisonType)
{
    dhorn::com::com_ptr<IFoo> foo;
    dhorn::com::com_ptr<IUnknown> unk;
    DoComparisonTest<true>(foo, unk);

    IFoo* fooPtr = new Foo();
    DoComparisonTest<false>(unk, fooPtr);

    foo = fooPtr;
    DoComparisonTest<false>(foo, unk);

    unk = foo;
    DoComparisonTest<true>(foo, unk);
    DoComparisonTest<true>(unk, fooPtr);

    unk.attach(static_cast<IFoo*>(new Foo()));
    DoComparisonTest<false>(foo, unk);
    DoComparisonTest<false>(unk, fooPtr);

    fooPtr->Release();
}

TEST(ComPtrTests, NullptrComparisonTest)
{
    dhorn::com::com_ptr<IFoo> foo;
    DoComparisonTest<true>(foo, nullptr);

    foo.attach(new Foo());
    DoComparisonTest<false>(foo, nullptr);
}
