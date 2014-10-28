/*
 * Duncan Horn
 *
 * ComPtrTests.cpp
 *
 * Tests for the throwing com_ptr
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/windows/com_ptr.h"

using namespace dhorn::win32;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
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
            STDMETHODIMP QueryInterface(
                _In_ REFIID riid,
                _COM_Outptr_ void **ppvObject)
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
            STDMETHODIMP QueryInterface(
                _In_ REFIID riid,
                _COM_Outptr_ void **ppvObject)
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

        class Bar :
            public Base,
            public IBar
        {
        public:

            // IUnknown
            STDMETHODIMP QueryInterface(
                _In_ REFIID riid,
                _COM_Outptr_ void **ppvObject)
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

        class FooBar :
            public Base,
            public IFoo,
            public IBar
        {
        public:

            // IUnknown
            STDMETHODIMP QueryInterface(
                _In_ REFIID riid,
                _COM_Outptr_ void **ppvObject)
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
            /*
             * Constructor(s)/Destructor Tests
             */
#pragma region Constructor(s)/Destructors

            TEST_METHOD(DefaultAndNullConstructorTest)
            {
                com_ptr<IUnknown> unk;
                com_ptr<IBase> base;
                com_ptr<IFoo> foo(nullptr);
                com_ptr<IBar> bar(nullptr);

                Assert::IsFalse(unk);
                Assert::IsFalse(base);
                Assert::IsFalse(foo);
                Assert::IsFalse(bar);
            }

            TEST_METHOD(ValidPointerConstructionTest)
            {
                auto foo = new Foo();
                auto bar = new Bar();
                auto foobar = new FooBar();

                IFoo *pFoo = foobar;
                IBar *pBar = foobar;

                {
                    com_ptr<IFoo> f(foo);
                    Assert::IsTrue(f->RefCount() == 2);

                    com_ptr<IBar> b(bar);
                    Assert::IsTrue(b->RefCount() == 2);

                    com_ptr<IFoo> f2(foobar);
                    Assert::IsTrue(f2->RefCount() == 2);

                    com_ptr<IBar> b2(foobar);
                    Assert::IsTrue(b2->RefCount() == 3);

                    com_ptr<IUnknown> unk(pFoo);
                    Assert::IsTrue(foobar->RefCount() == 4);

                    com_ptr<IBase> base(pBar);
                    Assert::IsTrue(base->RefCount() == 5);
                }

                Assert::IsTrue(foo->RefCount() == 1);
                Assert::IsTrue(bar->RefCount() == 1);
                Assert::IsTrue(foobar->RefCount() == 1);

                foo->Release();
                bar->Release();
                foobar->Release();
            }

            TEST_METHOD(InvalidPointerConstructionTest)
            {
                IFoo *foo = new Foo();
                IBar *bar = new Bar();
                IBase *base = foo;

                {
                    try
                    {
                        com_ptr<IFoo> f(bar);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }

                    try
                    {
                        com_ptr<IBar> b(foo);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }

                    try
                    {
                        // base is type Foo
                        com_ptr<IBar> b(base);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                }

                Assert::IsTrue(foo->RefCount() == 1);
                Assert::IsTrue(bar->RefCount() == 1);

                foo->Release();
                bar->Release();
            }

            TEST_METHOD(NullPointerConstructionTest)
            {
                IFoo *pFoo = nullptr;

                // Any type should be constructable from a null pointer, regardless of the type
                com_ptr<IFoo> foo(pFoo);
                Assert::IsFalse(foo);
                com_ptr<IBar> bar(pFoo);
                Assert::IsFalse(bar);
                com_ptr<IBase> base(pFoo);
                Assert::IsFalse(base);
                com_ptr<IUnknown> unk(pFoo);
                Assert::IsFalse(unk);

                IUnknown *pUnk = nullptr;

                com_ptr<IFoo> foo2(pUnk);
                Assert::IsFalse(foo2);
                com_ptr<IBar> bar2(pUnk);
                Assert::IsFalse(bar2);
                com_ptr<IBase> base2(pUnk);
                Assert::IsFalse(base2);
                com_ptr<IUnknown> unk2(pUnk);
                Assert::IsFalse(unk2);
            }

            /*
            TEST_METHOD(PointerMoveConstructionTest)
            {
                {
                    com_ptr<IFoo> foo(new Foo());
                    Assert::IsTrue(foo->RefCount() == 1);

                    com_ptr<IBar> bar(new Bar());
                    Assert::IsTrue(bar->RefCount() == 1);

                    com_ptr<IBase> base(new Base());
                    Assert::IsTrue(base->RefCount() == 1);

                    com_ptr<IBase> base2(static_cast<IFoo *>(new Foo()));
                    Assert::IsTrue(base2->RefCount() == 1);

                    // If we throw an exception while move-constructing, then we expect the
                    // reference count to be decremented
                    IFoo *pFoo = new Foo();
                    IFoo *pFoo2 = pFoo;
                    pFoo2->AddRef();

                    try
                    {
                        com_ptr<IBar> bar2(std::move(pFoo));
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }

                    Assert::IsTrue(pFoo == nullptr);
                    Assert::IsTrue(pFoo2->RefCount() == 1);
                    pFoo2->Release();
                }
            }
            */

            TEST_METHOD(ValidCopyConstructorTest)
            {
                auto pFoo = new FooBar();

                {
                    size_t expect = 1;

                    com_ptr<IFoo> foo(pFoo);
                    Assert::IsTrue(foo->RefCount() == ++expect);

                    com_ptr<IBar> bar(foo);
                    Assert::IsTrue(bar->RefCount() == ++expect);

                    com_ptr<IFoo> foo2(foo);
                    Assert::IsTrue(foo2->RefCount() == ++expect);

                    com_ptr<IBase> base(foo);
                    Assert::IsTrue(base->RefCount() == ++expect);

                    com_ptr<IUnknown> unk(foo);
                    Assert::IsTrue(foo->RefCount() == ++expect);

                    com_ptr<IFoo> foo3(unk);
                    Assert::IsTrue(foo3->RefCount() == ++expect);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                pFoo->Release();
            }

            TEST_METHOD(InvalidCopyConstructorTest)
            {
                auto pBase = new Base();

                {
                    com_ptr<IBase> base(pBase);
                    Assert::IsTrue(base->RefCount() == 2);

                    try
                    {
                        com_ptr<IFoo> foo(base);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(base->RefCount() == 2);

                    try
                    {
                        com_ptr<IBar> bar(base);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(base->RefCount() == 2);
                }

                Assert::IsTrue(pBase->RefCount() == 1);
                pBase->Release();
            }

            TEST_METHOD(NullCopyConstructorTest)
            {
                com_ptr<IFoo> spFoo;

                // Any type should be constructable from a null pointer, regardless of the type
                com_ptr<IFoo> foo(spFoo);
                Assert::IsFalse(foo);
                com_ptr<IBar> bar(spFoo);
                Assert::IsFalse(bar);
                com_ptr<IBase> base(spFoo);
                Assert::IsFalse(base);
                com_ptr<IUnknown> unk(spFoo);
                Assert::IsFalse(unk);

                com_ptr<IUnknown> spUnk;

                com_ptr<IFoo> foo2(spUnk);
                Assert::IsFalse(foo2);
                com_ptr<IBar> bar2(spUnk);
                Assert::IsFalse(bar2);
                com_ptr<IBase> base2(spUnk);
                Assert::IsFalse(base2);
                com_ptr<IUnknown> unk2(spUnk);
                Assert::IsFalse(unk2);
            }

            TEST_METHOD(ValidMoveConstructorTest)
            {
                auto pFooBar = new FooBar();

                {
                    com_ptr<IFoo> foo(pFooBar);
                    Assert::IsTrue(pFooBar->RefCount() == 2);

                    // Down/same-cast - move allowed
                    com_ptr<IFoo> foo2(std::move(foo));
                    Assert::IsTrue(foo2);
                    Assert::IsFalse(foo);

                    com_ptr<IBase> base(std::move(foo2));
                    Assert::IsTrue(pFooBar->RefCount() == 2);
                    Assert::IsTrue(base);
                    Assert::IsFalse(foo2);

                    com_ptr<IUnknown> unk(std::move(base));
                    Assert::IsTrue(pFooBar->RefCount() == 2);
                    Assert::IsTrue(unk);
                    Assert::IsFalse(base);

                    // Up-cast - cannot move
                    com_ptr<IBase> base2(std::move(unk));
                    Assert::IsTrue(pFooBar->RefCount() == 3);
                    Assert::IsTrue(base2);
                    Assert::IsTrue(unk);

                    com_ptr<IBar> bar(std::move(base2));
                    Assert::IsTrue(pFooBar->RefCount() == 4);
                    Assert::IsTrue(bar);
                    Assert::IsTrue(base2);

                    com_ptr<IFoo> foo3(std::move(bar));
                    Assert::IsTrue(pFooBar->RefCount() == 5);
                    Assert::IsTrue(foo3);
                    Assert::IsTrue(bar);
                }

                Assert::IsTrue(pFooBar->RefCount() == 1);
                pFooBar->Release();
            }

            TEST_METHOD(InvalidMoveConstructorTest)
            {
                IFoo *pFoo = new Foo();

                {
                    com_ptr<IFoo> foo(pFoo);
                    com_ptr<IBase> base(pFoo);
                    com_ptr<IUnknown> unk(pFoo);
                    Assert::IsTrue(pFoo->RefCount() == 4);

                    // The compiler should select the non-r-value constructor when we pass an
                    // r-value reference to a com_ptr that cannot be implicitly casted, so there's
                    // not really much to test here. Just verify that it works
                    try
                    {
                        com_ptr<IBar> bar(std::move(foo));
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(foo);

                    try
                    {
                        com_ptr<IBar> bar(std::move(base));
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(base);

                    try
                    {
                        com_ptr<IBar> bar(std::move(unk));
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(unk);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                pFoo->Release();
            }

            TEST_METHOD(NullMoveConstructorTest)
            {
                com_ptr<IFoo> spFoo;

                com_ptr<IFoo> foo(std::move(spFoo));
                Assert::IsFalse(foo);
                com_ptr<IBar> bar(std::move(spFoo));
                Assert::IsFalse(bar);
                com_ptr<IBase> base(std::move(spFoo));
                Assert::IsFalse(base);
                com_ptr<IUnknown> unk(std::move(spFoo));
                Assert::IsFalse(unk);

                com_ptr<IUnknown> spUnk;

                com_ptr<IFoo> foo2(std::move(spUnk));
                Assert::IsFalse(foo2);
                com_ptr<IBar> bar2(std::move(spUnk));
                Assert::IsFalse(bar2);
                com_ptr<IBase> base2(std::move(spUnk));
                Assert::IsFalse(base2);
                com_ptr<IUnknown> unk2(std::move(spUnk));
                Assert::IsFalse(unk2);
            }

            TEST_METHOD(DestructorTest)
            {
                // We've been testing the destructor this whole time... just make it explicit
                IFoo *pFoo = new Foo();
                IBase *pBase = pFoo;
                IUnknown *pUnk = pFoo;

                Assert::IsTrue(pFoo->RefCount() == 1);
                {
                    com_ptr<IFoo> foo(pFoo);
                    Assert::IsTrue(pFoo->RefCount() == 2);
                    {
                        com_ptr<IBase> base(pBase);
                        Assert::IsTrue(pFoo->RefCount() == 3);
                        {
                            com_ptr<IUnknown> unk(pUnk);
                            Assert::IsTrue(pFoo->RefCount() == 4);
                        }
                        Assert::IsTrue(pFoo->RefCount() == 3);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 2);
                }
                Assert::IsTrue(pFoo->RefCount() == 1);

                // Now do the same, but cause an exception
                try
                {
                    com_ptr<IFoo> foo(pFoo);
                    Assert::IsTrue(pFoo->RefCount() == 2);
                    {
                        com_ptr<IBase> base(pBase);
                        Assert::IsTrue(pFoo->RefCount() == 3);
                        {
                            com_ptr<IUnknown> unk(pUnk);
                            Assert::IsTrue(pFoo->RefCount() == 4);
                            {
                                com_ptr<IBar> bar(pUnk);
                                Assert::Fail(L"Expected an exception");
                            }
                        }
                    }
                }
                catch (hresult_exception &e)
                {
                    Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                }
                Assert::IsTrue(pFoo->RefCount() == 1);

                // Now do it with assignment
                {
                    com_ptr<IFoo> foo;
                    Assert::IsFalse(foo);
                    foo = pFoo;
                    Assert::IsTrue(pFoo->RefCount() == 2);
                    {
                        com_ptr<IBase> base;
                        Assert::IsFalse(base);
                        base = pBase;
                        Assert::IsTrue(pFoo->RefCount() == 3);
                        {
                            com_ptr<IUnknown> unk;
                            Assert::IsFalse(unk);
                            unk = pUnk;
                            Assert::IsTrue(pFoo->RefCount() == 4);
                        }
                    }
                }
                Assert::IsTrue(pFoo->RefCount() == 1);

                // And again with the assigment, but with an exception
                try
                {
                    com_ptr<IFoo> foo;
                    Assert::IsFalse(foo);
                    foo = pFoo;
                    Assert::IsTrue(pFoo->RefCount() == 2);
                    {
                        com_ptr<IBase> base;
                        Assert::IsFalse(base);
                        base = pBase;
                        Assert::IsTrue(pFoo->RefCount() == 3);
                        {
                            com_ptr<IUnknown> unk;
                            Assert::IsFalse(unk);
                            unk = pUnk;
                            Assert::IsTrue(pFoo->RefCount() == 4);
                            {
                                com_ptr<IBar> bar;
                                Assert::IsFalse(bar);
                                bar = pUnk;
                                Assert::Fail(L"Expected an exception");
                            }
                        }
                    }
                }
                catch (hresult_exception &e)
                {
                    Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                }
                Assert::IsTrue(pFoo->RefCount() == 1);

                pFoo->Release();
            }

#pragma endregion



            /*
             * Assignment Tests
             */
#pragma region Assignment

            TEST_METHOD(ValidPointerAssignmentTest)
            {
                IFoo *foo = new Foo();
                IBar *bar = new Bar();
                auto foobar = new FooBar();

                IFoo *pFoo = foobar;
                IBar *pBar = foobar;

                {
                    com_ptr<IFoo> f;
                    Assert::IsFalse(f);
                    f = foo;
                    Assert::IsTrue(f->RefCount() == 2);

                    com_ptr<IBar> b;
                    Assert::IsFalse(b);
                    b = bar;
                    Assert::IsTrue(b->RefCount() == 2);

                    com_ptr<IFoo> f2;
                    Assert::IsFalse(f2);
                    f2 = foobar;
                    Assert::IsTrue(f2->RefCount() == 2);

                    com_ptr<IBar> b2;
                    Assert::IsFalse(b2);
                    b2 = foobar;
                    Assert::IsTrue(b2->RefCount() == 3);

                    com_ptr<IUnknown> unk;
                    Assert::IsFalse(unk);

                    // Assign to many things
                    unk = foo;
                    Assert::IsTrue(foo->RefCount() == 3);
                    unk = bar;
                    unk = bar;
                    unk = bar;
                    Assert::IsTrue(foo->RefCount() == 2);
                    Assert::IsTrue(bar->RefCount() == 3);
                    unk = pFoo;
                    Assert::IsTrue(bar->RefCount() == 2);
                    Assert::IsTrue(foobar->RefCount() == 4);
                    unk = pBar;
                    Assert::IsTrue(foobar->RefCount() == 4);

                    com_ptr<IBase> base;
                    Assert::IsFalse(base);
                    base = pBar;
                    Assert::IsTrue(base->RefCount() == 5);

                    // Make sure we can assign to nullptr
                    base = nullptr;
                    Assert::IsTrue(pBar->RefCount() == 4);
                }

                Assert::IsTrue(foo->RefCount() == 1);
                Assert::IsTrue(bar->RefCount() == 1);
                Assert::IsTrue(foobar->RefCount() == 1);

                foo->Release();
                bar->Release();
                foobar->Release();
            }

            TEST_METHOD(InvalidPointerAssignmentTest)
            {
                IFoo *foo = new Foo();
                IBar *bar = new Bar();
                IBase *base = foo;

                {
                    try
                    {
                        com_ptr<IFoo> f;
                        Assert::IsFalse(f);
                        f = bar;
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }

                    try
                    {
                        com_ptr<IBar> b;
                        Assert::IsFalse(b);
                        b = foo;
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }

                    try
                    {
                        // base is type Foo
                        com_ptr<IBar> b;
                        Assert::IsFalse(b);
                        b = base;
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                }

                Assert::IsTrue(foo->RefCount() == 1);
                Assert::IsTrue(bar->RefCount() == 1);

                foo->Release();
                bar->Release();
            }

            TEST_METHOD(PointerAssignmentFailureTest)
            {
                IFoo *pFoo = new Foo();
                IBar *pBar = new Bar();

                com_ptr<IBar> bar(pBar);
                try
                {
                    bar = pFoo;
                    Assert::Fail(L"Expected an exception");
                }
                catch (hresult_exception &e)
                {
                    Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                Assert::IsTrue(pBar->RefCount() == 1);

                pFoo->Release();
                pBar->Release();
            }

            TEST_METHOD(NullPointerAssignmentTest)
            {
                FooBar *pFooBar = new FooBar();
                IFoo *pFoo = pFooBar;
                IBar *pBar = pFooBar;

                // Declare all com_ptr's. Since we can actually test that assigning from valid
                // pointers works, go ahead and do that now
                com_ptr<IFoo> foo(pFoo);
                com_ptr<IBar> bar(pBar);
                com_ptr<IBase> base(pFoo);
                com_ptr<IUnknown> unk(pBar);
                Assert::IsTrue(pFooBar->RefCount() == 5);

                IFoo *pNullFoo = nullptr;

                // Any type should be assignable from a null pointer, regardless of the type
                foo = pNullFoo;
                bar = pNullFoo;
                base = pNullFoo;
                unk = pNullFoo;
                Assert::IsTrue(pFooBar->RefCount() == 1);

                // Restore
                foo = pFoo;
                bar = pBar;
                base = pFoo;
                unk = pBar;
                Assert::IsTrue(pFooBar->RefCount() == 5);

                IUnknown *pNullUnk = nullptr;

                foo = pNullUnk;
                bar = pNullUnk;
                base = pNullUnk;
                unk = pNullUnk;
                Assert::IsTrue(pFooBar->RefCount() == 1);

                pFooBar->Release();
            }

            TEST_METHOD(ValidCopyAssignmentTest)
            {
                auto pFoo = new FooBar();
                auto pBase = new Base();

                {
                    size_t expect = 1;

                    com_ptr<IFoo> foo;
                    Assert::IsFalse(foo);
                    foo = pFoo;
                    Assert::IsTrue(foo->RefCount() == ++expect);

                    com_ptr<IBar> bar;
                    Assert::IsFalse(bar);
                    bar = foo;
                    Assert::IsTrue(bar->RefCount() == ++expect);

                    com_ptr<IFoo> foo2;
                    Assert::IsFalse(foo2);
                    foo2 = foo;
                    Assert::IsTrue(foo2->RefCount() == ++expect);

                    com_ptr<IBase> base;
                    Assert::IsFalse(base);
                    base = foo;
                    Assert::IsTrue(base->RefCount() == ++expect);

                    com_ptr<IUnknown> unk;
                    Assert::IsFalse(unk);
                    unk = foo;
                    Assert::IsTrue(foo->RefCount() == ++expect);

                    com_ptr<IFoo> foo3;
                    Assert::IsFalse(foo3);
                    foo3 = unk;
                    Assert::IsTrue(foo3->RefCount() == ++expect);

                    // Assigning to the same object shouldn't give any problems
                    com_ptr<IBase> base2(pBase);
                    Assert::IsTrue(pBase->RefCount() == 2);
                    base2 = base2;
                    Assert::IsTrue(pBase->RefCount() == 2);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                Assert::IsTrue(pBase->RefCount() == 1);

                pFoo->Release();
                pBase->Release();
            }

            TEST_METHOD(InvalidCopyAssignmentTest)
            {
                auto pBase = new Base();

                {
                    com_ptr<IBase> base;
                    Assert::IsFalse(base);
                    base = pBase;
                    Assert::IsTrue(base->RefCount() == 2);

                    try
                    {
                        com_ptr<IFoo> foo;
                        Assert::IsFalse(foo);
                        foo = base;
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(base->RefCount() == 2);

                    try
                    {
                        com_ptr<IBar> bar;
                        Assert::IsFalse(bar);
                        bar = base;
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(base->RefCount() == 2);
                }

                Assert::IsTrue(pBase->RefCount() == 1);
                pBase->Release();
            }

            TEST_METHOD(CopyAssignmentFailureTest)
            {
                IFoo *pFoo = new Foo();
                IBar *pBar = new Bar();

                com_ptr<IBar> bar(pBar);
                try
                {
                    com_ptr<IFoo> foo(pFoo);
                    bar = foo;
                    Assert::Fail(L"Expected an exception");
                }
                catch (hresult_exception &e)
                {
                    Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                Assert::IsTrue(pBar->RefCount() == 1);

                pFoo->Release();
                pBar->Release();
            }

            TEST_METHOD(NullCopyAssignmentTest)
            {
                FooBar *pFooBar = new FooBar();
                IFoo *pFoo = pFooBar;
                IBar *pBar = pFooBar;

                // Declare all com_ptr's. Since we can actually test that assigning from valid
                // pointers works, go ahead and do that now
                com_ptr<IFoo> foo(pFoo);
                com_ptr<IBar> bar(pBar);
                com_ptr<IBase> base(pFoo);
                com_ptr<IUnknown> unk(pBar);
                Assert::IsTrue(pFooBar->RefCount() == 5);

                com_ptr<IFoo> spFoo;

                // Any type should be assignable from a null pointer, regardless of the type
                foo = spFoo;
                bar = spFoo;
                base = spFoo;
                unk = spFoo;
                Assert::IsTrue(pFooBar->RefCount() == 1);

                // Restore
                foo = pFoo;
                bar = pBar;
                base = pFoo;
                unk = pBar;
                Assert::IsTrue(pFooBar->RefCount() == 5);

                com_ptr<IUnknown> spUnk;

                foo = spUnk;
                bar = spUnk;
                base = spUnk;
                unk = spUnk;
                Assert::IsTrue(pFooBar->RefCount() == 1);

                pFooBar->Release();
            }

            TEST_METHOD(ValidMoveAssignmentTest)
            {
                auto pFooBar = new FooBar();

                {
                    com_ptr<IFoo> foo(pFooBar);
                    Assert::IsTrue(pFooBar->RefCount() == 2);

                    // Down/same-cast - move allowed
                    com_ptr<IFoo> foo2;
                    Assert::IsFalse(foo2);
                    foo2 = std::move(foo);
                    Assert::IsTrue(foo2);
                    Assert::IsFalse(foo);

                    com_ptr<IBase> base;
                    Assert::IsFalse(base);
                    base = std::move(foo2);
                    Assert::IsTrue(pFooBar->RefCount() == 2);
                    Assert::IsTrue(base);
                    Assert::IsFalse(foo2);

                    com_ptr<IUnknown> unk;
                    Assert::IsFalse(unk);
                    unk = std::move(base);
                    Assert::IsTrue(pFooBar->RefCount() == 2);
                    Assert::IsTrue(unk);
                    Assert::IsFalse(base);

                    // Up-cast - cannot move
                    com_ptr<IBase> base2;
                    Assert::IsFalse(base2);
                    base2 = std::move(unk);
                    Assert::IsTrue(pFooBar->RefCount() == 3);
                    Assert::IsTrue(base2);
                    Assert::IsTrue(unk);

                    com_ptr<IBar> bar;
                    Assert::IsFalse(bar);
                    bar = std::move(base2);
                    Assert::IsTrue(pFooBar->RefCount() == 4);
                    Assert::IsTrue(bar);
                    Assert::IsTrue(base2);

                    com_ptr<IFoo> foo3;
                    Assert::IsFalse(foo3);
                    foo3 = std::move(bar);
                    Assert::IsTrue(pFooBar->RefCount() == 5);
                    Assert::IsTrue(foo3);
                    Assert::IsTrue(bar);
                }

                Assert::IsTrue(pFooBar->RefCount() == 1);
                pFooBar->Release();
            }

            TEST_METHOD(InvalidMoveAssignmentTest)
            {
                IFoo *pFoo = new Foo();

                {
                    com_ptr<IFoo> foo(pFoo);
                    com_ptr<IBase> base(pFoo);
                    com_ptr<IUnknown> unk(pFoo);
                    Assert::IsTrue(pFoo->RefCount() == 4);

                    com_ptr<IBar> bar;

                    // The compiler should select the non-r-value constructor when we pass an
                    // r-value reference to a com_ptr that cannot be implicitly casted, so there's
                    // not really much to test here. Just verify that it works
                    try
                    {
                        bar = std::move(foo);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(foo);
                    Assert::IsFalse(bar);

                    try
                    {
                        bar = std::move(base);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(base);
                    Assert::IsFalse(bar);

                    try
                    {
                        bar = std::move(unk);
                        Assert::Fail(L"Expected an exception");
                    }
                    catch (hresult_exception &e)
                    {
                        Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                    }
                    Assert::IsTrue(pFoo->RefCount() == 4);
                    Assert::IsTrue(unk);
                    Assert::IsFalse(bar);
                }

                Assert::IsTrue(pFoo->RefCount() == 1);
                pFoo->Release();
            }

            TEST_METHOD(MoveAssignmentFailureTest)
            {
                IFoo *pFoo = new Foo();
                IBar *pBar = new Bar();

                com_ptr<IFoo> foo(pFoo);
                com_ptr<IBar> bar(pBar);
                try
                {
                    bar = std::move(foo);
                    Assert::Fail(L"Expected an exception");
                }
                catch (hresult_exception &e)
                {
                    Assert::IsTrue(e.get_hresult() == E_NOINTERFACE);
                }

                // Compiler should not use the move assignment operator, and should instead choose
                // the copy constructor. Thus, foo should still hold a valid reference to pFoo
                Assert::IsTrue(pFoo->RefCount() == 2);
                Assert::IsTrue(pBar->RefCount() == 1);

                pFoo->Release();
                pBar->Release();
            }

            TEST_METHOD(NullMoveAssignmentTest)
            {
                FooBar *pFooBar = new FooBar();
                IFoo *pFoo = pFooBar;
                IBar *pBar = pFooBar;

                // Declare all com_ptr's. Since we can actually test that assigning from valid
                // pointers works, go ahead and do that now
                com_ptr<IFoo> foo(pFoo);
                com_ptr<IBar> bar(pBar);
                com_ptr<IBase> base(pFoo);
                com_ptr<IUnknown> unk(pBar);
                Assert::IsTrue(pFooBar->RefCount() == 5);

                com_ptr<IFoo> spFoo;

                // Any type should be assignable from a null pointer, regardless of the type
                foo = com_ptr<IFoo>(spFoo);
                bar = com_ptr<IFoo>(spFoo);
                base = com_ptr<IFoo>(spFoo);
                unk = com_ptr<IFoo>(spFoo);
                Assert::IsTrue(pFooBar->RefCount() == 1);

                // Restore
                foo = pFoo;
                bar = pBar;
                base = pFoo;
                unk = pBar;
                Assert::IsTrue(pFooBar->RefCount() == 5);

                com_ptr<IUnknown> spUnk;

                foo = com_ptr<IUnknown>(spUnk);
                bar = com_ptr<IUnknown>(spUnk);
                base = com_ptr<IUnknown>(spUnk);
                unk = com_ptr<IUnknown>(spUnk);
                Assert::IsTrue(pFooBar->RefCount() == 1);

                pFooBar->Release();
            }

#pragma endregion
        };
    }
}
