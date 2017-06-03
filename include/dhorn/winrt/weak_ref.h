/*
 * Duncan Horn
 *
 * weak_ref.h
 *
 * A `std::weak_ptr`-like smart pointer for use with COM types that uses the `IWeakReference` WinRT type.
 */
#pragma once

#include <WeakReference.h>

#include "../com/com_ptr.h"
#include "../com/com_traits.h"
#include "winrt_traits.h"

namespace dhorn::winrt
{
    /*
     * Helpers
     */
    namespace details
    {
        /*
         * get_weak_reference
         */
        template <typename Ty>
        inline com::com_ptr<IWeakReference> get_weak_reference(Ty* ptr)
        {
            static_assert(!std::is_same_v<Ty, IWeakReference>, "Cannot get an IWeakReference from an IWeakReference");

            com::com_ptr<IWeakReferenceSource> source;
            com::check_hresult(ptr->QueryInterface(IID_PPV_ARGS(&source)));

            com::com_ptr<IWeakReference> result;
            com::check_hresult(source->GetWeakReference(&result));
            return result;
        }



        /*
         * weak_ref_storage
         */
#pragma region weak_ref_storage

        template <typename Ty, bool HasIid = com::has_iid<Ty>::value>
        class weak_ref_storage;

        template <typename Ty>
        class weak_ref_storage<Ty, true>
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            weak_ref_storage() = default;

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            weak_ref_storage(Type* ptr) :
                _ptr(get_weak_reference(ptr))
            {
            }

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            weak_ref_storage(const weak_ref_storage<Type>& other) noexcept :
                _ptr(other.reference())
            {
            }

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            weak_ref_storage(weak_ref_storage<Type>&& other) noexcept :
                _ptr(other.detach())
            {
            }



            /*
             * Modifiers
             */
            void reset() noexcept
            {
                this->_ptr.reset();
            }

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            void reset(const weak_ref_storage<Type>& other) noexcept
            {
                this->_ptr = other.reference();
            }

            template <typename Type, std::enable_if_t<std::is_base_of<Ty, Type>::value, int> = 0>
            void reset(weak_ref_storage<Type>&& other) noexcept
            {
                this->_ptr = other.detach();
            }

            void swap(weak_ref_storage& other) noexcept
            {
                this->_ptr.swap(other._ptr);
            }

            com::com_ptr<IWeakReference> detach() noexcept
            {
                com::com_ptr<IWeakReference> result;
                this->_ptr.swap(result);
                return result;
            }



            /*
             * Observers
             */
            com::com_ptr<Ty> resolve() const noexcept
            {
                com::com_ptr<Ty> result;
                if (this->_ptr && SUCCEEDED(this->_ptr->Resolve(&result)))
                {
                    return result;
                }

                return nullptr;
            }

            com::com_ptr<IWeakReference> reference() const noexcept
            {
                return this->_ptr;
            }



        private:

            com::com_ptr<IWeakReference> _ptr;
        };

        template <typename Ty>
        class weak_ref_storage<Ty, false>
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            weak_ref_storage() = default;

            weak_ref_storage(Ty* ptr) :
                _weakRef(get_weak_reference(ptr)),
                _ptr(ptr)
            {
            }

            // NOTE: Default copy/move are okay since we don't ever use _ptr unless _weakRef resolves



            /*
             * Modifiers
             */
            void reset() noexcept
            {
                this->_weakRef.reset();
                this->_ptr = nullptr;
            }

            void swap(weak_ref_storage& other) noexcept
            {
                this->_weakRef.swap(other._weakRef);
                std::swap(this->_ptr, other._ptr);
            }

            com::com_ptr<IWeakReference> detach() noexcept
            {
                com::com_ptr<IWeakReference> result;
                this->_weakRef.swap(result);
                this->_ptr = nullptr;

                return result;
            }



            /*
             * Observers
             */
            com::com_ptr<Ty> resolve() const noexcept
            {
                com::com_ptr<IInspectable> insp;
                if (this->_weakRef && SUCCEEDED(this->_weakRef->Resolve(&insp)) && insp)
                {
                    com::com_ptr<Ty> result;
                    result.attach(this->_ptr);
                    insp.detach();

                    return result;
                }

                return nullptr;
            }

            com::com_ptr<IWeakReference> reference() const noexcept
            {
                return this->_weakRef;
            }



        private:

            com::com_ptr<IWeakReference> _weakRef;
            Ty* _ptr = nullptr;
        };

#pragma endregion
    }



    /*
     * weak_ref
     */
    template <typename Ty>
    class weak_ref
    {
        static_assert(is_inspectable_v<Ty>, "weak_ref can only be used with WinRT types");

        template <typename>
        friend class weak_ref;

    public:
        /*
         * Constructor(s)/Destructor
         */
        weak_ref() = default;

        explicit weak_ref(Ty* ptr) :
            _data(ptr)
        {
        }

        explicit weak_ref(const com::com_ptr<Ty>& ptr) :
            _data(ptr.get())
        {
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        explicit weak_ref(OtherTy* ptr) :
            _data(ptr)
        {
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        explicit weak_ref(const com::com_ptr<OtherTy>& ptr) :
            _data(ptr.get())
        {
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        explicit weak_ref(const weak_ref<OtherTy>& ref) noexcept :
            _data(ref._data)
        {
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        explicit weak_ref(weak_ref<OtherTy>&& ref) noexcept :
            _data(std::move(ref._data))
        {
        }



        /*
         * Assignment
         */
        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        weak_ref& operator=(const weak_ref<OtherTy>& ref) noexcept
        {
            this->_data.reset(ref._data);
            return *this;
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        weak_ref& operator=(weak_ref<OtherTy>&& ref) noexcept
        {
            this->_data.reset(std::move(ref._data));
            return *this;
        }



        /*
         * Modifiers
         */
        void reset() noexcept
        {
            this->_data.reset();
        }

        void swap(weak_ref& other) noexcept
        {
            this->_data.swap(other._data);
        }



        /*
         * Observers
         */
        bool expired() const noexcept
        {
            return lock() == nullptr;
        }

        com::com_ptr<Ty> lock() const noexcept
        {
            return this->_data.resolve();
        }



    private:

        details::weak_ref_storage<Ty> _data;
    };



    /*
     * as_weak
     */
#pragma region as_weak

    template <typename Ty>
    inline weak_ref<Ty> as_weak(Ty* ptr)
    {
        return weak_ref<Ty>(ptr);
    }

    template <typename Ty>
    inline weak_ref<Ty> as_weak(const com::com_ptr<Ty>& ptr)
    {
        return weak_ref<Ty>(ptr);
    }

#pragma endregion
}
