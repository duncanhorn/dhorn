/*
 * Duncan Horn
 *
 * com_ptr.h
 *
 * A throwing COM pointer implementation. The usage of `com_ptr` is intended to be similar to that of `std::shared_ptr`
 * (with obvious differences since we are inter-operating with COM). Construction/assignment/etc. to a `com_ptr` can
 * always be done implicitly if the pointer types are implicitly convertable. E.g. you can assign an `IFoo*` or a
 * `com_ptr<IFoo>` to a `com_ptr<IUnknown>`.
 *
 * Non-implicit conversions can be done explicitly through the `com_ptr::as`/`com_ptr::try_as` and `query`/`try_query`
 * functions. In all four cases, the conversion is done implicitly if possible, otherwise `QueryInterface` is called on
 * the pointer to do the conversion. The non-"try" variants will throw a `std::system_error` with `hresult_category()`
 * if the `QueryInterface` call fails, whereas the "try" variants will swallow the failed `HRESULT` and return null on
 * failure.
 */
#pragma once

#include <cassert>

#include "com_utility.h"
#include "hresult_error.h"

namespace dhorn::com
{
    /*
     * com_ptr
     */
    template <typename IFace>
    class com_ptr
    {
        static_assert(is_unknown_v<IFace>, "You can only use IUnknown types with com_ptr");

        template <typename Ty>
        friend class com_ptr;

    public:
        /*
         * Public Types
         */
        using element_type = IFace;



        /*
         * Constructor(s)/Destructor
         */
#pragma region Constructor(s)/Destructor

        constexpr com_ptr() noexcept
        {
        }

        constexpr com_ptr(std::nullptr_t) noexcept
        {
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr(Ty* ptr) noexcept
        {
            Assign(ptr);
        }

        // Need to explicitly define a copy assignment operator since the compiler won't like the templated one
        com_ptr(const com_ptr& other) noexcept
        {
            Assign(other._ptr);
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr(const com_ptr<Ty>& other) noexcept
        {
            Assign(other._ptr);
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr(com_ptr<Ty>&& other) noexcept
        {
            this->_ptr = other._ptr;
            other._ptr = nullptr;
        }

        ~com_ptr()
        {
            Release();
        }

#pragma endregion



        /*
         * Operators
         */
#pragma region Operators

        com_ptr& operator=(std::nullptr_t) noexcept
        {
            Release();
            return *this;
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr& operator=(Ty* ptr) noexcept
        {
            Release();
            Assign(ptr);
            return *this;
        }

        // Need to explicitly define a copy assignment operator since the compiler won't like the templated one
        com_ptr& operator=(const com_ptr& other) noexcept
        {
            if (&other != this)
            {
                Release();
                Assign(other._ptr);
            }

            return *this;
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr& operator=(const com_ptr<Ty>& other) noexcept
        {
            Release();
            Assign(other._ptr);
            return *this;
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        com_ptr& operator=(com_ptr<Ty>&& other) noexcept
        {
            Release();
            this->_ptr = other._ptr;
            other._ptr = nullptr;
            return *this;
        }

        operator bool() const noexcept
        {
            return this->_ptr != nullptr;
        }

        element_type& operator*() const noexcept
        {
            return *this->_ptr;
        }

        element_type* operator->() const noexcept
        {
            return this->_ptr;
        }

        element_type** operator&() noexcept
        {
            Release();
            return &this->_ptr;
        }

#pragma endregion



        /*
         * Modifiers
         */
#pragma region Modifiers

        void reset() noexcept
        {
            Release();
        }

        void reset(std::nullptr_t) noexcept
        {
            Release();
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void reset(Ty* ptr) noexcept
        {
            Release();
            Assign(ptr);
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void attach(Ty* ptr) noexcept
        {
            Release();
            this->_ptr = ptr;
        }

        element_type* detach() noexcept
        {
            auto ptr = this->_ptr;
            this->_ptr = nullptr;
            return ptr;
        }

        void swap(com_ptr& other) noexcept
        {
            std::swap(this->_ptr, other._ptr);
        }

        element_type** release_and_get_address_of() noexcept
        {
            Release();
            return &this->_ptr;
        }

#pragma endregion



        /*
         * Observers
         */
#pragma region Observers

        element_type* get() const noexcept
        {
            return this->_ptr;
        }

        element_type** address_of() noexcept
        {
            return &this->_ptr;
        }

#pragma endregion



        /*
         * Conversion
         */
#pragma region Conversion

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        com_ptr<Ty> as() noexcept(std::is_convertible_v<IFace*, Ty*>)
        {
            com_ptr<Ty> result;
            result.Assign(this->_ptr);
            return result;
        }

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        com_ptr<Ty> try_as() noexcept
        {
            com_ptr<Ty> result;
            result.TryAssign(this->_ptr);
            return result;
        }

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        void copy_to(Ty** ptr) noexcept(std::is_convertible_v<IFace*, Ty*>)
        {
            *ptr = nullptr;
            *ptr = as<Ty>().detach();
        }

        void copy_to(REFIID iid, void** ptr)
        {
            *ptr = nullptr;
            check_hresult(this->_ptr->QueryInterface(iid, ptr));
        }

#pragma endregion



    private:

        void Release()
        {
            if (this->_ptr)
            {
                this->_ptr->Release();
                this->_ptr = nullptr;
            }
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void Assign(Ty* ptr) noexcept
        {
            assert(!this->_ptr);
            if (ptr)
            {
                ptr->AddRef();
                this->_ptr = ptr;
            }
        }

        template <typename Ty, std::enable_if_t<!std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void Assign(Ty* ptr)
        {
            assert(!this->_ptr);
            if (ptr)
            {
                check_hresult(ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr)));
            }
        }

        template <typename Ty, std::enable_if_t<std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void TryAssign(Ty* ptr) noexcept
        {
            assert(!this->_ptr);
            if (ptr)
            {
                ptr->AddRef();
                this->_ptr = ptr;
            }
        }

        template <typename Ty, std::enable_if_t<!std::is_convertible_v<Ty*, IFace*>, int> = 0>
        void TryAssign(Ty* ptr) noexcept
        {
            assert(!this->_ptr);
            if (ptr)
            {
                if (FAILED(ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr))))
                {
                    this->_ptr = nullptr;
                }
            }
        }



        element_type* _ptr = nullptr;
    };



    /*
     * COM Pointer Casting
     */
#pragma region COM Pointer Casting

    template <
        typename Ty,
        typename FromTy,
        std::enable_if_t<is_unknown_v<Ty>, int> = 0,
        std::enable_if_t<is_unknown_v<FromTy>, int> = 0,
        std::enable_if_t<std::is_convertible_v<FromTy*, Ty*>, int> = 0>
    inline com_ptr<Ty> query(FromTy* ptr) noexcept
    {
        // Pointer is cast-able; no need to QI
        return com_ptr<Ty>(ptr);
    }

    template <
        typename Ty,
        typename FromTy,
        std::enable_if_t<is_unknown_v<Ty>, int> = 0,
        std::enable_if_t<is_unknown_v<FromTy>, int> = 0,
        std::enable_if_t<!std::is_convertible_v<FromTy*, Ty*>, int> = 0>
    inline com_ptr<Ty> query(FromTy* ptr)
    {
        com_ptr<Ty> result;
        check_hresult(ptr->QueryInterface(IID_PPV_ARGS(&result)));
        return result;
    }

    template <
        typename Ty,
        typename FromTy,
        std::enable_if_t<is_unknown_v<Ty>, int> = 0,
        std::enable_if_t<is_unknown_v<FromTy>, int> = 0,
        std::enable_if_t<std::is_convertible_v<FromTy*, Ty*>, int> = 0>
    inline com_ptr<Ty> try_query(FromTy* ptr) noexcept
    {
        // Pointer is cast-able; no need to QI
        return com_ptr<Ty>(ptr);
    }
    
    template <
        typename Ty,
        typename FromTy,
        std::enable_if_t<is_unknown_v<Ty>, int> = 0,
        std::enable_if_t<is_unknown_v<FromTy>, int> = 0,
        std::enable_if_t<!std::is_convertible_v<FromTy*, Ty*>, int> = 0>
    inline com_ptr<Ty> try_query(FromTy* ptr) noexcept
    {
        com_ptr<Ty> result;
        if (SUCCEEDED(ptr->QueryInterface(IID_PPV_ARGS(&result))))
        {
            return result;
        }

        return nullptr;
    }

#pragma endregion
}



#ifndef DHORN_NO_STD

namespace std
{
    template <typename Ty>
    inline void swap(dhorn::com::com_ptr<Ty>& lhs, dhorn::com::com_ptr<Ty>& rhs) noexcept
    {
        lhs.swap(rhs);
    }
}

#endif
