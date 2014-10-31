/*
 * Duncan Horn
 *
 * service_container.h
 *
 * 
 */
#pragma once

#ifdef  WIN32
#include <sal.h>
#endif  /* WIN32 */

#include <functional>
#include <map>
#include <memory>

#include "uuid.h"

namespace dhorn
{
    /*
     * Helpers
     */
    namespace garbage
    {
        class service_base
        {
        public:
            virtual ~service_base() {}
        };

        template <typename Ty>
        class service_derived :
            public service_base
        {
        public:
            service_derived(void) = default;

            service_derived(_In_ const std::shared_ptr<Ty> &ptr) :
                _ptr(ptr)
            {
            }

            service_derived(_Inout_ std::shared_ptr<Ty> &&ptr) :
                _ptr(std::move(ptr))
            {
            }

            service_derived(_Inout_ service_derived &&other) :
                _ptr(std::move(other._ptr))
            {
            }

            service_derived &operator=(_Inout_ service_derived &&other)
            {
                this->_ptr = std::move(other._ptr);
                return *this;
            }



            // Cannot copy
            service_derived(_In_ const service_derived &) = delete;
            service_derived &operator=(_In_ const service_derived &) = delete;



            service_derived &operator=(_In_ const std::shared_ptr<Ty> &other)
            {
                this->_ptr = other;
                return *this;
            }

            service_derived &operator=(_Inout_ std::shared_ptr<Ty> &&other)
            {
                this->_ptr = std::move(other);
                return *this;
            }

            const std::shared_ptr<Ty> &get(void) const
            {
                return this->_ptr;
            }



        private:

            std::shared_ptr<Ty> _ptr;
        };
    }



    /*
     * Exceptions
     */
#pragma region Exceptions

    class service_exception :
        public std::exception
    {
    public:
        service_exception(_In_ const std::string &exceptionType, _In_ const std::string &typeName) :
            _message(exceptionType + " : " + typeName)
        {
        }

        virtual const char *what(void) const
        {
            return this->_message.c_str();
        }

    protected:

        std::string _message;
    };



    class service_published :
        public service_exception
    {
    public:
        service_published(_In_ const std::string &serviceName) :
            service_exception("service_published", serviceName)
        {
        }
    };



    class service_not_published :
        public service_exception
    {
    public:
        service_not_published(_In_ const std::string &serviceName) :
            service_exception("service_not_published", serviceName)
        {
        }
    };

#pragma endregion



    /*
     * service_type_traits
     */
    template <typename Ty>
    struct service_type_traits {};



    /*
     * service_conatiner
     */
    class service_container final
    {
        using StorageType = std::unique_ptr<garbage::service_base>;
        using PairType = std::pair<uuid, StorageType>;
        using ContainerType = std::map<uuid, StorageType>;

    public:
        /*
         * Constructor(s)/Destructor
         */
        service_container(void)
        {
        }

        service_container(_Inout_ service_container &&other) :
            _map(std::move(other._map))
        {
        }



        // Cannot copy
        service_container(_In_ const service_container &) = delete;
        service_container &operator=(_In_ const service_container &) = delete;



        /*
         * Operators
         */
        service_container &operator=(_Inout_ service_container &&other)
        {
            this->_map = std::move(other._map);
            return *this;
        }



        /*
         * Insertion
         */
        template <typename Ty>
        std::shared_ptr<Ty> insert(_In_ Ty *ptr)
        {
            return this->Insert(std::shared_ptr<Ty>(ptr));
        }

        template <typename Ty, typename Deleter>
        std::shared_ptr<Ty> insert(_In_ Ty *ptr, _In_ const Deleter &del)
        {
            return this->Insert(std::shared_ptr<Ty>(ptr, del));
        }

        template <typename Ty, typename Deleter, typename Alloc>
        std::shared_ptr<Ty> insert(_In_ Ty *ptr, _In_ const Deleter &del, _In_ const Alloc &alloc)
        {
            return this->Insert(std::shared_ptr<Ty>(ptr, del, alloc));
        }

        template <typename Ty>
        std::shared_ptr<Ty> insert(_In_ const std::shared_ptr<Ty> &ptr)
        {
            return this->Insert(ptr);
        }

        template <typename Ty, typename... Args>
        std::shared_ptr<Ty> emplace(_Inout_ Args&&... args)
        {
            return this->Insert(std::make_shared<Ty>(std::forward<Args>(args)...));
        }

        template <typename Ty, typename Alloc, typename... Args>
        std::shared_ptr<Ty> allocate(_In_ const Alloc &alloc, _Inout_ Args&&... args)
        {
            return this->Insert(std::allocate_shared<Ty>(alloc, std::forward<Args>(args)...));
        }



        /*
         * Retrieval
         */
        template <typename Ty>
        const std::shared_ptr<Ty> &find(void) const
        {
            auto itr = this->Find<Ty>();
            auto derived = dynamic_cast<garbage::service_derived<Ty> *>(itr->second.get());

            if (!derived)
            {
                throw service_not_published(typeid(Ty).name());
            }

            return derived->get();
        }



        /*
         * Removal
         */
        template <typename Ty>
        std::shared_ptr<Ty> remove(void)
        {
            auto itr = this->Find<Ty>();
            auto derived = dynamic_cast<garbage::service_derived<Ty> *>(itr->second.get());

            if (!derived)
            {
                throw service_not_published(typeid(Ty).name());
            }

            auto result = derived->get();
            this->_map.erase(itr);

            return result;
        }



        /*
         * Other public functions
         */
        size_t size(void) const
        {
            return this->_map.size();
        }

        bool empty(void) const
        {
            return this->_map.empty();
        }

        void swap(_Inout_ service_container &other)
        {
            std::swap(this->_map, other._map);
        }



    private:

        template <typename Ty>
        const std::shared_ptr<Ty> &Insert(_In_ const std::shared_ptr<Ty> &ptr)
        {
            auto result = this->_map.insert(std::make_pair(
                service_type_traits<Ty>::id(),
                StorageType(new garbage::service_derived<Ty>(ptr))));

            if (!result.second)
            {
                throw service_published(typeid(Ty).name());
            }

            return ptr;
        }

        template <typename Ty>
        auto Find(void) const -> decltype(_map.find(std::declval<uuid>()))
        {
            auto itr = this->_map.find(service_type_traits<Ty>::id());

            if (itr == std::end(this->_map))
            {
                // Service does not exist
                throw service_not_published(typeid(Ty).name());
            }

            return itr;
        }

        ContainerType _map;
    };
}



#ifndef _DHORN_NO_STD

namespace std
{
    void swap(_Inout_ dhorn::service_container &lhs, _Inout_ dhorn::service_container &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif
