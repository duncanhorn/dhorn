/*
 * Duncan Horn
 *
 * CRTPBaseTests.cpp
 *
 * Tests for the crtp_base.h header
 */
#include "stdafx.h"

#include <dhorn/crtp_base.h>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace dhorn::tests
{
    TEST_CLASS(CRTPBaseTests)
    {
        template <typename Derived>
        struct base : public crtp_base<Derived>
        {
            void invoke()
            {
                derived()->impl();
            }
        };

        struct derived : public base<derived>
        {
        public:
            template <typename Func>
            derived(Func&& func) :
                _func(std::forward<Func>(func))
            {
            }

            void impl()
            {
                this->_func();
            }

        private:

            std::function<void(void)> _func;
        };

        TEST_METHOD(InvokeTest)
        {
            int x = 0;
            derived value([&]()
            {
                x = 42;
            });

            value.invoke();
            Assert::AreEqual(x, 42);
        }
    };
}
