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
                _ptr(com::query<IWeakReference>(ptr))
            {
            }



            /*
             * Modifiers
             */
            void reset() noexcept
            {
                this->_ptr.reset();
            }

            void swap(weak_ref_storage& other) noexcept
            {
                this->_ptr.swap(other._ptr);
            }



            /*
             * Observers
             */
            com::com_ptr<Ty> resolve() const noexcept
            {
                return this->_ptr.try_as<Ty>();
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
                _weakRef(com::query<IWeakReference>(ptr)),
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



            /*
             * Observers
             */
            com::com_ptr<Ty> resolve() const noexcept
            {
                if (auto ptr = this->_weakRef.try_as<IInspectable>())
                {
                    com::com_ptr<Ty> result;
                    result.attach(this->_ptr);
                    ptr.detach();

                    return result;
                }
            }



        private:

            com::com_ptr<IWeakReference> _weakRef;
            Ty* _ptr = nullptr;
        };
    }



    /*
     * weak_ref
     */
    template <typename Ty>
    class weak_ref
    {
        static_assert(is_inspectable_v<Ty>, "weak_ref can only be used with WinRT types");

    public:
        /*
         * Constructor(s)/Destructor
         */
        weak_ref() = default;

        weak_ref(Ty* ptr) :
            _data(ptr)
        {
        }

        template <
            typename OtherTy,
            typename Type = Ty,
            std::enable_if_t<com::has_iid<Type>::value, int> = 0,
            std::enable_if_t<std::is_base_of<Ty, OtherTy>::value, int> = 0>
        weak_ref(OtherTy* ptr) :
            _data(ptr)
        {
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
}
