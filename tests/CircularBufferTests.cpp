/*
 * Duncan Horn
 *
 * CircularBufferTests.cpp
 *
 * Tests for the circular_buffer type
 */
#include "stdafx.h"

#include <dhorn/experimental/circular_buffer.h>
#include <vector>

#include "object_counter.h"

using namespace dhorn::experimental;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    template <std::size_t Count>
    struct throw_after
    {
        throw_after()
        {
            ++count;
            if (count == Count)
            {
                count = 0;
                throw std::exception();
            }

            ++instance_count;
        }

        throw_after(const throw_after&) :
            throw_after()
        {
        }

        ~throw_after()
        {
            --instance_count;
        }

        static void reset()
        {
            count = 0;
            instance_count = 0;
        }

        // To ensure that we're not breaking this contract...
        int operator&()
        {
            return 0;
        }

        inline static std::size_t count = 0;
        inline static std::size_t instance_count = 0;
    };

    using throw_after_4 = throw_after<4>;
    using throw_after_8 = throw_after<8>;



    TEST_CLASS(CircularBufferTests)
    {
        TEST_METHOD_INITIALIZE(TestInitialize)
        {
            object_counter::reset();
            throw_after_4::reset();
            throw_after_8::reset();
        }

#pragma region Constructor Tests

        TEST_METHOD(DefaultConstructorTest)
        {
            {
                circular_buffer<object_counter, 4> buffer;
                Assert::AreEqual(0u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            Assert::AreEqual(0u, object_counter::destructed_count);
        }

        TEST_METHOD(SizeConstructorTest)
        {
            {
                circular_buffer<object_counter, 4> buffer(4);
                Assert::AreEqual(4u, object_counter::constructed_count);
                Assert::AreEqual(4u, object_counter::instance_count);

                Assert::AreEqual(0u, object_counter::destructed_count);
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(4u, object_counter::destructed_count);
            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            {
                circular_buffer<object_counter, 8> buffer(0);
                Assert::AreEqual(0u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            Assert::AreEqual(0u, object_counter::destructed_count);
            object_counter::reset();

            Assert::ExpectException<std::bad_alloc>([]()
            {
                circular_buffer<object_counter, 4>(5);
            });

            Assert::AreEqual(0u, object_counter::instance_count);

            Assert::ExpectException<std::exception>([]()
            {
                circular_buffer<throw_after_4, 8>(8);
            });

            Assert::AreEqual(0u, throw_after_4::instance_count);
        }

        TEST_METHOD(SizeCopyConstructorTest)
        {
            {
                circular_buffer<object_counter, 4> buffer(4, object_counter{});
                Assert::AreEqual(5u, object_counter::constructed_count);
                Assert::AreEqual(4u, object_counter::instance_count);

                Assert::AreEqual(1u, object_counter::destructed_count);
                Assert::AreEqual(4u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(5u, object_counter::destructed_count);
            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            {
                circular_buffer<object_counter, 8> buffer(0, object_counter{});
                Assert::AreEqual(1u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            Assert::AreEqual(1u, object_counter::destructed_count);
            object_counter::reset();

            Assert::ExpectException<std::bad_alloc>([]()
            {
                circular_buffer<object_counter, 4>(5, object_counter{});
            });

            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            Assert::ExpectException<std::exception>([]()
            {
                circular_buffer<throw_after_4, 8>(8, throw_after_4{});
            });

            Assert::AreEqual(0u, throw_after_4::instance_count);
        }

        TEST_METHOD(IteratorConstructorTest)
        {
            {
                std::vector<object_counter> vector(4);
                circular_buffer<object_counter, 8> buffer(vector.begin(), vector.end());

                Assert::AreEqual(8u, object_counter::instance_count);
                Assert::AreEqual(8u, object_counter::constructed_count);
                Assert::AreEqual(4u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(8u, object_counter::destructed_count);
            object_counter::reset();

            {
                std::vector<object_counter> vector(4);
                circular_buffer<object_counter, 8> buffer(
                    std::make_move_iterator(vector.begin()),
                    std::make_move_iterator(vector.end()));

                Assert::AreEqual(8u, object_counter::instance_count);
                Assert::AreEqual(8u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(4u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(8u, object_counter::destructed_count);
            object_counter::reset();

            Assert::ExpectException<std::bad_alloc>([]()
            {
                std::vector<object_counter> vector(8);
                circular_buffer<object_counter, 4> buffer(vector.begin(), vector.end());
            });

            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            Assert::ExpectException<std::exception>([]()
            {
                std::vector<throw_after_8> vector(5);
                circular_buffer<throw_after_8, 8> buffer(vector.begin(), vector.end());
            });

            Assert::AreEqual(0u, throw_after_8::instance_count);
        }

        TEST_METHOD(CopyConstructorTest)
        {
            {
                circular_buffer<object_counter, 8> buffer(4);
                circular_buffer<object_counter, 8> copy(buffer);

                Assert::AreEqual(8u, object_counter::instance_count);
                Assert::AreEqual(8u, object_counter::constructed_count);
                Assert::AreEqual(4u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(8u, object_counter::destructed_count);
            object_counter::reset();

            {
                circular_buffer<object_counter, 8> buffer;
                circular_buffer<object_counter, 8> copy(buffer);

                Assert::AreEqual(0u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(0u, object_counter::destructed_count);
            object_counter::reset();

            {
                circular_buffer<throw_after_8, 8> buffer(5);
                Assert::ExpectException<std::exception>([&]()
                {
                    circular_buffer<throw_after_8, 8> copy(buffer);
                });
            }

            Assert::AreEqual(0u, throw_after_8::instance_count);
        }

        TEST_METHOD(MoveConstructorTest)
        {
            {
                circular_buffer<object_counter, 8> buffer(4);
                circular_buffer<object_counter, 8> copy(std::move(buffer));

                Assert::AreEqual(8u, object_counter::instance_count);
                Assert::AreEqual(8u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(4u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(8u, object_counter::destructed_count);
            object_counter::reset();

            {
                circular_buffer<object_counter, 8> buffer;
                circular_buffer<object_counter, 8> copy(std::move(buffer));

                Assert::AreEqual(0u, object_counter::instance_count);
                Assert::AreEqual(0u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(0u, object_counter::instance_count);
            Assert::AreEqual(0u, object_counter::destructed_count);
            object_counter::reset();

            {
                circular_buffer<throw_after_8, 8> buffer(5);
                Assert::ExpectException<std::exception>([&]()
                {
                    circular_buffer<throw_after_8, 8> copy(std::move(buffer));
                });
            }

            Assert::AreEqual(0u, throw_after_8::instance_count);
        }

        TEST_METHOD(InitializerListConstructorTest)
        {
            {
                circular_buffer<object_counter, 8> buffer({ object_counter{}, object_counter{}, object_counter{} });
                Assert::AreEqual(6u, object_counter::constructed_count);
                Assert::AreEqual(3u, object_counter::instance_count);

                Assert::AreEqual(3u, object_counter::destructed_count);
                Assert::AreEqual(3u, object_counter::copy_count);
                Assert::AreEqual(0u, object_counter::move_count);
            }

            Assert::AreEqual(6u, object_counter::destructed_count);
            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            {
                circular_buffer<object_counter, 8> buffer({});
                Assert::AreEqual(0u, object_counter::constructed_count);
                Assert::AreEqual(0u, object_counter::instance_count);
            }

            Assert::AreEqual(0u, object_counter::destructed_count);
            object_counter::reset();

            Assert::ExpectException<std::bad_alloc>([]()
            {
                circular_buffer<object_counter, 2> buffer{ object_counter{}, object_counter{}, object_counter{} };
            });

            Assert::AreEqual(0u, object_counter::instance_count);
            object_counter::reset();

            Assert::ExpectException<std::exception>([]()
            {
                circular_buffer<throw_after_4, 8> buffer({ throw_after_4{}, throw_after_4{} });
            });

            Assert::AreEqual(0u, throw_after_4::instance_count);
        }

#pragma endregion



#pragma region Assignment Tests

        TEST_METHOD(CopyAssignmentTest)
        {
            circular_buffer<object_counter, 8> buffer0(4);
            circular_buffer<object_counter, 8> buffer1;
            Assert::AreEqual(4u, object_counter::instance_count); // Mostly to prevent optimizations

            buffer1 = buffer0;
        }

        TEST_METHOD(MoveAssignmentTest)
        {
            // TODO
        }

        // TODO

#pragma endregion



#pragma region Iterator Tests

        // TODO

#pragma endregion
    };
}
