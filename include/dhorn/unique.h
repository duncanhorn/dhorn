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
 * 
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
        using value_type = Ty*;



        /*
         * Constructor(s)/Destructor
         */
        using deleter_base::deleter_base;



        /*
         * Destroy Operator
         */
        void operator()(value_type ptr)
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
        using value_type = Ty*;



        /*
         * Constructor(s)/Destructor
         */
        using deleter_base::deleter_base;



        /*
         * Destroy Operator
         */
        void operator()(value_type ptr)
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
    }



    template <typename Ty, typename Traits = unique_traits<Ty>>
    class unique
    {
    public:
        /*
         * Public Types
         */
        using traits_type = Traits;
        using value_type = details::value_type_from_unique_traits_t<Traits, Ty>;
        using semantic_type = Ty;



        /*
         * Constructor(s)/Destructor
         */
        constexpr unique() noexcept :
            _data()
        {
        }

        template <typename Type, std::enable_if_t<std::is_constructible<Ty, const Type&>::value, int> = 0>
        explicit unique(const Type& value) :
            _data(value, traits_type{})
        {
            static_assert(!std::is_pointer_v<Traits>,
                "Construcing a unique object with a null traits object is ill-formed");
        }

        //explicit unique(const value_type& value) :
        //    _data(value, traits_type{})
        //{
        //    static_assert(!std::is_pointer_v<Traits>,
        //        "Construcing a unique object with a null traits object is ill-formed");
        //}

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

        constexpr value_type& my_value() noexcept
        {
            return this->_data.first();
        }

        constexpr traits_type& my_traits() noexcept
        {
            return this->_data.second();
        }

        compressed_pair<value_type, traits_type> _data;
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
