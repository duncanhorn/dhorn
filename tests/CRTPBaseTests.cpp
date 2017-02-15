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

            void invoke() const
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
                this->_func(false);
            }

            void impl() const
            {
                this->_func(true);
            }

        private:

            std::function<void(bool)> _func;
        };

        TEST_METHOD(InvokeTest)
        {
            int x = 0;
            derived value([&](bool isConst)
            {
                x = isConst ? 42 : 8;
            });

            value.invoke();
            Assert::AreEqual(x, 8);
        }

        TEST_METHOD(InvokeConstTest)
        {
            int x = 0;
            const derived value([&](bool isConst)
            {
                x = isConst ? 42 : 8;
            });

            value.invoke();
            Assert::AreEqual(x, 42);
        }
    };
}
