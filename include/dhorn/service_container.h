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
            template <typename Deleter>
            service_derived(Deleter del = Deleter()) :
                _ptr(nullptr),
                _deleter(del)
            {
            }

            template <typename Deleter>
            service_derived(_In_ Ty *ptr, _In_ const Deleter &del = Deleter()) :
                _ptr(ptr),
                _deleter(del)
            {
            }

            template <typename Deleter>
            service_derived(_In_ Ty *ptr, Deleter &&del) :
                _ptr(ptr),
                _deleter(std::move(del))
            {
            }

            service_derived(_Inout_ service_derived &&other) :
                _ptr(other._ptr),
                _deleter(std::move(other._deleter))
            {
                other._ptr = nullptr;
            }

            virtual ~service_derived(void)
            {
                this->Destroy();
            }



            // Cannot copy
            service_derived(_In_ const service_derived &) = delete;
            service_derived &operator=(_In_ const service_derived &) = delete;



            service_derived &operator=(_Inout_ service_derived &&other)
            {
                this->Destroy();

                this->_ptr = other._ptr;
                this->_deleter = other._deleter;
                return *this;
            }



            Ty *get(void) const
            {
                return this->_ptr;
            }

            Ty *detach(void)
            {
                Ty *ptr = this->_ptr;
                this->_ptr = nullptr;
                return ptr;
            }

            void set(_In_ Ty *ptr)
            {
                this->Destroy();
                this->_ptr = ptr;
            }



        private:

            void Destroy(void)
            {
                this->_deleter(this->_ptr);
                this->_ptr = nullptr;
            }

            Ty *_ptr;
            std::function<void(Ty *)> _deleter;
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
        service_exception(_In_ const char *exceptionType, _In_ const char *typeName) :
            _message(exceptionType)
        {
            this->_message += " : ";
            this->_message += typeName;
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
        service_published(_In_ const char *serviceName) :
            service_exception("service_published", serviceName)
        {
        }

        service_published(_In_ const std::string &serviceName) :
            service_exception("service_published", serviceName.c_str())
        {
        }
    };

    class service_not_published :
        public service_exception
    {
    public:
        service_not_published(_In_ const char *serviceName) :
            service_exception("service_not_published", serviceName)
        {
        }

        service_not_published(_In_ const std::string &serviceName) :
            service_exception("service_not_published", serviceName.c_str())
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
        template <typename Ty, typename Deleter = std::default_delete<Ty>>
        void insert(_In_ Ty *ptr, _In_ const Deleter &deleter = Deleter())
        {
            StorageType storage(new garbage::service_derived<Ty>(ptr, deleter));
            this->Insert<Ty>(std::move(storage));
        }

        template <typename Ty, typename Deleter = std::default_delete<Ty>>
        void insert(_In_ Ty *ptr, _Inout_ Deleter &&deleter)
        {
            StorageType storage(new garbage::service_derived<Ty>(ptr, std::move(deleter)));
            this->Insert<Ty>(std::move(storage));
        }

        template <typename Ty, typename Deleter = std::default_delete<Ty>, typename... Args>
        void emplace(Args&&... args)
        {
            this->insert(new Ty(std::forward<Args>(args)...), Deleter());
        }



        /*
         * Retrieval
         */
        template <typename Ty>
        Ty *find(void) const
        {
            auto itr = this->Find<Ty>();

            auto derived = dynamic_cast<garbage::service_derived<Ty> *>(itr->second.get());
            if (!derived)
            {
                throw std::bad_cast();
            }

            return derived->get();
        }



        /*
         * Removal
         */
        template <typename Ty>
        std::unique_ptr<Ty> remove(void)
        {
            auto itr = this->Find<Ty>();

            auto derived = dynamic_cast<garbage::service_derived<Ty> *>(itr->second.get());
            if (!derived)
            {
                throw std::bad_cast();
            }

            std::unique_ptr<Ty> ptr(derived->detach());
            this->_map.erase(itr);

            return std::move(ptr);
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



    private:

        template <typename Ty>
        void Insert(_Inout_ StorageType &&storage)
        {
            auto pair = std::make_pair(service_type_traits<Ty>::id(), std::move(storage));
            auto result = this->_map.insert(std::move(pair));

            if (!result.second)
            {
                throw service_published(typeid(Ty).name());
            }
        }

        template <typename Ty>
        ContainerType::const_iterator Find(void) const
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
