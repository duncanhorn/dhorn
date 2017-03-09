/*
 * Duncan Horn
 *
 * com_ptr.h
 *
 * A throwing COM pointer implementation
 */
#pragma once

#include <cassert>

#include "com_utility.h"
#include "hresult_error.h"

namespace dhorn::com
{
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

        // TODO: Would it be good to have a non-explicit version?
        //com_ptr(element_type* ptr) noexcept
        //{
        //    Assign(ptr);
        //}

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        explicit com_ptr(Ty* ptr) noexcept(std::is_convertible_v<Ty*, IFace*>)
        {
            Assign(ptr);
        }

        // Need to explicitly define a copy assignment operator since the compiler won't like the templated one
        com_ptr(const com_ptr& other) noexcept
        {
            Assign(other._ptr);
        }

        template <typename Ty>
        explicit com_ptr(const com_ptr<Ty>& other) noexcept(std::is_convertible_v<Ty*, IFace*>)
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

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        com_ptr& operator=(Ty* ptr) noexcept(std::is_convertible_v<Ty*, IFace*>)
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

        template <typename Ty>
        com_ptr& operator=(const com_ptr<Ty>& other) noexcept(std::is_convertible_v<Ty*, IFace*>)
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

        // TODO??
        //explicit operator element_type*() const noexcept
        //{
        //    return this->_ptr;
        //}

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

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        void reset(Ty* ptr) noexcept(std::is_convertible_v<Ty*, IFace*>)
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
            return com_ptr<Ty>(this->_ptr);
        }

        template <typename Ty, std::enable_if_t<is_unknown_v<Ty>, int> = 0>
        void copy_to(Ty** ptr) noexcept(std::is_convertible_v<IFace*, Ty*>)
        {
            *ptr = nullptr;
            *ptr = com_ptr<Ty>(this->_ptr).detach();
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



        element_type* _ptr = nullptr;
    };
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
