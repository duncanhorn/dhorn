/*
 * Duncan Horn
 *
 * crtp_base.h
 *
 * Useful base class for types that use the curiously recurring template pattern. It provides a single function -
 * `derived` that returns the `this` pointer converted to a pointer to the derived type. An example usage might look
 * like the following:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  template <typename Derived>
 *  class foo : public crtp_base<Derived>
 *  {
 *  public:
 *      void doit()
 *      {
 *          return derived()->doit_impl();
 *      }
 *  };
 *
 *  class bar : public foo<bar>
 *  {
 *  public:
 *      void doit_impl()
 *      {
 *          // ...
 *      }
 *  };
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#pragma once

namespace dhorn
{
    template <typename Derived>
    struct crtp_base
    {
    protected:

        Derived* derived(void)
        {
            return static_cast<Derived*>(this);
        }

        const Derived* derived(void) const
        {
            return static_cast<const Derived*>(this);
        }
    };
}
