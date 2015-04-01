/*
 * EventSourceTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn event_source class
 */

#include "stdafx.h"
#include "CppUnitTest.h"

#include "dhorn/auto_event_cookie.h"
#include "dhorn/event_source.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(EventSourceTests)
        {
            TEST_METHOD(SingleEventTest)
            {
                int x = 0;
                dhorn::event_source<void(void)> source;

                auto cookie = source.add([&]() { ++x; });
                Assert::AreNotEqual(dhorn::invalid_event_cookie, cookie);
                Assert::AreEqual(static_cast<size_t>(1), source.size());

                source.invoke_all();
                Assert::AreEqual(1, x);
            }

            TEST_METHOD(RemoveTest)
            {
                int x = 0;
                dhorn::event_source<void(void)> source;

                auto cookie = source.add([&]() { ++x; });
                source.remove(cookie);
                Assert::AreEqual(static_cast<size_t>(0), source.size());

                // Should throw if we try and remove again
                try
                {
                    source.remove(cookie);
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::out_of_range &) {}
            }

            TEST_METHOD(MultipleEventInvokeOneSimpleTest)
            {
                int x = 0;
                dhorn::event_source<void(void)> source;

                source.add([&]() { ++x; });
                source.add([&]() { x += 2; });

                source.invoke_one(); // Invokes first added
                Assert::AreEqual(1, x);
            }

            TEST_METHOD(MultipleEventInvokeOneAdvancedTest)
            {
                int x = 0;
                dhorn::event_source<int(void)> source;

                source.add([&]() -> int { ++x; return x; });
                source.add([&]() -> int { x += 2; return x; });

                source.invoke_one([&](int val) { Assert::AreEqual(1, val); }); // Invokes first added
                Assert::AreEqual(1, x);
            }

            TEST_METHOD(MultipleEventInvokeAllSimpleTest)
            {
                int x = 0;
                dhorn::event_source<void(void)> source;

                source.add([&]() { ++x; });
                source.add([&]() { x += 2; });

                source.invoke_all();
                Assert::AreEqual(3, x);
            }

            TEST_METHOD(MultipleEventInvokeAllAdvancedTest)
            {
                int x = 0;
                dhorn::event_source<int(void)> source;

                source.add([&]() -> int { ++x; return x; });
                source.add([&]() -> int { x += 2; return x; });

                source.invoke_all([&](int val) { Assert::AreEqual(x, val); });
                Assert::AreEqual(3, x);
            }
        };



        TEST_CLASS(AutoEventCookieTests)
        {
            TEST_METHOD(DefaultConstructionTest)
            {
                // Should not throw/AV when empty
                dhorn::auto_event_cookie cookie;
            }

            TEST_METHOD(EventCookieConstuctionTest)
            {
                dhorn::event_source<void(void)> source;
                {
                    dhorn::auto_event_cookie cookie(source.add([]() {}), [&](dhorn::event_cookie cookie)
                    {
                        source.remove(cookie);
                    });

                    Assert::AreEqual(static_cast<size_t>(1), source.size());
                }

                // Destructor should have removed
                Assert::AreEqual(static_cast<size_t>(0), source.size());
            }

            TEST_METHOD(EventCookieMoveConstructionTest)
            {
                dhorn::event_source<void(void)> source;
                {
                    dhorn::auto_event_cookie cookie(source.add([]() {}), [&](dhorn::event_cookie cookie)
                    {
                        source.remove(cookie);
                    });
                    Assert::AreEqual(static_cast<size_t>(1), source.size());

                    dhorn::auto_event_cookie cookie2(std::move(cookie));
                    Assert::AreEqual(dhorn::invalid_event_cookie, static_cast<dhorn::event_cookie>(cookie));
                }

                // Destructor should have removed
                Assert::AreEqual(static_cast<size_t>(0), source.size());
            }
        };
    }
}
