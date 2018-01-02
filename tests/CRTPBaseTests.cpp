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
                this->shim().impl();
            }

            void invoke() const
            {
                this->shim().impl();
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
            derived value([&](bool isConst)
            {
                Assert::IsFalse(isConst);
            });

            value.invoke();
        }

        TEST_METHOD(InvokeConstTest)
        {
            const derived value([&](bool isConst)
            {
                Assert::IsTrue(isConst);
            });

            value.invoke();
        }
    };
}
