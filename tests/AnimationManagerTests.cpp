/*
 * Duncan Horn
 *
 * UnitsTests.cpp
 *
 * Tests for the animation_manager.h types/functions
 */
#include "stdafx.h"

#include <dhorn/experimental/animation_manager.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(AnimationManagerTests)
        {
            // Test animation instance
            class test_animation :
                public animation
            {
            public:

                test_animation() :
                    _nextState(animation_state::running)
                {
                }

                virtual ~test_animation()
                {
                    if (this->_onDestroy)
                    {
                        this->_onDestroy();
                    }
                }

                // animation
                virtual animation_state on_update(duration /*delta*/) override
                {
                    if (this->_onUpdate)
                    {
                        this->_onUpdate();
                    }

                    return this->_nextState;
                }

                // Test functions
                void set_next_state(animation_state state)
                {
                    this->_nextState = state;
                }

                void on_destroy(std::function<void(void)> fn)
                {
                    this->_onDestroy = std::move(fn);
                }

                void on_update(std::function<void(void)> fn)
                {
                    this->_onUpdate = std::move(fn);
                }

            private:

                animation_state _nextState;
                std::function<void(void)> _onDestroy;
                std::function<void(void)> _onUpdate;
            };

            TEST_METHOD(QueryStateFailureTest)
            {
                animation_manager mgr;
                test_animation *anim = new test_animation();
                auto handle = mgr.submit(anim);

                // Querying the animation state on a different animation_manager instance should throw
                try
                {
                    animation_manager mgr2;
                    mgr2.query_state(handle.get());
                    Assert::Fail(L"Expected an exception");
                }
                catch (std::out_of_range& /*e*/)
                {
                }
            }

            TEST_METHOD(CancelTest)
            {
                animation_manager mgr;
                test_animation *anim = new test_animation();
                auto handle = mgr.submit(anim);

                mgr.cancel(handle.get());

                // Should either be in the canceled or completed state; we don't really care
                Assert::IsTrue(details::is_complete(mgr.query_state(handle.get())));

                // After update, it should definitely be completed
                mgr.update();
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::completed);

                // Animations should be able to cancel themselves (and immediately transition to completed
                anim = new test_animation();
                anim->set_next_state(animation_state::canceled);
                handle = mgr.submit(anim);

                mgr.update();
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::completed);
            }

            TEST_METHOD(DestroyTest)
            {
                animation_manager mgr;
                test_animation *anim = new test_animation();
                anim->set_next_state(animation_state::completed);

                int x = 0;
                anim->on_destroy([&]()
                {
                    x = 42;
                });

                { // Let the handle fall out of scope
                    auto handle = mgr.submit(anim);
                    Assert::AreEqual(0, x);
                }

                // Handle is destroyed; shouldn't destroy the animation yet since it is still running
                Assert::AreEqual(0, x);

                // After update, the animation will complete and will be destroyed
                mgr.update();
                Assert::AreEqual(42, x);

                x = 0;
                anim = new test_animation();
                anim->set_next_state(animation_state::completed);
                anim->on_destroy([&]()
                {
                    x = 42;
                });

                { // Let the handle fall out of scope
                    auto handle = mgr.submit(anim);
                    mgr.update();

                    // Animation is complete, but hasn't fallen out of scope
                    Assert::AreEqual(0, x);
                }
                // NOTE: The animation won't be destroyed yet since we defer the remove until the next call to update
                mgr.update();
                Assert::AreEqual(42, x);
            }

            TEST_METHOD(PauseResumeTest)
            {
                animation_manager mgr;
                test_animation *anim = new test_animation();
                auto handle = mgr.submit(anim);

                mgr.pause(handle.get());
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::paused);

                // Update shouldn't impact animations
                int x = 0;
                anim->on_update([&]() { x = 42; });
                mgr.update();
                Assert::AreEqual(0, x);
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::paused);

                mgr.resume(handle.get());
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::running);

                // Animations should be able to transition themselves to paused
                anim->set_next_state(animation_state::paused);
                mgr.update();
                Assert::IsTrue(mgr.query_state(handle.get()) == animation_state::paused);
            }

            TEST_METHOD(DestructorTest)
            {
                // There's not much that we can easily test here, so just go with the simplest and make sure the
                // animation instance is cleaned up. Go ahead and test the std::shared_ptr version of submit too
                int x = 0;
                int y = 0;
                test_animation *anim = new test_animation();
                std::shared_ptr<test_animation> anim2 = std::make_shared<test_animation>();
                {
                    anim->on_destroy([&]() { x = 42; });
                    anim2->on_destroy([&]() { y = 8;} );

                    animation_manager mgr;
                    auto handle = mgr.submit(anim);
                    auto handle2 = mgr.submit(anim2);
                }

                Assert::AreEqual(42, x);
                Assert::AreEqual(0, y);
            }
        };
    }
}
