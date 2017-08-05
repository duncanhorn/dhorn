/*
 * Duncan Horn
 *
 * agile_ref.h
 *
 * 
 */
#pragma once

#include "../com/com_ptr.h"

namespace dhorn::winrt
{
    /*
     * Helpers
     */
    namespace details
    {
        template <typename Ty>
        com::com_ptr<IAgileReference> get_agile_reference(Ty* ptr)
        {
            static_assert(!std::is_same_v<Ty, IAgileReference>, "Cannot get an IAgileReference from an IAgileReference");

            com::com_ptr<IAgileReference> result;
            if (ptr)
            {
                com::check_hresult(::RoGetAgileReference(
                    AGILEREFERENCE_DEFAULT,
                    com::interface_traits<Ty>::interface_id(),
                    ptr,
                    &result));
            }

            return result;
        }
    }



    /*
     * agile_ref
     */
    template <typename IFace>
    class agile_ref
    {
        template <typename>
        friend class agile_ref;

    public:
        /*
         * Constructor(s)/Destructor
         */
        agile_ref() = default;

        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref(Ty* ptr) :
            _agileRef(details::get_agile_reference(ptr))
        {
        }

        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref(const com::com_ptr<Ty>& ptr) :
            _agileRef(details::get_agile_reference(ptr.get()))
        {
        }

        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref(const agile_ref<Ty>& other) noexcept :
            _agileRef(other._agileRef)
        {
        }

        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref(agile_ref<Ty>&& other) noexcept :
            _agileRef(std::move(other._agileRef))
        {
        }



        /*
         * Assignment
         */
        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref& operator=(const agile_ref<Ty>& other) noexcept
        {
            this->_agileRef = other._agileRef;
            return *this;
        }

        template <typename Ty, std::enable_if_t<std::is_base_of<IFace, Ty>::value, int> = 0>
        agile_ref& operator=(agile_ref<Ty>&& other) noexcept
        {
            this->_agileRef.swap(other._agileRef);
            return *this;
        }



        /*
         * Modifiers
         */
        void reset() noexcept
        {
            this->_agileRef.reset();
        }

        void swap(agile_ref& other) noexcept
        {
            this->_agileRef.swap(other._agileRef);
        }



        /*
         * Observers
         */
        operator bool() const noexcept
        {
            return static_cast<bool>(this->_agileRef);
        }

        com::com_ptr<IFace> resolve() const
        {
            com::com_ptr<IFace> result;
            com::check_hresult(this->_agileRef->Resolve(com::interface_traits<IFace>::interface_id(), &result));
            return result;
        }

        template <typename Ty>
        com::com_ptr<Ty> resolve_as() const
        {
            com::com_ptr<Ty> result;
            com::check_hresult(this->_agileRef->Resolve(com::interface_traits<Ty>::interface_id(), &result));
            return result;
        }



    private:

        com::com_ptr<IAgileReference> _agileRef;
    };
}
