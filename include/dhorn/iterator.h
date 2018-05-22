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



    /*
     * array_iterator
     *
     * A re-usable iterator type for types that manage ranges that are exposed as-if by pointers into arrays. This gives
     * improvements over that solution by having this iterator be a unique type. The only requirement is that the type
     * used have the member alias `value_type`, which is used in defining the iterator's own `value_type` alias. For
     * const_iterator-s, use the const type (or the `const_array_iterator` alias). E.g.:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *  struct foo
     *  {
     *      using iterator = array_iterator<foo>;
     *      using const_iterator = array_iterator<const foo>; // OR const_array_iterator<foo>;
     *  };
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template <typename Ty>
    class array_iterator
    {
        using other_value_type = typename Ty::value_type;
        using non_const_iterator = array_iterator<std::remove_const_t<Ty>>;

        friend class array_iterator<std::add_const_t<Ty>>;

    public:
        /*
         * Public Types
         */
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<std::is_const_v<Ty>, const other_value_type, other_value_type>;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::random_access_iterator_tag;



        /*
         * Constructor(s)/Destructor
         */
        constexpr array_iterator() noexcept = default;

        constexpr array_iterator(pointer ptr) noexcept :
            _ptr(ptr)
        {
        }

        constexpr array_iterator(const non_const_iterator& other) noexcept :
            _ptr(other._ptr)
        {
        }



        /*
         * ForwardIterator
         */
        constexpr reference operator*() const noexcept
        {
            return *this->_ptr;
        }

        constexpr pointer operator->() const noexcept
        {
            return this->_ptr;
        }

        constexpr array_iterator& operator++() noexcept
        {
            ++this->_ptr;
            return *this;
        }

        constexpr array_iterator operator++(int) noexcept
        {
            auto copy = *this;
            ++this->_ptr;
            return copy;
        }

        friend constexpr bool operator==(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr == rhs._ptr;
        }

        friend constexpr bool operator!=(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr != rhs._ptr;
        }



        /*
         * BidirectionalIterator
         */
        constexpr array_iterator& operator--() noexcept
        {
            --this->_ptr;
            return *this;
        }

        constexpr array_iterator operator--(int) noexcept
        {
            auto copy = *this;
            --this->_ptr;
            return copy;
        }



        /*
         * RandomAccessIterator
         */
        constexpr reference operator[](difference_type index) const noexcept
        {
            return this->_ptr[index];
        }

        constexpr array_iterator& operator+=(difference_type diff) noexcept
        {
            this->_ptr += diff;
            return *this;
        }

        constexpr array_iterator& operator-=(difference_type diff) noexcept
        {
            this->_ptr -= diff;
            return *this;
        }

        friend constexpr array_iterator operator+(array_iterator lhs, difference_type rhs) noexcept
        {
            return lhs += rhs;
        }

        friend constexpr array_iterator operator+(difference_type lhs, array_iterator rhs) noexcept
        {
            return rhs += lhs;
        }

        friend constexpr array_iterator operator-(array_iterator lhs, difference_type rhs) noexcept
        {
            return lhs -= rhs;
        }

        friend constexpr difference_type operator-(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr - rhs._ptr;
        }

        friend constexpr bool operator<(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr < rhs._ptr;
        }

        friend constexpr bool operator<=(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr <= rhs._ptr;
        }

        friend constexpr bool operator>(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr > rhs._ptr;
        }

        friend constexpr bool operator>=(array_iterator lhs, array_iterator rhs) noexcept
        {
            return lhs._ptr >= rhs._ptr;
        }



    private:

        pointer _ptr = nullptr;
    };



    /*
     * const_array_iterator
     */
    template <typename Ty>
    using const_array_iterator = array_iterator<std::add_const_t<Ty>>;
}
