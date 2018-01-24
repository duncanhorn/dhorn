/*
 * Duncan Horn
 *
 * inplace_function.h
 *
 * Intended to function like std::experimental::inplace_function. That is, it's more-or-less functionally equivalent to
 * std::function only it *never* allocates memory, failing to compile if the size of the underlying function object is
 * larger than the space designated within the inplace_function object.
 */
#pragma once

#include <cassert>
#include <functional>

#include "../scope_guard.h"

namespace dhorn::experimental
{
    /*
     * inplace_function Declaration
     */
    template <typename Func, std::size_t Size = (8 * sizeof(void*))>
    class inplace_function;



    namespace details
    {
        /*
         * is_inplace_function
         */
        template <typename Ty>
        struct is_inplace_function : std::false_type
        {
        };

        template <typename Func, std::size_t Size>
        struct is_inplace_function<inplace_function<Func, Size>> : std::true_type
        {
        };



        /*
         * is_inplace_function_constructible
         */
        template <typename InplaceFunction, typename Func>
        struct is_inplace_function_constructible;

        template <typename ReturnTy, typename... ArgsTy, std::size_t Size, typename Func>
        struct is_inplace_function_constructible<inplace_function<ReturnTy(ArgsTy...), Size>, Func> : std::conjunction<
            std::negation<is_inplace_function<Func>>,
            std::is_invocable_r<ReturnTy, Func, ArgsTy...>>
        {
        };



        /*
         * is_function_null
         */
        template <typename Func>
        inline bool is_function_null(const Func& func, std::true_type)
        {
            return !func;
        }

        template <typename Func>
        inline bool is_function_null(const Func&, std::false_type)
        {
            return false;
        }

        template <typename Func>
        inline bool is_function_null(const Func& func)
        {
            using is_pointer = std::disjunction<
                std::is_member_pointer<Func>,
                std::conjunction<
                    std::is_pointer<Func>,
                    std::is_function<std::remove_pointer_t<Func>>
                >>;
            return is_function_null(func, is_pointer{});
        }



        /*
         * function_base
         *
         * The base invokable type used by inplace_function
         */
        template <typename ReturnTy, typename... ArgsTy>
        struct function_base
        {
            virtual ~function_base() {}

            virtual function_base* copy(void*) = 0;
            virtual function_base* move(void*) = 0;
            virtual ReturnTy invoke(ArgsTy&&...) = 0;
            virtual const std::type_info& target_type() noexcept = 0;
            virtual void* target(const std::type_info& type) noexcept = 0;
        };
    }



    /*
     * inplace_function
     */
    template <typename ReturnTy, typename... ArgsTy, std::size_t Size>
    class inplace_function<ReturnTy(ArgsTy...), Size>
    {
        template <typename, std::size_t>
        friend class inplace_function;

        using function_base = details::function_base<ReturnTy, ArgsTy...>;

        template <typename FuncTy>
        struct function_impl : function_base
        {
            template <typename Func>
            function_impl(Func&& func) :
                func(std::forward<Func>(func))
            {
            }

            virtual function_base* copy(void* addr) override
            {
                return ::new (addr) function_impl(func);
            }

            virtual function_base* move(void* addr) override
            {
                return ::new (addr) function_impl(std::move(func));
            }

            virtual ReturnTy invoke(ArgsTy&&... args) override
            {
                return std::invoke(this->func, std::forward<ArgsTy>(args)...);
            }

            virtual const std::type_info& target_type() noexcept override
            {
                return typeid(FuncTy);
            }

            virtual void* target(const std::type_info& type) noexcept override
            {
                return (typeid(func) == type) ? &func : nullptr;
            }

            FuncTy func;
        };

    public:
        /*
         * Public Types/Constants
         */
        using result_type = ReturnTy;
        static constexpr std::size_t max_size = Size;



        /*
         * Constructor(s)/Destructor
         */
        inplace_function() noexcept = default;

        inplace_function(std::nullptr_t) noexcept
        {
        }

        inplace_function(const inplace_function& other)
        {
            copy(other);
        }

        template <std::size_t SmallerSize, std::enable_if_t<(SmallerSize <= Size), int> = 0>
        inplace_function(const inplace_function<ReturnTy(ArgsTy...), SmallerSize>& other)
        {
            copy(other);
        }

        inplace_function(inplace_function&& other)
        {
            move(std::move(other));
        }

        template <std::size_t SmallerSize, std::enable_if_t<(SmallerSize <= Size), int> = 0>
        inplace_function(inplace_function<ReturnTy(ArgsTy...), SmallerSize>&& other)
        {
            move(std::move(other));
        }

        template <
            typename Func,
            std::enable_if_t<details::is_inplace_function_constructible<inplace_function, Func>::value, int> = 0>
        inplace_function(Func func)
        {
            set(std::move(func));
        }

        ~inplace_function()
        {
            reset();
        }



        /*
         * Operators
         */
        inplace_function& operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }

        inplace_function& operator=(const inplace_function& other)
        {
            if (this != &other)
            {
                reset();
                copy(other);
            }

            return *this;
        }

        inplace_function& operator=(inplace_function&& other)
        {
            reset();
            move(std::move(other));
            return *this;
        }

        template <
            typename Func,
            std::enable_if_t<details::is_inplace_function_constructible<inplace_function, std::decay_t<Func>>::value, int> = 0>
        inplace_function& operator=(Func&& func)
        {
            reset();
            set(std::forward<Func>(func));
            return *this;
        }

        ReturnTy operator()(ArgsTy... args)
        {
            if (!this->_func)
            {
                throw std::bad_function_call();
            }

            return this->_func->invoke(std::forward<ArgsTy>(args)...);
        }

        explicit operator bool() const noexcept
        {
            return this->_func != nullptr;
        }

        friend bool operator==(const inplace_function& func, std::nullptr_t) noexcept
        {
            return func._func == nullptr;
        }

        friend bool operator==(std::nullptr_t, const inplace_function& func) noexcept
        {
            return func._func == nullptr;
        }

        friend bool operator!=(const inplace_function& func, std::nullptr_t) noexcept
        {
            return func._func != nullptr;
        }

        friend bool operator!=(std::nullptr_t, const inplace_function& func) noexcept
        {
            return func._func != nullptr;
        }



        /*
         * Modifiers
         */
        void swap(inplace_function& other)
        {
            // TODO: Is it more efficient to manually control references?
#if 1
            inplace_function temp(std::move(other));
            other = std::move(*this);
            *this = std::move(temp);
#else
            if (other._func)
            {
                if (this->_func)
                {
                    std::uint8_t tempStorage[Size];
                    auto temp = this->_func->move(tempStorage);
                    auto destroy_temp = make_scope_guard([&]()
                    {
                        temp->~function_base();
                    });
                    destroy();

                    this->_func = other._func->move(this->_data);
                    other.destroy();

                    other._func = temp->move(other._data);
                }
                else
                {
                    this->_func = other._func->move(this->_data);
                    other.destroy();
                }
            }
            else if (this->_func)
            {
                other._func = this->_func->move(other._data);
                destroy();
            }
#endif
        }



        /*
         * Accessors
         */
        const std::type_info& target_type()
        {
            if (this->_func)
            {
                return this->_func->target_type();
            }

            return typeid(void);
        }

        template <typename Func>
        Func* target() noexcept
        {
            if (this->_func)
            {
                return reinterpret_cast<Func*>(this->_func->target(typeid(Func)));
            }

            return nullptr;
        }

        template <typename Func>
        const Func* target() const noexcept
        {
            if (this->_func)
            {
                return reinterpret_cast<const Func*>(this->_func->target(typeid(Func)));
            }

            return nullptr;
        }



    private:

        template <typename Func>
        void set(Func&& func)
        {
            assert(!this->_func);
            if (!details::is_function_null(func))
            {
                using impl_type = function_impl<std::decay_t<Func>>;
                static_assert(sizeof(impl_type) <= Size, "Function object too large for inlpace_function." \
                    " Either reduce the object's size or use a larger sized inplace_function");

                this->_func = ::new (this->_data) impl_type(std::forward<Func>(func));
            }
        }

        void destroy()
        {
            assert(this->_func);

            // If an exception gets thrown, let it propagate, but leave us in a state that says we cleaned it all up
            auto func = this->_func;
            this->_func = nullptr;
            func->~function_base();
        }

        void reset()
        {
            if (this->_func)
            {
                destroy();
            }
        }

        void copy(const inplace_function& other)
        {
            assert(!this->_func);
            if (other._func)
            {
                this->_func = other._func->copy(this->_data);
            }
        }

        template <std::size_t SmallerSize>
        void copy(const inplace_function<ReturnTy(ArgsTy...), SmallerSize>& other)
        {
            assert(!this->_func);
            if (other._func)
            {
                this->_func = other._func->copy(this->_data);
            }
        }

        void move(inplace_function&& other)
        {
            assert(!this->_func);
            if (other._func)
            {
                this->_func = other._func->move(this->_data);

                // If an exception gets thrown, it would have been in the move constructor. In that case, assume that no
                // state has changed. If it succeeded, we don't know if the underlying function is in a usable state, so
                // reset the moved-from function
                other.reset();
            }
        }

        template <std::size_t SmallerSize>
        void move(inplace_function<ReturnTy(ArgsTy...), SmallerSize>&& other)
        {
            assert(!this->_func);
            if (other._func)
            {
                this->_func = other._func->move(this->_data);
                other.reset();
            }
        }



        function_base* _func = nullptr;
        union
        {
            std::max_align_t _;
            std::uint8_t _data[Size];
        };
    };



    /*
     * swap
     */
    template <typename Func, std::size_t Size>
    void swap(inplace_function<Func, Size>& lhs, inplace_function<Func, Size>& rhs)
    {
        lhs.swap(rhs);
    }
}
