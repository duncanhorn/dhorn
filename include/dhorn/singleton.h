/*
 * Duncan Horn
 *
 * singleton.h
 *
 * Implements a generic wrapper for singleton objects with various customizable synchronization techniques. By default,
 * singleton objects are managed by std::shared_ptr and synchronization is done using double-checked locking. The
 * template arguments available are:
 *
 *  Ty      - The underlying type
 *  Pointer - The type that is returned by get_instance. By default this is std::shared_ptr<Ty>. Note that this type
 *            must be copyable (i.e. cannot be std::unique_ptr)
 *  Synch   - The synchronization technique that will be used. By default, this uses double-checked locking through the
 *            double_checked_locking_t type
 *
 * Each synchronization technique provides a single non-static operator() function which returns a value of type
 * Pointer. There are a couple synchronization techniques provided. The first (the default) is lazy_initialization_t
 * which synchronizes using double-checked locking and allows for the following template arguments for further
 * customization:
 *
 *  Pointer - The type returned by the operator() function. Note that in order to accomplish double-checked locking,
 *            loading and storing the pointer must be atomic. Thus, this type can be of type std::shared_ptr<Ty>, or it
 *            can be of type Ty *. In the latter case, the underlying pointer is stored as std::atomic<Ty *>
 *  Alloc   - Function object type for creating and destroying an instance of the object. This type should provide a
 *            static create and destroy function and should return an object of type Pointer. Note that the default
 *            behavior when Pointer is Ty * (i.e. not shared_ptr) is to always delete the pointer on detroy
 *
 * The second type is eager_initialization_t which creates the object instance on initialization. The template
 * arguments are the same as lazy_initialization_t without the restrictions on the type of Pointer.
 *
 * The final type is atomic_exchange_initialization_t which is a lock free implementation that is similar to double-
 * checked-locking in that it initially checks for null, but then allocates a new object and uses a compare-and-swap
 * operation to attempt and set the pointer. If the compare-and-swap fails (another thread set the pointer first), the
 * created object is destroyed and the value set by the other thread is returned. This is intended for use when
 * contention may be low and the overhead of a lock is not desired (and of it is okay to allocate more than one
 * instance of the object of course). The template arguments are again the same as lazy_initialization_t.
 */
#pragma once

#include <atomic>
#include <memory>
#include <mutex>

namespace dhorn
{
    namespace garbage
    {
        /*
         * Default Alloc for the various initialization types
         */
        template <typename Ty, typename Alloc = std::allocator<Ty>>
        struct shared_ptr_create_instance
        {
            shared_ptr_create_instance(void) = default;
            shared_ptr_create_instance(_In_ Alloc alloc) :
                _alloc(std::move(alloc))
            {
            }

            std::shared_ptr<Ty> create(void)
            {
                return std::allocate_shared<Ty>(this->_alloc);
            }

            void destroy(_In_ std::shared_ptr<Ty> &) const
            {
                // shared_ptr handles its own lifetime
            }

        private:

            Alloc _alloc;
        };

        template <typename Ty, typename Alloc = std::allocator<Ty>>
        struct raw_ptr_create_instance
        {
            raw_ptr_create_instance(void) = default;
            raw_ptr_create_instance(_In_ Alloc alloc) :
                _alloc(std::move(alloc))
            {
            }

            Ty *create(void)
            {

            }

            void destroy(_In_ Ty *ptr) const
            {
                this->_alloc.destroy(ptr);
            }

        private:

            Alloc _alloc;
        };
    }



    /*
     * double_checked_locking_t
     */
    template <typename Pointer, typename Alloc /*TODO: default*/>
    struct lazy_initialization_t
    {

    };



    template <typename Pointer, typename Alloc /*TODO: default*/>
    struct eager_initialization_t
    {

    };



    template <typename Pointer, typename Alloc /*TODO: default*/>
    struct atomic_exchange_initialization_t
    {

    };



    namespace garbage
    {
        /*
         * Creators
         */
        template <typename Ty>
        struct singleton_raw_pointer_lifetime_traits
        {
            using pointer = Ty *;

            static pointer create(void)
            {
                return new Ty();
            }

            static void destroy(pointer ptr)
            {
                delete ptr;
            }
        };

        template <typename Ty>
        struct singleton_shared_ptr_lifetime_traits
        {
            using pointer = std::shared_ptr<Ty>;

            static pointer create(void)
            {
                return std::make_shared<Ty>();
            }

            static void destroy(const pointer &ptr)
            {
                // shared_ptr is already RAII
            }
        };



        /*
         * select_creator
         */
        template <typename Ty>
        struct select_singleton_lifetime_traits;

        template <typename Ty>
        struct select_singleton_lifetime_traits<Ty *>
        {
            using type = singleton_raw_pointer_lifetime_traits<Ty>;
        };

        template <typename Ty>
        struct select_singleton_lifetime_traits<std::shared_ptr<Ty>>
        {

        };



        /*
         * select_singleton_pointer
         */
        template <typename Ty>
        struct select_singleton_pointer;

        template <typename Ty>
        struct select_singleton_pointer<Ty *>
        {
            using pointer = std::atomic<Ty *>;
        };

        template <typename Ty>
        struct select_singleton_pointer<std::shared_ptr<Ty>>
        {
            using pointer = std::shared_ptr<Ty>;
        };
    }



    /*
     * singleton
     */
    template <
        typename Ty,
        typename Pointer = std::shared_ptr<Ty>,
        typename LifetimeTraits = typename garbage::select_singleton_lifetime_traits<Pointer>::type>
    class singleton
    {
        using internal_pointer = typename garbage::select_singleton_pointer<Pointer>::pointer;

    public:
        /*
         * Public Types
         */
        using lifetime_traits = LifetimeTraits;
        using pointer = Pointer;



        /*
         * Constructor(s)/Destructor
         */
        singleton() :
            _ptr(nullptr)
        {
        }

        singleton(_In_ lifetime_traits traits) :
            _ptr(nullptr),
            _traits(std::move(traits))
        {
        }

        // Cannot copy or move
        singleton(_In_ const singleton &) = delete;
        singleton &operator=(_In_ const singleton &) = delete;



        Pointer get_instance()
        {
            auto ptr = std::atomic_load_explicit(&this->_ptr, std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);

            if (ptr == nullptr)
            {
                std::lock_guard<Mutex> lock(this->_mutex);
                ptr = std::atomic_load_explicit(&this->_ptr, std::memory_order_relaxed);
                if (ptr == nullptr)
                {
                    ptr = lifetime_traits::create();
                    std::atomic_thread_fence(std::memory_order_release);
                    std::atomic_store_explicit(&this->_ptr, ptr, std::memory_order_relaxed);
                }
            }

            return this->_ptr;
        }



        /*
         * Operators
         */
        Ty &operator*(void)
        {
            return *this->get_instance();
        }

        pointer operator->(void)
        {
            return this->get_instance();
        }



    private:

        internal_pointer _ptr;
        std::mutex _mutex;

        // 
        lifetime_traits _traits;
    };
}
