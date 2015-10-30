/*
 * EventSourceTests.cpp
 *
 * Duncan Horn
 *
 * Tests for the dhorn event_source class
 */

#include "stdafx.h"

#include "dhorn/unique_event_cookie.h"
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

                auto cookie = source.add([&]() -> int { ++x; return x; });
                source.add([&]() -> int { x += 2; return x; });

                source.invoke_all([&](int val) { Assert::AreEqual(x, val); });
                Assert::AreEqual(3, x);

                source.remove(cookie);
                source.invoke_all();
                Assert::AreEqual(5, x);
            }

            TEST_METHOD(NonVoidArgsTest)
            {
                int x = 0;
                dhorn::event_source<void(int, int)> source;

                source.add([&](int a, int b)
                {
                    x += a + b;
                });

                source.invoke_all(1, 2);

                Assert::AreEqual(3, x);
            }
        };



        TEST_CLASS(UniqueEventCookieTests)
        {
            using source_type = dhorn::event_source<void(void)>;

            TEST_METHOD(DefaultConstructionTest)
            {
                // Should not throw/AV when empty
                dhorn::unique_event_cookie cookie;
            }

            TEST_METHOD(EventCookieConstuctionTest)
            {
                source_type source;
                {
                    dhorn::unique_event_cookie cookie(
                        source.add([]() {}),
                        std::bind(&source_type::remove, &source, std::placeholders::_1));

                    Assert::AreEqual(static_cast<size_t>(1), source.size());
                }

                // Destructor should have removed
                Assert::AreEqual(static_cast<size_t>(0), source.size());
            }

            TEST_METHOD(EventCookieMoveConstructionTest)
            {
                source_type source;
                {
                    dhorn::unique_event_cookie cookie(
                        source.add([]() {}),
                        std::bind(&source_type::remove, &source, std::placeholders::_1));
                    Assert::AreEqual(static_cast<size_t>(1), source.size());

                    dhorn::unique_event_cookie cookie2(std::move(cookie));
                    Assert::AreEqual(dhorn::invalid_event_cookie, static_cast<dhorn::event_cookie>(cookie));
                }

                // Destructor should have removed
                Assert::AreEqual(static_cast<size_t>(0), source.size());
            }
        };
    }
}
