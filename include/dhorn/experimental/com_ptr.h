/*
 * Duncan Horn
 *
 * com_ptr.h
 *
 * Throwing COM smart pointer
 */
#pragma once

#include <cassert>
#include <inspectable.h>
#include <type_traits>

#include "windows_exception.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * com_ptr_ref
         */
        namespace details
        {
            template <typename ComPtrType>
            class com_ptr_ref
            {
            public:
                /*
                 * Types
                 */
                using interface_type = typename ComPtrType::interface_type;



                /*
                 * Constructor(s)/Destructor
                 */
                com_ptr_ref(ComPtrType *ptr) :
                    _ptr(ptr)
                {
                }



                /*
                 * Operators
                 */
                operator interface_type **(void) const
                {
                    return this->_ptr->release_and_get_address_of();
                }

                operator void **(void) const
                {
                    return reinterpret_cast<void **>(this->_ptr->release_and_get_address_of());
                }

                template <typename = std::enable_if_t<std::is_base_of<IUnknown, interface_type>::value>>
                operator IUnknown **(void) const
                {
                    return reinterpret_cast<IUnknown **>(this->_ptr->release_and_get_address_of());
                }

                template <typename = std::enable_if_t<std::is_base_of<IInspectable, interface_type>::value>>
                operator IInspectable **(void) const
                {
                    return reinterpret_cast<IInspectable **>(this->_ptr->release_and_get_address_of());
                }

                // Used with IID_PPV_ARGS (for __uuidof). Reset is done in operator void **
                interface_type *operator*(void) const
                {
                    return this->_ptr->get();
                }

                operator ComPtrType *(void) const
                {
                    return this->_ptr;
                }



            private:

                ComPtrType *_ptr;
            };
        }



        /*
         * com_ptr class
         */
        template <typename IFace>
        class com_ptr final
        {
            static_assert(std::is_base_of<IUnknown, IFace>::value,
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

            com_ptr(void) = default;

            com_ptr(std::nullptr_t)
            {
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                com_ptr(Ty *ptr)
            {
                this->Assign(ptr);
            }

            com_ptr(const com_ptr &other)
            {
                this->Assign(other._ptr);
            }

            template <typename Ty>
            com_ptr(const com_ptr<Ty> &other)
            {
                this->Assign(other._ptr);
            }

            com_ptr(com_ptr &&other)
            {
                this->swap(other);
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<interface_type, Ty>::value>>
                com_ptr(com_ptr<Ty> &&other) :
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
            com_ptr &operator=(std::nullptr_t)
            {
                this->Release();
                return *this;
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                com_ptr &operator=(Ty *ptr)
            {
                // We cannot call swap() here, because the assignment might throw. In the case that it does, we would not
                // release as we are expected to do.
                this->Release();
                this->Assign(ptr);

                return *this;
            }

            com_ptr &operator=(const com_ptr &other)
            {
                if (this != &other)
                {
                    com_ptr(other).swap(*this);
                }

                return *this;
            }

            template <typename Ty>
            com_ptr &operator=(const com_ptr<Ty> &other)
            {
                // We cannot call swap() here, because the assignment might throw. In the case that it does, we would not
                // release as we are expected to do.
                this->Release();
                this->Assign(other._ptr);

                return *this;
            }

            com_ptr &operator=(com_ptr &&other)
            {
                assert(this != &other);
                this->swap(other);

                return *this;
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<interface_type, Ty>::value>>
                com_ptr &operator=(com_ptr<Ty> &&other)
            {
                // We do not call swap here as it would require an extra AddRef and Release since we would need to create
                // a separate object
                this->Release();
                this->_ptr = other._ptr;
                other._ptr = nullptr;

                return *this;
            }

            details::com_ptr_ref<com_ptr> operator&(void)
            {
                return details::com_ptr_ref<com_ptr>(this);
            }

            operator bool(void) const
            {
                return this->_ptr != nullptr;
            }

            bool operator==(const com_ptr &other)
            {
                return this->_ptr == other._ptr;
            }

            bool operator<(const com_ptr &other)
            {
                return this->_ptr < other._ptr;
            }

            bool operator>(const com_ptr &other)
            {
                return this->_ptr > other._ptr;
            }

            bool operator!=(const com_ptr &other) { return !(*this == other); }
            bool operator<=(const com_ptr &other) { return !(*this > other); }
            bool operator>=(const com_ptr &other) { return !(*this < other); }

            operator interface_type *(void) const
            {
                return this->_ptr;
            }

            interface_type *operator->(void) const
            {
                return this->_ptr;
            }

            interface_type &operator*(void)
            {
                return *this->_ptr;
            }



            /*
             * Explicit Functions
             */
            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                com_ptr<Ty> as(void)
            {
                return com_ptr<Ty>(*this);
            }

            void assign(std::nullptr_t)
            {
                this->Release();
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                void assign(Ty *ptr)
            {
                this->Release();
                this->Assign(ptr);
            }

            void attach(std::nullptr_t)
            {
                this->Release();
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                void attach(Ty *ptr)
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

            void swap(com_ptr &other)
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

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                void copy_to(Ty **ptr)
            {
                // Must first assign to null in case an exception is thrown
                *ptr = nullptr;
                *ptr = com_ptr<Ty>(*this).detach();
            }

            void copy_to(REFIID iid, void **ptr)
            {
                *ptr = nullptr;
                throw_if_failed(this->_ptr->QueryInterface(iid, ptr));
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
                typename = typename std::enable_if_t<std::is_base_of<interface_type, Ty>::value>>
                inline void Assign(Ty *ptr)
            {
                assert(!this->_ptr);
                this->_ptr = ptr;
                this->AddRef();
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<!std::is_base_of<interface_type, Ty>::value>,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                inline void Assign(Ty *ptr)
            {
                assert(!this->_ptr);
                if (ptr)
                {
                    throw_if_failed(ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr)));
                }
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<std::is_base_of<interface_type, Ty>::value>>
                inline void Attach(Ty *ptr)
            {
                // Easy case: we can just do simple assignment
                assert(!this->_ptr);
                this->_ptr = ptr;
            }

            template <
                typename Ty,
                typename = typename std::enable_if_t<!std::is_base_of<interface_type, Ty>::value>,
                typename = typename std::enable_if_t<std::is_base_of<IUnknown, Ty>::value>>
                inline void Attach(Ty *ptr)
            {
                // More complicated case: we must QI and Release, even if we fail since the caller is transferring
                // ownership over to us
                assert(!this->_ptr);

                if (ptr)
                {
                    HRESULT hr = ptr->QueryInterface(IID_PPV_ARGS(&this->_ptr));
                    ptr->Release();

                    throw_if_failed(hr);
                }
            }



            IFace *_ptr = nullptr;
        };
    }
}



#ifndef DHORN_NO_STD

namespace std
{
    template <typename Ty>
    inline void swap(dhorn::experimental::com_ptr<Ty> &lhs, dhorn::experimental::com_ptr<Ty> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif  /*DHORN_NO_STD*/



#ifndef DHORN_NO_GLOBAL

// Overload for IID_PPV_ARGS
template <typename Ty>
inline constexpr void **IID_PPV_ARGS_Helper(dhorn::experimental::details::com_ptr_ref<Ty> ptr)
{
    static_assert(std::is_base_of<IUnknown, typename Ty::interface_type>::value,
        "Cannot use IID_PPV_ARGS with a type that does not derive from IUnknown");
    return ptr;
}

#endif  /* DHORN_NO_GLOBAL */
