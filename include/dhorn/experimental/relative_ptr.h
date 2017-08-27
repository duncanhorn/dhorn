/*
 * Duncan Horn
 *
 * relative_ptr.h
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace dhorn::experimental
{
    /*
     * relative_ptr
     *
     * TODO: Make a decision about alignment
     * TODO: How should we handle arrays? Like `unique_ptr` with an extent?
     */
    template <typename Ty, typename OffsetTy>
    class relative_ptr
    {
        static_assert(std::is_integral_v<OffsetTy>);

    public:
        /*
         * std::iterator_traits Type Alias Support
         *
         * TODO: Currently `const` is treated as the _this_ pointer is const, not what we're pointing to. I.e. if we
         * want to change to represent a `const` pointer we'd have to construct a `relative_ptr<const Ty>`. That is, the
         * `const` does not propagate. We are considering this acceptable
         */
        using difference_type = std::ptrdiff_t; // TODO: Relate to OffsetTy?
        using value_type = Ty;
        using pointer = Ty*;
        using reference = Ty&;
        using iterator_category = std::random_access_iterator_tag;



        /*
         * Constructor(s)/Destructor
         */
        relative_ptr() = default;

        relative_ptr(const relative_ptr& ptr) :
            _offset(calculate_offset(ptr.get()))
        {
            assert(get() == ptr.get());
        }

        template <
            typename OtherTy,
            typename OtherOffsetTy,
            std::enable_if_t<std::is_convertible<OtherTy*, Ty*>::value, int> = 0>
        relative_ptr(const relative_ptr<OtherTy, OtherOffsetTy>& ptr) :
            _offset(calculate_offset(ptr.get()))
        {
            assert(get() == ptr.get());
        }

        relative_ptr(std::nullptr_t) noexcept
        {
        }

        relative_ptr(pointer ptr) :
            _offset(calculate_offset(ptr))
        {
            assert(get() == ptr);
        }



        /*
         * Modifiers
         */
        relative_ptr& operator=(const relative_ptr& ptr)
        {
            if (this != &ptr)
            {
                reset(ptr.get());
            }

            return *this;
        }

        template <
            typename OtherTy,
            typename OtherOffsetTy,
            std::enable_if_t<std::is_convertible<OtherTy*, Ty*>::value, int> = 0>
        relative_ptr& operator=(const relative_ptr<OtherTy, OtherOffsetTy>& ptr)
        {
            reset(ptr.get());
            return *this;
        }

        relative_ptr& operator+=(difference_type distance)
        {
#if _DEBUG
            auto expected = get() + distance;
#endif

            // NOTE: This is meant to mimic pointer arithmetic
            this->_offset = check_offset(static_cast<difference_type>(this->_offset) + (distance * sizeof(Ty)));
            assert(get() == expected);

            return *this;
        }

        relative_ptr& operator-=(difference_type distance)
        {
#if _DEBUG
            auto expected = get() - distance;
#endif

            // NOTE: This is meant to mimic pointer arithmetic
            this->_offset = check_offset(static_cast<difference_type>(this->_offset) - (distance * sizeof(Ty)));
            assert(get() == expected);

            return *this;
        }

        void reset() noexcept
        {
            this->_offset = 0;
        }

        void reset(std::nullptr_t) noexcept
        {
            this->_offset = 0;
        }

        void reset(pointer ptr)
        {
            this->_offset = 0;
            this->_offset = calculate_offset(ptr);
            assert(get() == ptr);
        }

        template <typename OtherOffsetTy>
        void swap(relative_ptr<Ty, OtherOffsetTy>& other)
        {
            auto thisPtr = get();
            auto otherPtr = other.get();

            // Calculate the offsets before doing the swap so that we have the strong exception guarantee that neither
            // state is changed if an exception gets thrown
            auto thisOffset = calculate_offset(otherPtr);
            auto otherOffset = other.calculate_offset(thisPtr);

            this->_offset = thisOffset;
            other._offset = otherOffset;
        }



        /*
         * Observers
         */
        reference operator*() const noexcept
        {
            return *get();
        }

        pointer operator->() const noexcept
        {
            return get();
        }

        reference operator[](difference_type index) const noexcept
        {
            return get()[index];
        }

        explicit operator bool() const noexcept
        {
            return this->_offset != 0;
        }

        pointer get() const noexcept
        {
            if (!this->_offset)
            {
                // Pointer to `this` can't possibly point to `Ty`, so we map it to `nullptr`
                return nullptr;
            }

            // The behavior of `relative_ptr` is intended to mimic raw pointers. That is, if `this` is `const`, then we
            // should behave like a `const` pointer (i.e. `Ty* const`) as opposed to a pointer to `const` (i.e.
            // `const Ty*`), hence the `const_cast`.
            auto bytePtr = const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(this));
            bytePtr += this->_offset;
            return reinterpret_cast<pointer>(bytePtr);
        }



    private:

        OffsetTy check_offset(std::ptrdiff_t offset) const
        {
            auto result = static_cast<OffsetTy>(offset);
            if (offset != static_cast<std::ptrdiff_t>(result))
            {
                throw std::range_error("Pointer offset too large to be represented by relative_ptr");
            }

            return result;
        }

        OffsetTy calculate_offset(pointer ptr) const
        {
            if (ptr)
            {
                auto diff = reinterpret_cast<const std::uint8_t*>(ptr) - reinterpret_cast<const std::uint8_t*>(this);
                return check_offset(diff);
            }

            // We allow representing null as a relative_ptr that "points to itself"
            return 0;
        }



        OffsetTy _offset = 0;
    };



    /*
     * Public Aliases
     */
    template <typename Ty>
    using relative_ptr8 = relative_ptr<Ty, std::int8_t>;

    template <typename Ty>
    using relative_ptr16 = relative_ptr<Ty, std::int16_t>;

    template <typename Ty>
    using relative_ptr32 = relative_ptr<Ty, std::int32_t>;

    template <typename Ty>
    using relative_ptr64 = relative_ptr<Ty, std::int64_t>;



    /*
     * ADL Functions
     */
    template <typename Ty, typename OffsetTy>
    void swap(relative_ptr<Ty, OffsetTy>& lhs, relative_ptr<Ty, OffsetTy>& rhs)
    {
        lhs.swap(rhs);
    }
}
