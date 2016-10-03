/*
 * Duncan Horn
 *
 * ScopeGuardTests.cpp
 *
 * Tests for the scope_guard.h functions
 */
#include "stdafx.h"

#include <dhorn/experimental/scope_guard.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(ScopeGuardTests)
        {
            TEST_METHOD(RunOnExitTest)
            {
                int x = 0;
                {
                    auto fn = dhorn::experimental::make_scope_guard([&]() { x = 42; });
                    Assert::AreEqual(0, x);
                }
                Assert::AreEqual(42, x);
            }

            TEST_METHOD(RunOnExceptionTest)
            {
                int x = 0;
                try
                {
                    auto fn = dhorn::experimental::make_scope_guard([&]() { x = 42; });
                    Assert::AreEqual(0, x);

                    throw std::exception();
                }
                catch (...)
                {
                }

                Assert::AreEqual(42, x);
            }

            TEST_METHOD(CancelTest)
            {
                int x = 0;
                {
                    auto fn = dhorn::experimental::make_scope_guard([&]() { x = 42; });
                    Assert::AreEqual(0, x);

                    fn.cancel();
                }
                Assert::AreEqual(0, x);
            }

            TEST_METHOD(MoveConstructTest)
            {
                object_counter::reset();

                {
                    object_counter cnt;
                    auto fn = dhorn::experimental::make_scope_guard([cnt = std::move(cnt)]() {});
                }

                // Only the initial constructed value should have been a non-move
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(object_counter::constructed_count - 1, object_counter::move_count);
            }

            TEST_METHOD(CopyConstructorTest)
            {
                object_counter::reset();

                {
                    object_counter cnt;
                    auto func = [cnt = std::move(cnt)]() {};
                    auto fn = dhorn::experimental::make_scope_guard(func);
                }

                // Should have been at least one copy
                Assert::AreEqual(1u, object_counter::copy_count);
            }
        };
    }
}
