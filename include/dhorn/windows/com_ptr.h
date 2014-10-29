/*
 * Duncan Horn
 *
 * com_ptr.h
 *
 * Throwing COM smart pointer
 */
#pragma once

#include <cassert>
#include <type_traits>
#include <Unknwn.h>

#include "windows_exception.h"

namespace dhorn
{
    namespace win32
    {
        /*
         * com_ptr class
         */
        template <typename IFace>
        class com_ptr final
        {
            static_assert(std::is_convertible<IFace *, IUnknown *>::value,
                "You can only declare com_ptr for types that derive from IUnknown");
        public:
            /*
             * Types
             */
            using interface_type = IFace;

            /*
             * Constructor(s)/Destructor
             */
#pragma region Constructor(s)/Destructor

            com_ptr(void) :
                _ptr(nullptr)
            {
            }

            com_ptr(_In_ std::nullptr_t) :
                com_ptr()
            {
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            com_ptr(_In_opt_ Ty *ptr) :
                com_ptr()
            {
                this->Assign(ptr);
            }

            //template <
            //    typename Ty,
            //    typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            //com_ptr(_In_ Ty *&&ptr) :
            //    com_ptr()
            //{
            //    this->Assign(std::move(ptr));
            //}

            com_ptr(_In_ const com_ptr &other) :
                com_ptr()
            {
                this->Assign(other._ptr);
            }

            template <typename Ty>
            com_ptr(_In_ const com_ptr<Ty> &other) :
                com_ptr()
            {
                this->Assign(other._ptr);
            }

            com_ptr(_Inout_ com_ptr &&other) :
                com_ptr()
            {
                this->swap(other);
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, interface_type *>::value>::type>
            com_ptr(_Inout_ com_ptr<Ty> &&other) :
                _ptr(other._ptr)
            {
                other._ptr = nullptr;
            }

            ~com_ptr(void)
            {
                this->Release();
            }

#pragma endregion



            /*
             * Operators
             */
            com_ptr &operator=(_In_ nullptr_t)
            {
                this->Release();
                return *this;
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            com_ptr &operator=(_In_opt_ Ty *ptr)
            {
                // We cannot call swap() here, because the assignment might throw. In the case that
                // it does, we would not release as we are expected to do.
                this->Release();
                this->Assign(ptr);

                return *this;
            }

            com_ptr &operator=(_In_ const com_ptr &other)
            {
                if (this != &other)
                {
                    com_ptr(other).swap(*this);
                }

                return *this;
            }

            template <typename Ty>
            com_ptr &operator=(_In_ const com_ptr<Ty> &other)
            {
                // We cannot call swap() here, because the assignment might throw. In the case that
                // it does, we would not release as we are expected to do.
                this->Release();
                this->Assign(other._ptr);

                return *this;
            }

            com_ptr &operator=(_Inout_ com_ptr &&other)
            {
                assert(this != &other);
                this->swap(other);

                return *this;
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, interface_type *>::value>::type>
            com_ptr &operator=(_Inout_ com_ptr<Ty> &&other)
            {
                // We do not call swap here as it would require an extra AddRef and release since
                // we would need to create a separate object
                this->Release();
                this->_ptr = other._ptr;
                other._ptr = nullptr;

                return *this;
            }

            //template <
            //    typename Ty,
            //    typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            //com_ptr &operator=(_In_ Ty *&&ptr)
            //{
            //    this->Release();
            //    this->Assign(std::move(ptr));

            //    return *this;
            //}

            operator bool(void) const
            {
                return this->_ptr != nullptr;
            }

            operator interface_type *(void) const
            {
                return this->_ptr;
            }

            interface_type *operator->(void)
            {
                return this->_ptr;
            }

            interface_type &operator*(void)
            {
                return *this->_ptr;
            }



            void assign(nullptr_t)
            {
                this->Release();
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            void assign(_In_opt_ Ty *ptr)
            {
                this->Release();
                this->Assign(ptr);
            }

            void attach(nullptr_t)
            {
                this->Release();
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            void attach(_In_opt_ Ty *ptr)
            {
                this->Release();
                this->Attach(ptr);
            }

            interface_type *detach(void)
            {
                auto ptr = this->_ptr;
                this->_ptr = nullptr;
                return ptr;
            }

            void reset(void)
            {
                this->Release();
            }

            void swap(_Inout_ com_ptr &other)
            {
                auto temp = this->_ptr;
                this->_ptr = other._ptr;
                other._ptr = temp;
            }

            interface_type *get(void) const
            {
                return this->_ptr;
            }

            interface_type **get_address_of(void)
            {
                return &this->_ptr;
            }

            interface_type **release_and_get_address_of(void)
            {
                this->Release();
                return this->get_address_of();
            }



            template <typename Ty>
            friend class com_ptr;
        private:

            inline void Release(void)
            {
                if (this->_ptr)
                {
                    auto ptr = this->_ptr;
                    this->_ptr = nullptr;
                    ptr->Release();
                }
            }

            inline void AddRef(void)
            {
                if (this->_ptr)
                {
                    this->_ptr->AddRef();
                }
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, interface_type *>::value>::type>
            inline void Assign(_In_opt_ Ty *ptr)
            {
                assert(!this->_ptr);
                this->_ptr = ptr;
                this->AddRef();
            }

            template <
                typename Ty,
                typename = typename std::enable_if<!std::is_convertible<Ty *, interface_type *>::value>::type,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            inline void Assign(_In_opt_ Ty *ptr)
            {
                assert(!this->_ptr);
                if (ptr)
                {
                    throw_if_failed(ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr)));
                }
            }

            template <
                typename Ty,
                typename = typename std::enable_if<std::is_convertible<Ty *, interface_type *>::value>::type>
            inline void Attach(_In_opt_ Ty *ptr)
            {
                // Easy case: we can just do simple assignment
                assert(!this->_ptr);
                this->_ptr = ptr;
            }

            template <
                typename Ty,
                typename = typename std::enable_if<!std::is_convertible<Ty *, interface_type *>::value>::type,
                typename = typename std::enable_if<std::is_convertible<Ty *, IUnknown *>::value>::type>
            inline void Attach(_In_opt_ Ty *ptr)
            {
                // More complicated case: we must QI and Release, even if we fail
                assert(!this->_ptr);

                if (ptr)
                {
                    HRESULT hr = ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr));
                    ptr->Release();

                    throw_if_failed(hr);
                }
            }



            IFace *_ptr;
        };
    }
}

#ifndef _DHORN_NO_STD

namespace std
{
    template <typename Ty>
    inline void swap(
        _Inout_ dhorn::win32::com_ptr<Ty> &lhs,
        _Inout_ dhorn::win32::com_ptr<Ty> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif  /*_DHORN_NO_STD*/
