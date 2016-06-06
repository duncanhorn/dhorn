/*
 * Duncan Horn
 *
 * object_counter.h
 *
 * Helper for tests that need to know how many copies, etc. have been made
 */
#pragma once

namespace dhorn
{
    namespace tests
    {
        class object_counter
        {
        public:

            object_counter()
            {
                ++instance_count;
                ++constructed_count;
            }

            object_counter(const object_counter &) :
                object_counter()
            {
                ++copy_count;
            }

            object_counter &operator=(const object_counter &)
            {
                ++copy_count;
                return *this;
            }

            object_counter(object_counter &&) :
                object_counter()
            {
                ++move_count;
            }

            object_counter &operator=(object_counter &&)
            {
                ++move_count;
                return *this;
            }

            ~object_counter()
            {
                --instance_count;
                ++destructed_count;
            }



            static void reset()
            {
                instance_count = 0;
                constructed_count = 0;
                destructed_count = 0;
                copy_count = 0;
                move_count = 0;
            }



            static size_t instance_count;
            static size_t constructed_count;
            static size_t destructed_count;
            static size_t copy_count;
            static size_t move_count;
        };
    }
}
