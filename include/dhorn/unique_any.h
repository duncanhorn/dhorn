/*
 * Duncan Horn
 *
 * unique_any.h
 *
 * 
 */
#pragma once

#include <memory>
#include <tuple>

namespace dhorn
{
    /*
     * unique_any Traits Types
     *
     * Expected to provide the following type aliases:
     *      * value_type    This is the type returned by `unique_any` to its caller. In general, this should be the same
     *                      as the template type, but there are some obvious exceptions (e.g. arrays).
     *      * storage_type  This is the type that `unique_any` stores internally and is the type that gets passed to the
     *                      static functions on the traits type. This allows things like custom deleters, etc. without
     *                      dirtying the user-facing API.
     *
     * Expected to provide the following static member functions:
     *      * default_value Returns a `storage_type` object that represents the value of a default constructed
     *                      `unique_any` object. Note that unlike the `construct` function, the return value of this
     *                      function is _not_ a `std::tuple` since it is used for `reset`, etc. as well.
     *      * construct     Returns a `std::tuple` that is used to construct the `storage_type` member. In general, this
     *                      should return `std::forward_as_tuple` for the input argument(s). Note that this function
     *                      function is _not_ called for the default constructor (`default_value` is).
     *      * destroy       
     *      * valid         Tests a `storage_type` value to see if it contains a valid value (e.g. non-null, etc.).
     */
#pragma region unique_any Traits Types

    /*
     * unique_pointer_traits
     */
    template <typename Ty, typename Deleter = std::default_delete<Ty>>
    struct unique_pointer_traits
    {
        using value_type = Ty*;
    };



    /*
     * unique_any_traits
     */
    template <typename Ty>
    struct unique_any_traits;

    template <typename Ty>
    struct unique_any_traits<Ty*>
    {
        using value_type = Ty*;

        static constexpr bool valid(value_type ptr) noexcept
        {
            return (ptr != nullptr);
        }

        static constexpr value_type default_value() noexcept
        {
            return nullptr;
        }

        static void destroy(value_type ptr) noexcept
        {
            delete ptr;
        }
    };

    template <typename Ty>
    struct unique_any_traits<Ty[]> :
        public unique_any_traits<Ty*>
    {
        static void destroy(Ty* ptr) noexcept
        {
            delete[] ptr;
        }
    };

#pragma endregion



    /*
     * unique_any
     */
    template <typename Ty, typename Traits = unique_any_traits<Ty>>
    class unique_any
    {
        // Validate properties on the traits type
        static_assert(std::is_constructible_v<Ty, decltype(Traits::default_value())>,
            "Must be able to construct value_type with the result of default_value");
        static_assert(std::is_assignable_v<Ty&, decltype(Traits::default_value())>,
            "Must be able to assign the result of default_value to value_type");

    public:
        /*
         * Public Types
         */
        using value_type = typename Traits::value_type;



        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        // Default/no-arg Construction
        template <std::enable_if_t<std::is_convertible_v<decltype(Traits::default_value()), value_type>, int> = 0>
        constexpr unique_any() noexcept(std::is_nothrow_constructible_v<value_type, decltype(Traits::default_value())>) :
            _value(Traits::default_value())
        {
        }

        template <std::enable_if_t<!std::is_convertible_v<decltype(Traits::default_value()), value_type>, int> = 0>
        explicit constexpr unique_any() noexcept(std::is_nothrow_constructible_v<value_type, decltype(Traits::default_value())>) :
            _value(Traits::default_value())
        {
        }

        // Value Construction
        //template <
        //    std::enable_if_t<std::is_copy_constructible_v<value_type>, int> = 0,
        //    std::enable_if_t<std::is_convertible_v<const value_type&, value_type>, int> = 0>
        //constexpr unique_any(const value_type& value) noexcept(std::is_nothrow_copy_constructible_v<value_type>) :
        //    _value(value)
        //{
        //}
        //
        //template <
        //    std::enable_if_t<std::is_copy_constructible_v<value_type>, int> = 0,
        //    std::enable_if_t<!std::is_convertible_v<const value_type&, value_type>, int> = 0>
        //explicit constexpr unique_any(const value_type& value) noexcept(std::is_nothrow_copy_constructible_v<value_type>) :
        //    _value(value)
        //{
        //}

        // R-Value Construction
        template <
            typename OtherTy,
            std::enable_if_t<std::is_constructible_v<value_type, OtherTy&&>, int> = 0,
            std::enable_if_t<std::is_convertible_v<OtherTy&&, value_type>, int> = 0>
        constexpr unique_any(OtherTy&& value) noexcept(std::is_nothrow_constructible_v<value_type, OtherTy&&>) :
            _value(std::forward<OtherTy>(value))
        {
        }

        template <
            typename OtherTy,
            std::enable_if_t<std::is_constructible_v<value_type, OtherTy&&>, int> = 0,
            std::enable_if_t<!std::is_convertible_v<OtherTy&&, value_type>, int> = 0>
        explicit constexpr unique_any(OtherTy&& value) noexcept(std::is_nothrow_constructible_v<value_type, OtherTy&&>) :
            _value(std::forward<OtherTy>(value))
        {
        }

        // Cannot copy unique objects
        unique_any(const unique_any&) = delete;

        // Move Construction
        template <
            typename OtherTy,
            std::enable_if_t<std::is_constructible_v<value_type, OtherTy&&>, int> = 0,
            std::enable_if_t<std::is_convertible_v<OtherTy&&, value_type>, int> = 0>
        unique_any(unique_any<OtherTy>&& other) noexcept(std::is_nothrow_constructible_v<value_type, OtherTy&&>) :
            _value(std::move(other._value))
        {
            // TODO: Detach/invalidate other
        }
        
        template <
            typename OtherTy,
            std::enable_if_t<std::is_constructible_v<value_type, OtherTy&&>, int> = 0,
            std::enable_if_t<!std::is_convertible_v<OtherTy&&, value_type>, int> = 0>
        explicit unique_any(unique_any<OtherTy>&& other) noexcept(std::is_nothrow_constructible_v<value_type, OtherTy&&>) :
            _value(std::move(other._value))
        {
            // TODO: Detach/invalidate other
        }

        ~unique_any()
        {
            // TODO: reset();
        }

#pragma endregion



        /*
         * Operators
         */
#pragma region Operators

        unique_any& operator=(const unique_any&) = delete;

        template <
            typename OtherTy,
            std::enable_if_t<std::is_assignable_v<value_type&, OtherTy&&>, int> = 0>
        unique_any& operator=(unique_any<OtherTy>&& other) noexcept(std::is_nothrow_assignable_v<value_type, OtherTy&&>)
        {

        }

        //unique_any& operator=(unique_any&& other) noexcept(swap(other))
        //{
        //    other.swap(*this);
        //    return *this;
        //}

        operator bool() const noexcept
        {
            return Traits::valid(this->_value);
        }

#pragma endregion



        /*
         * 
         */
#pragma region

        void swap(unique_any& other) noexcept(
            std::is_nothrow_move_constructible_v<value_type> &&
            std::is_nothrow_swappable_v<value_type>)
        {
            using std::swap;
            swap(this->_value, other._value);
        }

#pragma endregion



    private:

        value_type _value;
    };
}
