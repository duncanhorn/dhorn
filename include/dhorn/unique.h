/*
 * Duncan Horn
 *
 * unique.h
 *
 * The `dhorn::unique` type is a generic resource management type modeled after `std::unique_ptr`, but with the
 * difference that it can manage _any_ type, not just pointers. The `unique` type takes two template arguments:
 *
 *      `Ty`            Represents the "effective type" or "semantic type" of the `unique` type. Objects are said to
 *                      "act like" this type. For example, a template argument of `int*` means that the object "acts
 *                      like" a pointer to a single integer. Similarly, a type of `int(*)[]` "acts like" a pointer to an
 *                      array of integers. Unlike `std::unique_ptr`, a type of `T[]` for some `T` does _not_ "act like"
 *                      a pointer to an array of `T`s.
 *      `Traits`        Describes how the underlying resource type is managed. The full functionality of this type is
 *                      decribed later, but in general this type described how the type is stored, how the resource's
 *                      lifetime is managed, and properties of this type such as validity and default construction.
 *
 * Each `unique` object provides several type aliases that are summarized as follows
 *
 *      value_type      The type that is stored internally to the `unique` object. Operations that construct or modify
 *                      a `unique` object accept `value_type` (or an l/r-value reference to `value_type`) as the
 *                      argument type. Additionally, accessor functions return objects of `value_type` (or l/r-value
 *                      references to `value_type`). This is an alias for `Traits::value_type` if it exists. Otherwise
 *                      it is an alias for `Ty`.
 *      semantic_type   An alias for `Ty`. This type is primarily used for overload resolution purposes for construction
 *                      or assignment to the underlying resource member variable.
 *      traits_type     An alias for `Traits`.
 *
 * The traits type is expected has a number of required and optional functions/type aliases:
 *
 *      value_type      An optional type alias. If present, is used as the `unique::value_type` alias.
 *      is_valid        A static member function that takes an argument of type `const unique::value_type&` and returns
 *                      a `bool` indicating whether or not the resource is valid. I.e. whether or not the resource is
 *                      something that needs to be cleaned up.
 *      default_value   A static member function that takes no arguments and returns a value of type
 *                      `unique::value_type` - or something that is convertible to `unique::value_type`. This function
 *                      is used for operations such as default construction and resetting the value. Note that values
 *                      returned by this function need not be considered "invalid" by the `is_valid` function.
 *      operator()      A non-static member function that takes in arguments of type `unique::value_type&` and is
 *                      expected to destroy/release that resource.
 */
#pragma once

#include <memory>
#include <tuple>

#include "compressed_pair.h"

namespace dhorn
{
    /*
     * unique Traits Types
     */
#pragma region unique Traits Types

    /*
     * Helpers
     */
    namespace details
    {
        /*
         * unique_value_type_from_deleter
         */
        template <typename Deleter, typename Ty, typename = void>
        struct unique_value_type_from_deleter
        {
            // Fallback case
            using type = Ty*;
        };

        template <typename Deleter, typename Ty>
        struct unique_value_type_from_deleter<Deleter, Ty, std::void_t<typename Deleter::pointer>>
        {
            using type = typename Deleter::pointer;
        };

        template <typename Deleter, typename Ty>
        using unique_value_type_from_deleter_t = typename unique_value_type_from_deleter<Deleter, Ty>::type;
    }



    /*
     * unique_pointer_traits
     */
    template <typename Ty, typename Deleter = std::default_delete<Ty>>
    struct unique_pointer_traits :
        private details::compressed_base<Deleter>
    {
    private:
        using deleter_base = details::compressed_base<Deleter>;

    public:
        /*
         * Type Aliases
         */
        using value_type = details::unique_value_type_from_deleter_t<Deleter, Ty>;



        /*
         * Constructor(s)/Destructor
         */
        using deleter_base::deleter_base;



        /*
         * Destroy Operator
         */
        void operator()(value_type ptr) const
        {
            deleter_base::value()(ptr);
        }



        /*
         * Static Traits Functions
         */
        static constexpr bool is_valid(value_type ptr) noexcept
        {
            return ptr != nullptr;
        }

        static constexpr value_type default_value() noexcept
        {
            return nullptr;
        }
    };



    /*
     * unique_array_traits
     */
    template <typename Ty, typename Deleter = std::default_delete<Ty[]>>
    struct unique_array_traits :
        private details::compressed_base<Deleter>
    {
    private:
        using deleter_base = details::compressed_base<Deleter>;

    public:
        /*
         * Type Aliases
         */
        using value_type = details::unique_value_type_from_deleter_t<Deleter, Ty>;



        /*
         * Construction/Assignment Enablement
         *
         * Follows the same rules as std::unique_ptr. Same rules apply to both construction and assignment
         */
        template <typename ArgTy>
        struct is_convertible :
            std::disjunction<
                std::is_same<std::remove_reference_t<ArgTy>, value_type>,
                std::is_same<std::remove_reference_t<ArgTy>, std::nullptr_t>,
                std::conjunction<
                    std::is_same<value_type, Ty*>,
                    std::is_pointer<std::remove_reference_t<ArgTy>>,
                    std::is_convertible<std::remove_pointer_t<std::remove_reference_t<ArgTy>>(*)[], Ty(*)[]>
                >
            >
        {
        };

        template <typename ArgTy>
        struct is_constructible :
            public is_convertible<ArgTy>
        {
        };



        /*
         * Constructor(s)/Destructor
         */
        using deleter_base::deleter_base;



        /*
         * Destroy Operator
         */
        template <typename Other, std::enable_if_t<std::is_convertible<Other(*)[], Ty(*)[]>::value, int> = 0>
        void operator()(Other* ptr) const
        {
            deleter_base::value()(ptr);
        }



        /*
         * Static Traits Functions
         */
        static constexpr bool is_valid(value_type ptr) noexcept
        {
            return ptr != nullptr;
        }

        static constexpr value_type default_value() noexcept
        {
            return nullptr;
        }
    };



    /*
     * unique_traits
     */
    template <typename Ty>
    struct unique_traits;

    template <typename Ty>
    struct unique_traits<Ty*> :
        public unique_pointer_traits<Ty>
    {
    };

    template <typename Ty>
    struct unique_traits<Ty(*)[]> :
        public unique_array_traits<Ty>
    {
    };

#pragma endregion



    /*
     * unique
     */
#pragma region unique

    /*
     * Helpers
     */
    namespace details
    {
        /*
         * value_type_from_unique_traits
         *
         * TraitsTy::value_type if it exists, else Ty
         */
        template <typename TraitsTy, typename Ty, typename = void>
        struct value_type_from_unique_traits
        {
            // Fallback case
            using type = Ty;
        };

        template <typename TraitsTy, typename Ty>
        struct value_type_from_unique_traits<TraitsTy, Ty, std::void_t<typename TraitsTy::value_type>>
        {
            using type = typename TraitsTy::value_type;
        };

        template <typename TraitsTy, typename Ty>
        using value_type_from_unique_traits_t = typename value_type_from_unique_traits<TraitsTy, Ty>::type;



        /*
         * is_unique_constructible
         *
         * TraitsTy::is_constructible<ArgTy> if it exists, else std::is_constructible<ValueTy, ArgTy>
         */
        template <typename TraitsTy, typename ValueTy, typename ArgTy, typename = void>
        struct is_unique_constructible :
            public std::is_constructible<ValueTy, ArgTy> // Fallback case
        {
        };

        template <typename TraitsTy, typename ValueTy, typename ArgTy>
        struct is_unique_constructible<TraitsTy, ValueTy, ArgTy, std::void_t<typename TraitsTy::template is_constructible<ArgTy>>> :
            public TraitsTy::template is_constructible<ArgTy>
        {
        };

        template <typename TraitsTy, typename ValueTy, typename ArgTy>
        constexpr bool is_unique_constructible_v = is_unique_constructible<TraitsTy, ValueTy, ArgTy>::value;
    }



    template <typename Ty, typename Traits = unique_traits<Ty>>
    class unique
    {
        // We need to invoke static functions/type traits on the traits type, but we allow pointers/references
        using traits_type = std::remove_reference_t<std::remove_pointer_t<Traits>>;

    public:
        /*
         * Public Types
         */
        using value_type = details::value_type_from_unique_traits_t<Traits, Ty>;
        using semantic_type = Ty;



        /*
         * Constructor(s)/Destructor
         */
        constexpr unique() noexcept :
            _data(traits_type::default_value(), Traits{})
        {
            static_assert(!std::is_pointer_v<Traits>,
                "Construcing a unique object with a null traits object is ill-formed");
        }

        template <
            typename ArgTy,
            std::enable_if_t<details::is_unique_constructible<traits_type, value_type, ArgTy&&>::value, int> = 0>
        explicit unique(ArgTy&& value) :
            _data(std::forward<ArgTy>(value), Traits{})
        {
            static_assert(!std::is_pointer_v<Traits>,
                "Construcing a unique object with a null traits object is ill-formed");
        }

        template <
            typename ArgTy,
            std::enable_if_t<details::is_unique_constructible<traits_type, value_type, ArgTy&&>::value, int> = 0>
        unique(ArgTy&& value, std::conditional_t<std::is_reference<Traits>::value, Traits, const Traits&> traits) :
            _data(std::forward<ArgTy>(value), traits)
        {
        }

        template <
            typename ArgTy,
            typename TraitsTy = Traits,
            std::enable_if_t<details::is_unique_constructible<traits_type, value_type, ArgTy&&>::value, int> = 0,
            std::enable_if_t<!std::is_reference<TraitsTy>::value, int> = 0>
        unique(ArgTy&& value, Traits&& traits) :
            _data(std::forward<ArgTy>(value), std::move(traits))
        {
        }

        ~unique()
        {
            reset();
        }



        /*
         * Modifiers
         */
        value_type release() noexcept // <-- TOOD: Should this be conditional?
        {
            value_type result = traits_type::default_value();
            std::swap(result, my_value());
            return result;
        }

        void reset() noexcept // <-- TODO: Should this be conditional
        {
            value_type value = traits_type::default_value();
            std::swap(value, my_value());
            my_traits()(value);
        }

        template <typename Type, std::enable_if_t<std::is_convertible<const Type&, value_type>::value, int> = 0>
        void reset(const Type& value) noexcept // <-- TODO: Should this be conditional
        {
            my_traits()(my_value());
            my_value() = value;
        }

        void reset(value_type&& value) noexcept // <-- TODO: Should this be conditional
        {
            my_traits()(my_value());
            my_value() = std::move(value);
        }



        /*
         * Observers
         */



    private:

#pragma region Member Accessors

        constexpr value_type& my_value() noexcept
        {
            return this->_data.first();
        }

        constexpr const value_type& my_value() const noexcept
        {
            return this->_data.first();
        }

        constexpr Traits& my_traits() noexcept
        {
            return this->_data.second();
        }

        constexpr const Traits& my_traits() const noexcept
        {
            return this->_data.second();
        }

#pragma endregion

        compressed_pair<value_type, Traits> _data;
    };

#pragma endregion



    /*
     * unique Specializations
     */
#pragma region unique Specializations

    /*
     * Helpers
     */
    namespace details
    {
        template <typename Ty>
        struct select_pointer_for_unique
        {
            using type = Ty*;
        };

        template <typename Ty>
        struct select_pointer_for_unique<Ty[]>
        {
            using type = Ty(*)[];
        };

        template <typename Ty>
        using select_pointer_for_unique_t = typename select_pointer_for_unique<Ty>::type;
    }



    /*
     * unique_ptr
     */
    template <typename Ty>
    using unique_ptr = unique<details::select_pointer_for_unique_t<Ty>>;

#pragma endregion
}
