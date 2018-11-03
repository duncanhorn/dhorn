/*
 * Duncan Horn
 *
 * CRTPBaseTests.cpp
 *
 * Tests for the crtp_base.h header
 */

#include <dhorn/crtp_base.h>
#include <functional>
#include <gtest/gtest.h>

template <typename Derived>
struct base : public dhorn::crtp_base<Derived>
{
    void invoke()
    {
        this->derived().impl();
    }

    void invoke() const
    {
        this->derived().impl();
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

TEST(CRTPBaseTests, InvokeTest)
{
    derived value([&](bool isConst)
    {
        ASSERT_FALSE(isConst);
    });

    value.invoke();
}

TEST(CRTPBaseTests, InvokeConstTest)
{
    const derived value([&](bool isConst)
    {
        ASSERT_TRUE(isConst);
    });

    value.invoke();
}
