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
        STDMETHOD_(size_t, RefCount)(void);
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
        Base(void) :
            count(1)
        {
        }

        ~Base(void)
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

        STDMETHOD_(ULONG, AddRef)(void)
        {
            Assert::IsTrue(count > 0);
            return ++count;
        }

        STDMETHOD_(ULONG, Release)(void)
        {
            auto val = --count;

            if (val == 0)
            {
                delete this;
            }

            return val;
        }

        // IBase
        STDMETHOD_(size_t, RefCount)(void)
        {
            return count;
        }

        // Tests run on single thread, so no need to synchronize
        size_t count;
    };

    class Foo :
        public Base,
        public IFoo
    {
    public:
        // IUnknown
        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
        {
            if (riid == __uuidof(IFoo))
            {
                *ppvObject = static_cast<IFoo *>(this);
                this->AddRef();
                return S_OK;
            }

            return Base::QueryInterface(riid, ppvObject);
        }

        STDMETHOD_(ULONG, AddRef)(void)
        {
            Assert::IsFalse(this->_moveOnly);
            return Base::AddRef();
        }

        STDMETHOD_(ULONG, Release)(void)
        {
            Assert::IsFalse(this->_moveOnly);
            return Base::Release();
        }

        // IBase
        STDMETHOD_(size_t, RefCount)(void)
        {
            return Base::RefCount();
        }

        // Other
        void StartMoveOnly() { this->_moveOnly = true; }
        void StopMoveOnly() { this->_moveOnly = false; }

    private:
        bool _moveOnly = false;
    };

    class Bar :
        public Base,
        public IBar
    {
    public:

        // IUnknown
        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
        {
            if (riid == __uuidof(IBar))
            {
                *ppvObject = static_cast<IBar *>(this);
                this->AddRef();
                return S_OK;
            }

            return Base::QueryInterface(riid, ppvObject);
        }

        STDMETHOD_(ULONG, AddRef)(void)
        {
            return Base::AddRef();
        }

        STDMETHOD_(ULONG, Release)(void)
        {
            return Base::Release();
        }

        // IBase
        STDMETHOD_(size_t, RefCount)(void)
        {
            return Base::RefCount();
        }
    };

    void GetFoo(REFIID iid, void **ppOut)
    {
        static IFoo *pFoo = new Foo();
        dhorn::com::com_ptr<IFoo>(pFoo).copy_to(iid, ppOut);
    }

    class FooBar :
        public Base,
        public IFoo,
        public IBar
    {
    public:

        // IUnknown
        STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
        {
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

        STDMETHOD_(ULONG, AddRef)(void)
        {
            return Base::AddRef();
        }

        STDMETHOD_(ULONG, Release)(void)
        {
            return Base::Release();
        }

        // IBase
        STDMETHOD_(size_t, RefCount)(void)
        {
            return Base::RefCount();
        }
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
        void DoTest(Func&& func)
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
        template <typename Ty, typename CastTy = Ty, typename PtrTy>
        static void PointerConstructionTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<Ty> comPtr(static_cast<CastTy*>(ptr));
            Assert::AreEqual(2u, ptr->RefCount());
        }

        TEST_METHOD(PointerConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                PointerConstructionTest_Impl<Foo>(foo);
                PointerConstructionTest_Impl<Bar>(bar);
                PointerConstructionTest_Impl<FooBar>(foobar);

                PointerConstructionTest_Impl<IFoo>(foo);
                PointerConstructionTest_Impl<IBar>(bar);
                PointerConstructionTest_Impl<IBase, IBase, IFoo>(foobar);
                PointerConstructionTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastPointerConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                PointerConstructionTest_Impl<IFoo, Foo>(foo);
                PointerConstructionTest_Impl<IBar, Bar>(bar);
                PointerConstructionTest_Impl<IFoo, FooBar>(foobar);
                PointerConstructionTest_Impl<IBar, FooBar>(foobar);

                PointerConstructionTest_Impl<IBase, IFoo>(foo);
                PointerConstructionTest_Impl<IBase, IBar>(bar);
                PointerConstructionTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                PointerConstructionTest_Impl<IUnknown, IBar, IBar>(foobar);
                PointerConstructionTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(PointerConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_ptr, Foo*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_ptr, Bar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<foobar_ptr, FooBar*>);

            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, IBase*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, IUnknown*>);
        }

        TEST_METHOD(DownCastPointerConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, Foo*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, Bar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, FooBar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, FooBar*>);

            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, IBase*>);
        }

        template <typename ComPtrTy, typename PtrTy = ComPtrTy>
        void NullPointerConstructionTest_Impl()
        {
            PtrTy* ptr = nullptr;
            dhorn::com::com_ptr<ComPtrTy> comPtr(ptr);
            Assert::IsFalse(comPtr);
        }

        TEST_METHOD(NullPointerConstructionTest)
        {
            NullPointerConstructionTest_Impl<Foo>();
            NullPointerConstructionTest_Impl<Bar>();
            NullPointerConstructionTest_Impl<FooBar>();

            NullPointerConstructionTest_Impl<IFoo>();
            NullPointerConstructionTest_Impl<IBar>();
            NullPointerConstructionTest_Impl<IBase>();
            NullPointerConstructionTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastPointerConstructionTest)
        {
            NullPointerConstructionTest_Impl<IFoo, Foo>();
            NullPointerConstructionTest_Impl<IBar, Bar>();
            NullPointerConstructionTest_Impl<IFoo, FooBar>();
            NullPointerConstructionTest_Impl<IBar, FooBar>();

            NullPointerConstructionTest_Impl<IBase, IFoo>();
            NullPointerConstructionTest_Impl<IBase, IBar>();
            NullPointerConstructionTest_Impl<IUnknown, IFoo>();
            NullPointerConstructionTest_Impl<IUnknown, IBar>();
            NullPointerConstructionTest_Impl<IUnknown, IBase>();
        }



        // Copy Construction
        template <typename Ty, typename OtherTy = Ty, typename PtrTy>
        static void CopyConstructionTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<OtherTy> comPtr(ptr);
            dhorn::com::com_ptr<Ty> copyPtr(comPtr);
            Assert::AreEqual(3u, ptr->RefCount());
        }

        TEST_METHOD(CopyConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                CopyConstructionTest_Impl<Foo>(foo);
                CopyConstructionTest_Impl<Bar>(bar);
                CopyConstructionTest_Impl<FooBar>(foobar);

                CopyConstructionTest_Impl<IFoo>(foo);
                CopyConstructionTest_Impl<IBar>(bar);
                CopyConstructionTest_Impl<IBase, IBase, IFoo>(foobar);
                CopyConstructionTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastCopyConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                CopyConstructionTest_Impl<IFoo, Foo>(foo);
                CopyConstructionTest_Impl<IBar, Bar>(bar);
                CopyConstructionTest_Impl<IFoo, FooBar>(foobar);
                CopyConstructionTest_Impl<IBar, FooBar>(foobar);

                CopyConstructionTest_Impl<IBase, IFoo>(foo);
                CopyConstructionTest_Impl<IBase, IBar>(bar);
                CopyConstructionTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                CopyConstructionTest_Impl<IUnknown, IBar, IBar>(foobar);
                CopyConstructionTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(CopyConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<foo_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<bar_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_copy_constructible_v<foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<base_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_constructible_v<unknown_interface_ptr>);
        }

        TEST_METHOD(DownCastCopyConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, foo_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, bar_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, foobar_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, base_interface_ptr>);
        }

        template <typename Ty, typename FromTy = Ty>
        void NullCopyConstructionTest_Impl()
        {
            dhorn::com::com_ptr<FromTy> ptr;
            dhorn::com::com_ptr<Ty> ptrCopy(ptr);
            Assert::IsFalse(ptrCopy);
        }

        TEST_METHOD(NullCopyConstructionTest)
        {
            NullCopyConstructionTest_Impl<Foo>();
            NullCopyConstructionTest_Impl<Bar>();
            NullCopyConstructionTest_Impl<FooBar>();

            NullCopyConstructionTest_Impl<IFoo>();
            NullCopyConstructionTest_Impl<IBar>();
            NullCopyConstructionTest_Impl<IBase>();
            NullCopyConstructionTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastCopyConstructionTest)
        {
            NullCopyConstructionTest_Impl<IFoo, Foo>();
            NullCopyConstructionTest_Impl<IBar, Bar>();
            NullCopyConstructionTest_Impl<IFoo, FooBar>();
            NullCopyConstructionTest_Impl<IBar, FooBar>();

            NullCopyConstructionTest_Impl<IBase, IFoo>();
            NullCopyConstructionTest_Impl<IBase, IBar>();
            NullCopyConstructionTest_Impl<IUnknown, IFoo>();
            NullCopyConstructionTest_Impl<IUnknown, IBar>();
            NullCopyConstructionTest_Impl<IUnknown, IBase>();
        }



        // Move Construction
        template <typename Ty, typename OtherTy = Ty, typename PtrTy>
        static void MoveConstructionTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<OtherTy> comPtr(ptr);
            dhorn::com::com_ptr<Ty> movedPtr(std::move(comPtr));

            Assert::AreEqual(2u, ptr->RefCount());
            Assert::IsFalse(comPtr);
            Assert::IsTrue(movedPtr);
        }

        TEST_METHOD(MoveConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                MoveConstructionTest_Impl<Foo>(foo);
                MoveConstructionTest_Impl<Bar>(bar);
                MoveConstructionTest_Impl<FooBar>(foobar);

                MoveConstructionTest_Impl<IFoo>(foo);
                MoveConstructionTest_Impl<IBar>(bar);
                MoveConstructionTest_Impl<IBase, IBase, IFoo>(foobar);
                MoveConstructionTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(MoveDownCastConstructionTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                MoveConstructionTest_Impl<IFoo, Foo>(foo);
                MoveConstructionTest_Impl<IBar, Bar>(bar);
                MoveConstructionTest_Impl<IFoo, FooBar>(foobar);
                MoveConstructionTest_Impl<IBar, FooBar>(foobar);

                MoveConstructionTest_Impl<IBase, IFoo>(foo);
                MoveConstructionTest_Impl<IBase, IBar>(bar);
                MoveConstructionTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                MoveConstructionTest_Impl<IUnknown, IBar, IBar>(foobar);
                MoveConstructionTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(MoveConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_move_constructible_v<foo_ptr>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<bar_ptr>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_move_constructible_v<foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<base_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_constructible_v<unknown_interface_ptr>);
        }

        TEST_METHOD(MoveDownCastConstructionNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, foo_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, bar_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<foo_interface_ptr, foobar_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<bar_interface_ptr, foobar_ptr&&>);

            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, foo_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<base_interface_ptr, bar_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, foo_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, bar_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_constructible_v<unknown_interface_ptr, base_interface_ptr&&>);
        }

        template <typename Ty, typename FromTy = Ty>
        void NullMoveConstructionTest_Impl()
        {
            dhorn::com::com_ptr<FromTy> ptr;
            dhorn::com::com_ptr<Ty> ptrMove(std::move(ptr));
            Assert::IsFalse(ptrMove);
        }

        TEST_METHOD(NullMoveConstructionTest)
        {
            NullMoveConstructionTest_Impl<Foo>();
            NullMoveConstructionTest_Impl<Bar>();
            NullMoveConstructionTest_Impl<FooBar>();

            NullMoveConstructionTest_Impl<IFoo>();
            NullMoveConstructionTest_Impl<IBar>();
            NullMoveConstructionTest_Impl<IBase>();
            NullMoveConstructionTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastMoveConstructionTest)
        {
            NullMoveConstructionTest_Impl<IFoo, Foo>();
            NullMoveConstructionTest_Impl<IBar, Bar>();
            NullMoveConstructionTest_Impl<IFoo, FooBar>();
            NullMoveConstructionTest_Impl<IBar, FooBar>();

            NullMoveConstructionTest_Impl<IBase, IFoo>();
            NullMoveConstructionTest_Impl<IBase, IBar>();
            NullMoveConstructionTest_Impl<IUnknown, IFoo>();
            NullMoveConstructionTest_Impl<IUnknown, IBar>();
            NullMoveConstructionTest_Impl<IUnknown, IBase>();
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
        template <typename Ty, typename CastTy = Ty, typename PtrTy>
        static void PointerAssignmentTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<Ty> comPtr;
            Assert::IsFalse(comPtr); // Avoid optimization

            comPtr = static_cast<CastTy*>(ptr);
            Assert::IsTrue(comPtr);
            Assert::AreEqual(2u, ptr->RefCount());
        }

        TEST_METHOD(PointerAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                PointerAssignmentTest_Impl<Foo>(foo);
                PointerAssignmentTest_Impl<Bar>(bar);
                PointerAssignmentTest_Impl<FooBar>(foobar);

                PointerAssignmentTest_Impl<IFoo>(foo);
                PointerAssignmentTest_Impl<IBar>(bar);
                PointerAssignmentTest_Impl<IBase, IBase, IFoo>(foobar);
                PointerAssignmentTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastPointerAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                PointerAssignmentTest_Impl<IFoo, Foo>(foo);
                PointerAssignmentTest_Impl<IBar, Bar>(bar);
                PointerAssignmentTest_Impl<IFoo, FooBar>(foobar);
                PointerAssignmentTest_Impl<IBar, FooBar>(foobar);

                PointerAssignmentTest_Impl<IBase, IFoo>(foo);
                PointerAssignmentTest_Impl<IBase, IBar>(bar);
                PointerAssignmentTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                PointerAssignmentTest_Impl<IUnknown, IBar, IBar>(foobar);
                PointerAssignmentTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(PointerAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_ptr, Foo*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_ptr, Bar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<foobar_ptr, FooBar*>);

            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, IBase*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, IUnknown*>);
        }

        TEST_METHOD(DownCastPointerAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, Foo*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, Bar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, FooBar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, FooBar*>);

            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, IFoo*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, IBar*>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, IBase*>);
        }

        template <typename ComPtrTy, typename PtrTy = ComPtrTy>
        static void NullPointerAssignmentTest_Impl()
        {
            PtrTy* ptr = nullptr;
            dhorn::com::com_ptr<ComPtrTy> comPtr;
            Assert::IsFalse(comPtr);

            comPtr = ptr;
            Assert::IsFalse(comPtr);
        }

        TEST_METHOD(NullPointerAssignmentTest)
        {
            NullPointerAssignmentTest_Impl<Foo>();
            NullPointerAssignmentTest_Impl<Bar>();
            NullPointerAssignmentTest_Impl<FooBar>();

            NullPointerAssignmentTest_Impl<IFoo>();
            NullPointerAssignmentTest_Impl<IBar>();
            NullPointerAssignmentTest_Impl<IBase>();
            NullPointerAssignmentTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastPointerAssignmentTest)
        {
            NullPointerAssignmentTest_Impl<IFoo, Foo>();
            NullPointerAssignmentTest_Impl<IBar, Bar>();
            NullPointerAssignmentTest_Impl<IFoo, FooBar>();
            NullPointerAssignmentTest_Impl<IBar, FooBar>();

            NullPointerAssignmentTest_Impl<IBase, IFoo>();
            NullPointerAssignmentTest_Impl<IBase, IBar>();
            NullPointerAssignmentTest_Impl<IUnknown, IFoo>();
            NullPointerAssignmentTest_Impl<IUnknown, IBar>();
            NullPointerAssignmentTest_Impl<IUnknown, IBase>();
        }



        // Copy Assignment
        template <typename Ty, typename OtherTy = Ty, typename PtrTy>
        static void CopyAssignmentTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<OtherTy> comPtr(ptr);
            dhorn::com::com_ptr<Ty> copyPtr;
            Assert::IsFalse(copyPtr);

            copyPtr = comPtr;
            Assert::IsTrue(comPtr);
            Assert::AreEqual(3u, ptr->RefCount());
        }

        TEST_METHOD(CopyAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                CopyAssignmentTest_Impl<Foo>(foo);
                CopyAssignmentTest_Impl<Bar>(bar);
                CopyAssignmentTest_Impl<FooBar>(foobar);

                CopyAssignmentTest_Impl<IFoo>(foo);
                CopyAssignmentTest_Impl<IBar>(bar);
                CopyAssignmentTest_Impl<IBase, IBase, IFoo>(foobar);
                CopyAssignmentTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastCopyAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                CopyAssignmentTest_Impl<IFoo, Foo>(foo);
                CopyAssignmentTest_Impl<IBar, Bar>(bar);
                CopyAssignmentTest_Impl<IFoo, FooBar>(foobar);
                CopyAssignmentTest_Impl<IBar, FooBar>(foobar);

                CopyAssignmentTest_Impl<IBase, IFoo>(foo);
                CopyAssignmentTest_Impl<IBase, IBar>(bar);
                CopyAssignmentTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                CopyAssignmentTest_Impl<IUnknown, IBar, IBar>(foobar);
                CopyAssignmentTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(CopyAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<foo_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<bar_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_copy_assignable_v<foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<base_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_copy_assignable_v<unknown_interface_ptr>);
        }

        TEST_METHOD(DownCastCopyAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, foo_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, bar_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, foobar_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, base_interface_ptr>);
        }

        template <typename Ty, typename FromTy = Ty>
        void NullCopyAssignmentTest_Impl()
        {
            dhorn::com::com_ptr<FromTy> ptr;
            dhorn::com::com_ptr<Ty> comPtr;
            Assert::IsFalse(comPtr);

            comPtr = ptr;
            Assert::IsFalse(comPtr);
        }

        TEST_METHOD(NullCopyAssignmentTest)
        {
            NullCopyAssignmentTest_Impl<Foo>();
            NullCopyAssignmentTest_Impl<Bar>();
            NullCopyAssignmentTest_Impl<FooBar>();

            NullCopyAssignmentTest_Impl<IFoo>();
            NullCopyAssignmentTest_Impl<IBar>();
            NullCopyAssignmentTest_Impl<IBase>();
            NullCopyAssignmentTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastCopyAssignmentTest)
        {
            NullCopyAssignmentTest_Impl<IFoo, Foo>();
            NullCopyAssignmentTest_Impl<IBar, Bar>();
            NullCopyAssignmentTest_Impl<IFoo, FooBar>();
            NullCopyAssignmentTest_Impl<IBar, FooBar>();

            NullCopyAssignmentTest_Impl<IBase, IFoo>();
            NullCopyAssignmentTest_Impl<IBase, IBar>();
            NullCopyAssignmentTest_Impl<IUnknown, IFoo>();
            NullCopyAssignmentTest_Impl<IUnknown, IBar>();
            NullCopyAssignmentTest_Impl<IUnknown, IBase>();
        }



        // Move Assignment
        template <typename Ty, typename OtherTy = Ty, typename PtrTy>
        static void MoveAssignmentTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<OtherTy> comPtr(ptr);
            dhorn::com::com_ptr<Ty> movedPtr;
            Assert::IsFalse(movedPtr);

            movedPtr = std::move(comPtr);
            Assert::AreEqual(2u, ptr->RefCount());
            Assert::IsFalse(comPtr);
            Assert::IsTrue(movedPtr);
        }

        TEST_METHOD(MoveAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                MoveAssignmentTest_Impl<Foo>(foo);
                MoveAssignmentTest_Impl<Bar>(bar);
                MoveAssignmentTest_Impl<FooBar>(foobar);

                MoveAssignmentTest_Impl<IFoo>(foo);
                MoveAssignmentTest_Impl<IBar>(bar);
                MoveAssignmentTest_Impl<IBase, IBase, IFoo>(foobar);
                MoveAssignmentTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastMoveAssignmentTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                MoveAssignmentTest_Impl<IFoo, Foo>(foo);
                MoveAssignmentTest_Impl<IBar, Bar>(bar);
                MoveAssignmentTest_Impl<IFoo, FooBar>(foobar);
                MoveAssignmentTest_Impl<IBar, FooBar>(foobar);

                MoveAssignmentTest_Impl<IBase, IFoo>(foo);
                MoveAssignmentTest_Impl<IBase, IBar>(bar);
                MoveAssignmentTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                MoveAssignmentTest_Impl<IUnknown, IBar, IBar>(foobar);
                MoveAssignmentTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(MoveAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_move_assignable_v<foo_ptr>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<bar_ptr>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<foobar_ptr>);

            Assert::IsTrue(std::is_nothrow_move_assignable_v<foo_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<bar_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<base_interface_ptr>);
            Assert::IsTrue(std::is_nothrow_move_assignable_v<unknown_interface_ptr>);
        }

        TEST_METHOD(DownCastMoveAssignmentNoExceptTest)
        {
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, foo_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, bar_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<foo_interface_ptr, foobar_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<bar_interface_ptr, foobar_ptr&&>);

            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, foo_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<base_interface_ptr, bar_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, foo_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, bar_interface_ptr&&>);
            Assert::IsTrue(std::is_nothrow_assignable_v<unknown_interface_ptr, base_interface_ptr&&>);
        }

        template <typename Ty, typename FromTy = Ty>
        void NullMoveAssignmentTest_Impl()
        {
            dhorn::com::com_ptr<FromTy> ptr;
            dhorn::com::com_ptr<Ty> ptrMove;
            Assert::IsFalse(ptrMove);

            ptrMove = std::move(ptr);
            Assert::IsFalse(ptrMove);
        }

        TEST_METHOD(NullMoveAssignmentTest)
        {
            NullMoveAssignmentTest_Impl<Foo>();
            NullMoveAssignmentTest_Impl<Bar>();
            NullMoveAssignmentTest_Impl<FooBar>();

            NullMoveAssignmentTest_Impl<IFoo>();
            NullMoveAssignmentTest_Impl<IBar>();
            NullMoveAssignmentTest_Impl<IBase>();
            NullMoveAssignmentTest_Impl<IUnknown>();
        }

        TEST_METHOD(NullDownCastMoveAssignmentTest)
        {
            NullMoveAssignmentTest_Impl<IFoo, Foo>();
            NullMoveAssignmentTest_Impl<IBar, Bar>();
            NullMoveAssignmentTest_Impl<IFoo, FooBar>();
            NullMoveAssignmentTest_Impl<IBar, FooBar>();

            NullMoveAssignmentTest_Impl<IBase, IFoo>();
            NullMoveAssignmentTest_Impl<IBase, IBar>();
            NullMoveAssignmentTest_Impl<IUnknown, IFoo>();
            NullMoveAssignmentTest_Impl<IUnknown, IBar>();
            NullMoveAssignmentTest_Impl<IUnknown, IBase>();
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

            foobar->Release();
        }

        TEST_METHOD(NullResetNoExceptTest)
        {
            dhorn::com::com_ptr<IFoo> comPtr;
            Assert::IsTrue(noexcept(comPtr.reset()));
            Assert::IsTrue(noexcept(comPtr.reset(nullptr)));
        }



        template <typename Ty, typename CastTy = Ty, typename PtrTy>
        static void ResetTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<Ty> comPtr;
            Assert::IsFalse(comPtr);

            comPtr.reset(static_cast<CastTy*>(ptr));
            Assert::IsTrue(comPtr);
            Assert::AreEqual(2u, ptr->RefCount());

            comPtr.reset(static_cast<CastTy*>(ptr));
            Assert::IsTrue(comPtr);
            Assert::AreEqual(2u, ptr->RefCount());
        }

        TEST_METHOD(ResetTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                ResetTest_Impl<Foo>(foo);
                ResetTest_Impl<Bar>(bar);
                ResetTest_Impl<FooBar>(foobar);

                ResetTest_Impl<IFoo>(foo);
                ResetTest_Impl<IBar>(bar);
                ResetTest_Impl<IBase, IBase, IFoo>(foobar);
                ResetTest_Impl<IUnknown, IUnknown, IBar>(foobar);
            });
        }

        TEST_METHOD(DownCastResetTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                ResetTest_Impl<IFoo, Foo>(foo);
                ResetTest_Impl<IBar, Bar>(bar);
                ResetTest_Impl<IFoo, FooBar>(foobar);
                ResetTest_Impl<IBar, FooBar>(foobar);

                ResetTest_Impl<IBase, IFoo>(foo);
                ResetTest_Impl<IBase, IBar>(bar);
                ResetTest_Impl<IUnknown, IFoo, IFoo>(foobar);
                ResetTest_Impl<IUnknown, IBar, IBar>(foobar);
                ResetTest_Impl<IUnknown, IBase, IFoo>(foobar);
            });
        }

        TEST_METHOD(ResetNoExceptTest)
        {
            Assert::IsTrue(noexcept(std::declval<foo_ptr>().reset(std::declval<Foo*>())));
            Assert::IsTrue(noexcept(std::declval<bar_ptr>().reset(std::declval<Bar*>())));
            Assert::IsTrue(noexcept(std::declval<foobar_ptr>().reset(std::declval<FooBar*>())));

            Assert::IsTrue(noexcept(std::declval<foo_interface_ptr>().reset(std::declval<IFoo*>())));
            Assert::IsTrue(noexcept(std::declval<bar_interface_ptr>().reset(std::declval<IBar*>())));
            Assert::IsTrue(noexcept(std::declval<base_interface_ptr>().reset(std::declval<IBase*>())));
            Assert::IsTrue(noexcept(std::declval<unknown_interface_ptr>().reset(std::declval<IUnknown*>())));
        }

        TEST_METHOD(DownCastResetNoExceptTest)
        {
            Assert::IsTrue(noexcept(std::declval<foo_interface_ptr>().reset(std::declval<Foo*>())));
            Assert::IsTrue(noexcept(std::declval<bar_interface_ptr>().reset(std::declval<Bar*>())));
            Assert::IsTrue(noexcept(std::declval<foo_interface_ptr>().reset(std::declval<FooBar*>())));
            Assert::IsTrue(noexcept(std::declval<bar_interface_ptr>().reset(std::declval<FooBar*>())));

            Assert::IsTrue(noexcept(std::declval<base_interface_ptr>().reset(std::declval<IFoo*>())));
            Assert::IsTrue(noexcept(std::declval<base_interface_ptr>().reset(std::declval<IBar*>())));
            Assert::IsTrue(noexcept(std::declval<unknown_interface_ptr>().reset(std::declval<IFoo*>())));
            Assert::IsTrue(noexcept(std::declval<unknown_interface_ptr>().reset(std::declval<IBar*>())));
            Assert::IsTrue(noexcept(std::declval<unknown_interface_ptr>().reset(std::declval<IBase*>())));
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

        template <bool ShouldSucceed, typename Ty, typename FromTy, typename PtrTy>
        static void UpCastTest_Impl(PtrTy* ptr)
        {
            dhorn::com::com_ptr<FromTy> fromPtr(ptr);
            try
            {
                auto comPtr = fromPtr.as<Ty>();
                if (!ShouldSucceed)
                {
                    Assert::Fail(L"Expected an exception");
                }

                Assert::AreEqual(3u, ptr->RefCount());
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
                }
            }

            Assert::AreEqual(2u, ptr->RefCount());
        }

        TEST_METHOD(UpCastTest)
        {
            DoTest([](Foo* foo, Bar* bar, FooBar* foobar)
            {
                // FooBar should QI to anything
                UpCastTest_Impl<true, IUnknown, IUnknown>(foobar);

                foo;
                bar;
            });
        }

#pragma endregion
    };
}
