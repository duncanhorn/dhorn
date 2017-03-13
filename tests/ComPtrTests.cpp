/*
 * Duncan Horn
 *
 * ComPtrTests.cpp
 *
 * Tests for the throwing dhorn::com::com_ptr
 */
#include "stdafx.h"

#include <dhorn/com/com_ptr.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    __interface __declspec(uuid("{56C9CB0F-534C-42D5-B297-9D77E71D908C}"))
    IBase : public IUnknown
    {
        STDMETHOD_(size_t, RefCount)();
        STDMETHOD_(size_t, AddRefCount)();
        STDMETHOD_(size_t, QueryCount)();
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

        ~Base()
        {
            Assert::IsTrue(count == 0);
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
            Assert::IsTrue(count > 0);
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
        STDMETHOD_(size_t, RefCount)()
        {
            return count;
        }

        STDMETHOD_(size_t, AddRefCount)()
        {
            return addRefCount;
        }

        // Tests run on single thread, so no need to synchronize
        size_t count = 1;
        size_t addRefCount = 0;
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
        STDMETHOD_(size_t, RefCount)()
        {
            return Base::RefCount();
        }

        STDMETHOD_(size_t, AddRefCount)()
        {
            return Base::AddRefCount();
        }

        STDMETHOD_(size_t, QueryCount)()
        {
            return this->_queryCount;
        }

    private:

        size_t _queryCount = 0;
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
        STDMETHOD_(size_t, RefCount)()
        {
            return Base::RefCount();
        }

        STDMETHOD_(size_t, AddRefCount)()
        {
            return Base::AddRefCount();
        }

        STDMETHOD_(size_t, QueryCount)()
        {
            return this->_queryCount;
        }

    private:

        size_t _queryCount = 0;
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
        STDMETHOD_(size_t, RefCount)()
        {
            return Base::RefCount();
        }

        STDMETHOD_(size_t, AddRefCount)()
        {
            return Base::AddRefCount();
        }

        STDMETHOD_(size_t, QueryCount)()
        {
            return this->_queryCount;
        }

    private:

        size_t _queryCount = 0;
    };

    TEST_CLASS(ComPtrTests)
    {
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

            Assert::AreEqual(1u, foo->RefCount());
            Assert::AreEqual(1u, bar->RefCount());
            Assert::AreEqual(1u, foobar->RefCount());

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



        /*
         * Constructor(s)/Destructor Tests
         */
#pragma region Constructor(s)/Destructors

        // Null/Default Construction
        TEST_METHOD(DefaultAndNullConstructionTest)
        {
            dhorn::com::com_ptr<IUnknown> unk;
            dhorn::com::com_ptr<IBase> base;
            dhorn::com::com_ptr<IFoo> foo(nullptr);
            dhorn::com::com_ptr<IBar> bar(nullptr);
            dhorn::com::com_ptr<FooBar> foobar(nullptr);

            Assert::IsFalse(unk);
            Assert::IsFalse(base);
            Assert::IsFalse(foo);
            Assert::IsFalse(bar);
            Assert::IsFalse(foobar);
        }

        TEST_METHOD(DefaultAndNullConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_default_constructible_v<foo_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_ptr, std::nullptr_t>);
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
                Assert::AreEqual(2u, ptr->RefCount());

                // Construction should add ref with no QIs
                Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(PointerConstructionTest)
        {
            DoPointerTest(PointerConstructionTester{});
        }

        TEST_METHOD(DownCastPointerConstructionTest)
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
                Assert::IsFalse(comPtr);
            }
        };

        TEST_METHOD(NullPointerConstructionTest)
        {
            DoPointerTraitsTest(NullPointerConstructionTester{});
        }

        TEST_METHOD(NullDownCastPointerConstructionTest)
        {
            DoDownCastPointerTraitsTest(NullPointerConstructionTester{});
        }

        struct PointerConstructionNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, FromTy*>);
            }
        };

        TEST_METHOD(PointerConstructionNoExceptTest)
        {
            DoPointerTraitsTest(PointerConstructionNoExceptTester{});
        }

        TEST_METHOD(DownCastPointerConstructionNoExceptTest)
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
                Assert::AreEqual(3u, ptr->RefCount());

                // Construction should add ref with no QIs
                Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(CopyConstructionTest)
        {
            DoPointerTest(CopyConstructionTester{});
        }

        TEST_METHOD(DownCastCopyConstructionTest)
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
                Assert::IsFalse(ptrCopy);
            }
        };

        TEST_METHOD(NullCopyConstructionTest)
        {
            DoPointerTraitsTest(NullCopyConstructionTester{});
        }

        TEST_METHOD(NullDownCastCopyConstructionTest)
        {
            DoDownCastPointerTraitsTest(NullCopyConstructionTester{});
        }

        struct CopyConstructionNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>>);
            }
        };

        TEST_METHOD(CopyConstructionNoExceptTest)
        {
            DoPointerTraitsTest(CopyConstructionNoExceptTester{});
        }

        TEST_METHOD(DownCastCopyConstructionNoExceptTest)
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

                Assert::AreEqual(2u, ptr->RefCount());
                Assert::IsFalse(comPtr);
                Assert::IsTrue(movedPtr);

                // One AddRef for the moved-from, but not for the moved-to. No QIs
                Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(MoveConstructionTest)
        {
            DoPointerTest(MoveConstructionTester{});
        }

        TEST_METHOD(MoveDownCastConstructionTest)
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
                Assert::IsFalse(ptrMove);
            }
        };

        TEST_METHOD(NullMoveConstructionTest)
        {
            DoPointerTraitsTest(NullMoveConstructionTester{});
        }

        TEST_METHOD(NullDownCastMoveConstructionTest)
        {
            DoDownCastPointerTraitsTest(NullMoveConstructionTester{});
        }

        struct MoveConstructionNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_constructible_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>&&>);
            }
        };

        TEST_METHOD(MoveConstructionNoExceptTest)
        {
            DoPointerTraitsTest(MoveConstructionNoExceptTester{});
        }

        TEST_METHOD(MoveDownCastConstructionNoExceptTest)
        {
            DoDownCastPointerTraitsTest(MoveConstructionNoExceptTester{});
        }

#pragma endregion



        /*
         * Assignment Tests
         */
#pragma region Assignment

        // Null Assignment
        template <typename Ty, typename PtrTy>
        static void NullAssignmentTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<Ty> comPtr(ptr);
            comPtr = nullptr;
            Assert::IsFalse(comPtr);
        }

        TEST_METHOD(NullAssignmentTest)
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

        TEST_METHOD(NullAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_ptr, std::nullptr_t>);
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
                Assert::IsFalse(comPtr);

                comPtr = static_cast<CastTy*>(ptr);
                Assert::IsTrue(comPtr);
                Assert::AreEqual(2u, ptr->RefCount());

                // Construction should add ref with no QIs
                Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(PointerAssignmentTest)
        {
            DoPointerTest(PointerAssignmentTester{});
        }

        TEST_METHOD(DownCastPointerAssignmentTest)
        {
            DoDownCastPointerTest(PointerAssignmentTester{});
        }

        struct NullPointerAssignmentTester
        {
            template <typename ComPtrTy, typename PtrTy = ComPtrTy>
            void run() const
            {
                dhorn::com::com_ptr<ComPtrTy> comPtr;
                Assert::IsFalse(comPtr);

                PtrTy* ptr = nullptr;
                comPtr = ptr;
                Assert::IsFalse(comPtr);
            }
        };

        TEST_METHOD(NullPointerAssignmentTest)
        {
            DoPointerTraitsTest(NullPointerAssignmentTester{});
        }

        TEST_METHOD(NullDownCastPointerAssignmentTest)
        {
            DoDownCastPointerTraitsTest(NullPointerAssignmentTester{});
        }

        struct PointerAssignmentNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, FromTy*>);
            }
        };

        TEST_METHOD(PointerAssignmentNoExceptTest)
        {
            DoPointerTraitsTest(PointerAssignmentNoExceptTester{});
        }

        TEST_METHOD(DownCastPointerAssignmentNoExceptTest)
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
                Assert::IsFalse(copyPtr);

                copyPtr = comPtr;
                Assert::IsTrue(comPtr);
                Assert::AreEqual(3u, ptr->RefCount());

                // Construction should add ref with no QIs
                Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(CopyAssignmentTest)
        {
            DoPointerTest(CopyAssignmentTester{});
        }

        TEST_METHOD(DownCastCopyAssignmentTest)
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
                Assert::IsFalse(comPtr);

                comPtr = ptr;
                Assert::IsFalse(comPtr);
            }
        };

        TEST_METHOD(NullCopyAssignmentTest)
        {
            DoPointerTraitsTest(NullCopyAssignmentTester{});
        }

        TEST_METHOD(NullDownCastCopyAssignmentTest)
        {
            DoDownCastPointerTraitsTest(NullCopyAssignmentTester{});
        }

        struct CopyAssignmentNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>>);
            }
        };

        TEST_METHOD(CopyAssignmentNoExceptTest)
        {
            DoPointerTraitsTest(CopyAssignmentNoExceptTester{});
        }

        TEST_METHOD(DownCastCopyAssignmentNoExceptTest)
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
                Assert::IsFalse(movedPtr);

                movedPtr = std::move(comPtr);
                Assert::AreEqual(2u, ptr->RefCount());
                Assert::IsFalse(comPtr);
                Assert::IsTrue(movedPtr);

                // AddRef for the moved-from, but not the moved-to; no QIs
                Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(MoveAssignmentTest)
        {
            DoPointerTest(MoveAssignmentTester{});
        }

        TEST_METHOD(DownCastMoveAssignmentTest)
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
                Assert::IsFalse(ptrMove);

                ptrMove = std::move(ptr);
                Assert::IsFalse(ptrMove);
            }
        };

        TEST_METHOD(NullMoveAssignmentTest)
        {
            DoPointerTraitsTest(NullMoveAssignmentTester{});
        }

        TEST_METHOD(NullDownCastMoveAssignmentTest)
        {
            DoDownCastPointerTraitsTest(NullMoveAssignmentTester{});
        }

        struct MoveAssignmentNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(std::is_nothrow_assignable_v<dhorn::com::com_ptr<Ty>, dhorn::com::com_ptr<FromTy>&&>);
            }
        };

        TEST_METHOD(MoveAssignmentNoExceptTest)
        {
            DoPointerTraitsTest(MoveAssignmentNoExceptTester{});
        }

        TEST_METHOD(DownCastMoveAssignmentNoExceptTest)
        {
            DoDownCastPointerTraitsTest(MoveAssignmentNoExceptTester{});
        }



        // Self Assignment
        TEST_METHOD(SelfAssignmentTest)
        {
            auto foobar = new FooBar();
            {
                // Easier to spot errors if the ref count is 1
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                foobar->Release();
                Assert::AreEqual(1u, foobar->RefCount());

                comPtr = comPtr;
                Assert::AreEqual(1u, foobar->RefCount());

                foobar->AddRef();
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

#pragma endregion



        /*
         * Reset Tests
         */
#pragma region Reset Tests

        TEST_METHOD(NullResetTest)
        {
            auto foobar = new FooBar();

            dhorn::com::com_ptr<FooBar> comPtr(foobar);
            Assert::AreEqual(2u, foobar->RefCount());

            comPtr.reset();
            Assert::IsFalse(comPtr);
            Assert::AreEqual(1u, foobar->RefCount());

            comPtr = foobar;
            Assert::AreEqual(2u, foobar->RefCount());

            comPtr.reset(nullptr);
            Assert::IsFalse(comPtr);
            Assert::AreEqual(1u, foobar->RefCount());

            // Calling reset when null should be fine
            comPtr.reset();
            Assert::IsFalse(comPtr);

            // Null non-std::nullptr_t should also be fine
            comPtr.reset(static_cast<FooBar*>(nullptr));
            Assert::IsFalse(comPtr);

            foobar->Release();
        }

        TEST_METHOD(NullResetNoExceptTest)
        {
            dhorn::com::com_ptr<IFoo> comPtr;
            Assert::IsTrue(noexcept(comPtr.reset()));
            Assert::IsTrue(noexcept(comPtr.reset(nullptr)));
        }



        struct ResetTester
        {
            template <typename Ty, typename CastTy = Ty, typename PtrTy>
            void run(PtrTy* ptr) const
            {
                auto addRefCount = ptr->AddRefCount();
                auto qiCount = ptr->QueryCount();

                dhorn::com::com_ptr<Ty> comPtr;
                Assert::IsFalse(comPtr);

                comPtr.reset(static_cast<CastTy*>(ptr));
                Assert::IsTrue(comPtr);
                Assert::AreEqual(2u, ptr->RefCount());

                comPtr.reset(static_cast<CastTy*>(ptr));
                Assert::IsTrue(comPtr);
                Assert::AreEqual(2u, ptr->RefCount());

                // One AddRef for each reset; no QIs
                Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                Assert::AreEqual(qiCount, ptr->QueryCount());
            }
        };

        TEST_METHOD(ResetTest)
        {
            DoPointerTest(ResetTester{});
        }

        TEST_METHOD(DownCastResetTest)
        {
            DoDownCastPointerTest(ResetTester{});
        }

        struct ResetNoExceptTester
        {
            template <typename Ty, typename FromTy = Ty>
            void run() const
            {
                Assert::IsTrue(noexcept(std::declval<dhorn::com::com_ptr<Ty>>().reset(std::declval<FromTy*>())));
            }
        };

        TEST_METHOD(ResetNoExceptTest)
        {
            DoPointerTraitsTest(ResetNoExceptTester{});
        }

        TEST_METHOD(DownCastResetNoExceptTest)
        {
            DoDownCastPointerTraitsTest(ResetNoExceptTester{});
        }

#pragma endregion



        /*
         * Other Tests
         */
#pragma region Other Tests

        TEST_METHOD(DereferenceOperatorTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, (*comPtr).RefCount());
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

        TEST_METHOD(OperatorArrowTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, comPtr->RefCount());
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

        TEST_METHOD(AddressOfOperatorTest)
        {
            auto foobar = new FooBar();

            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, comPtr->RefCount());

                // operator& should release
                auto ptr = &comPtr;
                Assert::IsFalse(comPtr);
                Assert::AreEqual(1u, foobar->RefCount());
                Assert::IsTrue(*ptr == nullptr);

                Assert::IsTrue(std::is_same_v<decltype(ptr), FooBar**>);
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

        TEST_METHOD(AttachTest)
        {
            auto foobar = new FooBar();

            dhorn::com::com_ptr<FooBar> comPtr;

            // Attach should not AddRef
            comPtr.attach(foobar);
            Assert::IsTrue(comPtr);
            Assert::AreEqual(1u, foobar->RefCount());

            foobar->AddRef();
            Assert::AreEqual(2u, foobar->RefCount());

            // Calling attach again should Release, but not AddRef the input
            comPtr.attach(foobar);
            Assert::AreEqual(1u, foobar->RefCount());
        }

        TEST_METHOD(DetachTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, foobar->RefCount());

                auto ptr = comPtr.detach();
                Assert::IsTrue(ptr == foobar);
                Assert::IsFalse(comPtr);
            }

            Assert::AreEqual(2u, foobar->RefCount());
            foobar->Release();
            foobar->Release();
        }

        TEST_METHOD(SwapTest)
        {
            IFoo* foo = new Foo();
            IBar* bar = new Bar();

            {
                dhorn::com::com_ptr<IBase> ptr1(foo);
                {
                    dhorn::com::com_ptr<IBase> ptr2(bar);

                    ptr1.swap(ptr2);
                    Assert::AreEqual(2u, foo->RefCount());
                    Assert::AreEqual(2u, bar->RefCount());

                    Assert::IsTrue(ptr1.get() == bar);
                    Assert::IsTrue(ptr2.get() == foo);
                }

                Assert::AreEqual(1u, foo->RefCount());
                Assert::AreEqual(2u, bar->RefCount());
            }

            Assert::AreEqual(1u, foo->RefCount());
            Assert::AreEqual(1u, bar->RefCount());
            foo->Release();
            bar->Release();
        }

        TEST_METHOD(SwapSelfTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, foobar->RefCount());

                comPtr.swap(comPtr);
                Assert::AreEqual(2u, foobar->RefCount());
                Assert::IsTrue(comPtr.get() == foobar);
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

        TEST_METHOD(ReleaseAndGetAddressOfTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, comPtr->RefCount());

                auto ptr = comPtr.release_and_get_address_of();
                Assert::IsFalse(comPtr);
                Assert::AreEqual(1u, foobar->RefCount());
                Assert::IsTrue(*ptr == nullptr);

                Assert::IsTrue(std::is_same_v<decltype(ptr), FooBar**>);
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

        TEST_METHOD(AddressOfTest)
        {
            auto foobar = new FooBar();
            {
                dhorn::com::com_ptr<FooBar> comPtr(foobar);
                Assert::AreEqual(2u, foobar->RefCount());

                // address_of does _not_ release
                auto ptr = comPtr.address_of();
                Assert::IsTrue(comPtr);
                Assert::AreEqual(2u, foobar->RefCount());
                Assert::IsTrue(*ptr == foobar);
                Assert::IsTrue(*ptr == comPtr.get());

                Assert::IsTrue(std::is_same_v<decltype(ptr), FooBar**>);
            }

            Assert::AreEqual(1u, foobar->RefCount());
            foobar->Release();
        }

#pragma endregion



        /*
         * Conversion Tests
         */
#pragma region Conversion Tests

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
                    auto comPtr = fromPtr.as<Ty>();
                    if (!ShouldSucceed)
                    {
                        Assert::Fail(L"Expected an exception");
                    }

                    Assert::AreEqual(3u, ptr->RefCount());

                    // Query succeeded. Two AddRefs and QI only if needed
                    Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                    Assert::AreEqual(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
                }
                catch (std::system_error& e)
                {
                    if (ShouldSucceed)
                    {
                        Assert::Fail(L"QueryInterface expected to succeed");
                    }
                    else if (e.code().category() != dhorn::com::hresult_category())
                    {
                        Assert::Fail(L"Expected an HRESULT failure");
                    }
                    else
                    {
                        Assert::AreEqual(static_cast<int>(E_NOINTERFACE), e.code().value());

                        // Query failed. One (initial) AddRef, but should have attempted QI
                        Assert::IsTrue(shouldQI);
                        Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(2u, ptr->RefCount());
            }
        };

        TEST_METHOD(AsTest)
        {
            DoConversionTest(AsTester{});
        }

        struct AsNoExceptTester
        {
            template <bool NoExcept, typename FromTy, typename ToTy>
            void run() const
            {
                Assert::AreEqual(NoExcept, noexcept(std::declval<FromTy>().as<ToTy>()));
            }
        };

        TEST_METHOD(AsNoExceptTest)
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
                    auto comPtr = fromPtr.try_as<Ty>();

                    if (ShouldSucceed)
                    {
                        Assert::AreEqual(3u, ptr->RefCount());
                        Assert::IsTrue(comPtr);

                        // Query succeeded. Two AddRefs and QI only if needed
                        Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
                    }
                    else
                    {
                        Assert::AreEqual(2u, ptr->RefCount());
                        Assert::IsFalse(comPtr);

                        // Query failed. One (initial) AddRef, but should have attempted QI
                        Assert::IsTrue(shouldQI);
                        Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(2u, ptr->RefCount());
            }
        };

        TEST_METHOD(TryAsTest)
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

                    if (!ShouldSucceed)
                    {
                        Assert::Fail(L"Expected an exception");
                    }

                    Assert::AreEqual(3u, ptr->RefCount());
                    rawPtr->Release();

                    // Query succeeded. Two AddRefs and QI only if needed
                    Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                    Assert::AreEqual(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
                }
                catch (std::system_error& e)
                {
                    if (ShouldSucceed)
                    {
                        Assert::Fail(L"QueryInterface expected to succeed");
                    }
                    else if (e.code().category() != dhorn::com::hresult_category())
                    {
                        Assert::Fail(L"Expected an HRESULT failure");
                    }
                    else
                    {
                        Assert::AreEqual(static_cast<int>(E_NOINTERFACE), e.code().value());

                        // Query failed. One (initial) AddRef, but should have attempted QI
                        Assert::IsTrue(shouldQI);
                        Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(2u, ptr->RefCount());
            }
        };

        TEST_METHOD(CopyToTest)
        {
            DoConversionTest(CopyToTester{});
        }

        struct CopyToNoExceptTester
        {
            template <bool NoExcept, typename FromTy, typename ToTy>
            void run() const
            {
                Assert::AreEqual(NoExcept, noexcept(std::declval<FromTy>().copy_to(std::declval<ToTy**>())));
            }
        };

        TEST_METHOD(CopyToNoExceptTest)
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

                    if (!ShouldSucceed)
                    {
                        Assert::Fail(L"Expected an exception");
                    }

                    Assert::AreEqual(3u, ptr->RefCount());
                    rawPtr->Release();

                    // Query succeeded. Two AddRefs and always attempt QI
                    Assert::AreEqual(addRefCount + 2, ptr->AddRefCount());
                    Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                }
                catch (std::system_error& e)
                {
                    if (ShouldSucceed)
                    {
                        Assert::Fail(L"QueryInterface expected to succeed");
                    }
                    else if (e.code().category() != dhorn::com::hresult_category())
                    {
                        Assert::Fail(L"Expected an HRESULT failure");
                    }
                    else
                    {
                        Assert::AreEqual(static_cast<int>(E_NOINTERFACE), e.code().value());

                        // Query failed. One (initial) AddRef, but should have attempted QI
                        Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(2u, ptr->RefCount());
            }
        };

        TEST_METHOD(CopyToIIDTest)
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
                    if (!ShouldSucceed)
                    {
                        Assert::Fail(L"Expected an exception");
                    }

                    Assert::AreEqual(2u, ptr->RefCount());

                    // Query succeeded. One AddRef and QI only if needed
                    Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                    Assert::AreEqual(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
                }
                catch (std::system_error& e)
                {
                    if (ShouldSucceed)
                    {
                        Assert::Fail(L"QueryInterface expected to succeed");
                    }
                    else if (e.code().category() != dhorn::com::hresult_category())
                    {
                        Assert::Fail(L"Expected an HRESULT failure");
                    }
                    else
                    {
                        Assert::AreEqual(static_cast<int>(E_NOINTERFACE), e.code().value());

                        // Query failed. No AddRefs, but should have attempted QI
                        Assert::IsTrue(shouldQI);
                        Assert::AreEqual(addRefCount, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(1u, ptr->RefCount());
            }
        };

        TEST_METHOD(QueryTest)
        {
            DoConversionTest(QueryTester{});
        }

        struct QueryNoExceptTester
        {
            template <bool NoExcept, typename FromTy, typename ToTy>
            void run() const
            {
                Assert::AreEqual(NoExcept, noexcept(dhorn::com::query<ToTy>(std::declval<typename FromTy::element_type*>())));
            }
        };

        TEST_METHOD(QueryNoExceptTest)
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
                        Assert::AreEqual(2u, ptr->RefCount());
                        Assert::IsTrue(comPtr);

                        // Query succeeded. One AddRef and QI only if needed
                        Assert::AreEqual(addRefCount + 1, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + (shouldQI ? 1 : 0), ptr->QueryCount());
                    }
                    else
                    {
                        Assert::AreEqual(1u, ptr->RefCount());
                        Assert::IsFalse(comPtr);

                        // Query failed. No AddRefs, but should have attempted QI
                        Assert::IsTrue(shouldQI);
                        Assert::AreEqual(addRefCount, ptr->AddRefCount());
                        Assert::AreEqual(qiCount + 1, ptr->QueryCount());
                    }
                }

                Assert::AreEqual(1u, ptr->RefCount());
            }
        };

        TEST_METHOD(TryQueryTest)
        {
            DoConversionTest(TryQueryTester{});
        }

#pragma endregion
    };
}
