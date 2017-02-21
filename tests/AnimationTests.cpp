/*
 * Duncan Horn
 *
 * AnimationTests.cpp
 *
 * Tests for the various animations functions/types
 */
#include "stdafx.h"

#include <dhorn/experimental/discrete_animation.h>
#include <dhorn/experimental/linear_animation.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn
{
    namespace tests
    {
        TEST_CLASS(DiscreteAnimationTests)
        {
            using animation_type = dhorn::experimental::discrete_animation<float>;
            using duration = std::chrono::high_resolution_clock::duration;

            TEST_METHOD(NoValueTest)
            {
                float value = 42;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                const auto state = anim.on_update(duration(0));
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(42.0f, value);
            }

            TEST_METHOD(SingleValueTest)
            {
                float value = 42;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                anim.add_key_frame(2 * one_second, 1.0f);

                auto state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(42.0f, value);

                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(1.0f, value);
            }

            TEST_METHOD(TwoValueAddIncreasingTest)
            {
                float value = 42;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                anim.add_key_frame(one_second, 1.0f);
                anim.add_key_frame(one_second * 2, 2.0f);

                auto state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(42.0f, value);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(1.0f, value);

                value = 8.0f;
                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(8.0f, value);

                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(2.0f, value);
            }

            TEST_METHOD(TwoValueAddDecreasingTest)
            {
                float value = 42;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                anim.add_key_frame(one_second * 2, 2.0f);
                anim.add_key_frame(one_second, 1.0f);

                auto state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(42.0f, value);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(1.0f, value);

                value = 8.0f;
                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(8.0f, value);

                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(2.0f, value);
            }

            TEST_METHOD(InsertDuringTest)
            {
                float value = 42;
                animation_type anim([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                anim.add_key_frame(one_second * 10, 10.0f);

                auto state = anim.on_update(one_second + half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(42.0f, value);

                // Despite already reaching one and a half seconds, we should still get the update for one second
                anim.add_key_frame(one_second, 1.0f);
                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(1.0f, value);

                // We'll skip the two second mark though, since we now have a three second one
                anim.add_key_frame(one_second * 3, 3.0f);
                anim.add_key_frame(one_second * 2, 2.0f);
                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(3.0f, value);

                // re-defining the value shouldn't cause another update
                anim.add_key_frame(one_second * 3, 100.0f);
                state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(3.0f, value);

                anim.add_key_frame(one_second * 9 + half_second, 9.5f);
                state = anim.on_update(one_second * 10);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(10.0f, value);
            }
        };



        TEST_CLASS(LinearAnimationTests)
        {
            using animation_type = dhorn::experimental::linear_animation<float>;
            using duration = animation_type::duration;

            TEST_METHOD(NoValueTest)
            {
                float value = 42.0;
                animation_type anim([&](float val)
                {
                    value = val;
                });

                auto state = anim.on_update(std::chrono::duration_cast<duration>(std::chrono::seconds(1)));
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(42.0f, value);
            }

            TEST_METHOD(SingleValueTest)
            {
                float value = 42.0;
                animation_type anim([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                anim.add_key_frame(one_second, 1.0f);

                auto state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(42.0f, value);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(1.0f, value);
            }

            TEST_METHOD(TwoValueSimpleTest)
            {
                float value = 42.0;
                animation_type anim([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                anim.add_key_frame(duration(0), 0.0f);
                anim.add_key_frame(one_second, 1.0f);

                auto state = anim.on_update(duration(0));
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 0.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(0.5f, value);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(1.0f, value);
            }

            TEST_METHOD(TwoValueComplexTest)
            {
                float value = 42.0;
                animation_type anim([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));
                auto quarter_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(250));
                anim.add_key_frame(one_second * 3, 3.0f);
                anim.add_key_frame(one_second + half_second, 5.0f);

                auto state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 42.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 5.0f);

                state = anim.on_update(quarter_second * 3);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 4.0f);

                state = anim.on_update(quarter_second * 3);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(value, 3.0f);
            }

            TEST_METHOD(MultiValueTest)
            {
                float value = 42.0;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));

                // The graph looks somewhat like (but steeper at parts; one second per direction change):
                //    /\
                //   /  \__/\
                //  /        \_
                anim.add_key_frame(one_second * 6, 0.0f);
                anim.add_key_frame(one_second * 3, 1.0f);
                anim.add_key_frame(one_second, 3.0f);
                anim.add_key_frame(one_second * 5, 0.0f);
                anim.add_key_frame(one_second * 4, 2.0f);
                anim.add_key_frame(one_second * 2, 1.0f);
                anim.add_key_frame(duration(0), 0.0f);

                auto state = anim.on_update(duration(0));
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 0.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.5f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 3.0f); // Second 1

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 2.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.0f); // Second 2

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.0f); // Second 3

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.5f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 2.0f); // Second 4

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 0.0f); // Second 5

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 0.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(value, 0.0f); // Second 6
            }

            TEST_METHOD(InsertDuringTest)
            {
                float value = 42.0;
                animation_type anim;
                anim.set_callback([&](float val)
                {
                    value = val;
                });

                auto one_second = std::chrono::duration_cast<duration>(std::chrono::seconds(1));
                auto half_second = std::chrono::duration_cast<duration>(std::chrono::milliseconds(500));

                anim.add_key_frame(duration(0), 0.0f);
                anim.add_key_frame(one_second * 2, 2.0f);

                auto state = anim.on_update(one_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 1.0f);

                anim.add_key_frame(one_second, 2.0f);
                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::running);
                Assert::AreEqual(value, 2.0f);

                state = anim.on_update(half_second);
                Assert::IsTrue(state == dhorn::experimental::animation_state::completed);
                Assert::AreEqual(value, 2.0f);
            }
        };
    }
}
