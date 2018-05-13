/*
 * Duncan Horn
 *
 * Helpers and adapters for consuming and authoring iterators.
 */
#pragma once

#include <iterator>

namespace dhorn
{
    /*
     * proxy_pointer
     *
     * The type of `std::iterator_traits<Iterator>::pointer` is defined to be the result of `operator->` on an instance
     * of `Iterator`. This doesn't mix well with iterator adapters as they tend to return temporary objects. Returning
     * by value for such iterators works fine for `operator*`, but not so much for `operator->` since the `->`
     * "propagates" until it reaches a pointer type. The `proxy_iterator` type is an alternative that can be constructed
     * by value and provides a single `operator->` that returns a pointer to the object.
     *
     * This type also provides an `operator*`, making it a useful type for use as an input iterator's post-increment
     * proxy iterator return value.
     */
    template <typename Ty>
    class proxy_pointer
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        template <typename ValueTy, std::enable_if_t<std::is_constructible_v<Ty, ValueTy&&>, int> = 0>
        constexpr proxy_pointer(ValueTy&& value) noexcept(std::is_nothrow_constructible_v<Ty, ValueTy&&>) :
            _value(std::forward<ValueTy>(value))
        {
        }



        /*
         * Accessors
         */
        Ty* operator->() noexcept
        {
            return &this->_value;
        }

        const Ty* operator->() const noexcept
        {
            return &this->_value;
        }

        Ty& operator*() noexcept
        {
            return this->_value;
        }

        const Ty& operator*() const noexcept
        {
            return this->_value;
        }



    private:

        Ty _value;
    };
}
