/*
 * efficiency.h
 *
 * Duncan Horn
 *
 * Functions that are used for compiling and printing out efficiency information
 */
#pragma once

#include <chrono>
#include <vector>

namespace dhorn
{
    namespace tests
    {
        class efficiency_test
        {
            using clock = std::chrono::high_resolution_clock;
            using tick = decltype(clock::now());

        public:

            using time_val = std::chrono::milliseconds;

            void start(void)
            {
                _start = clock::now();
            }

            void stop(void)
            {
                using namespace std::chrono;
                auto duration = duration_cast<time_val>(clock::now() - this->_start);

                this->_times.push_back(duration);
            }

            const std::vector<time_val> &get_times(void) const
            {
                return this->_times;
            }

        private:

            tick _start;
            std::vector<time_val> _times;
        };
    }
}
