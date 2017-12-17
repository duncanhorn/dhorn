/*
 * Duncan Horn
 *
 * instance.h
 *
 * Implements a generic wrapper for instance objects with various customizable synchronization techniques. By default,
 * instance objects are managed by std::shared_ptr and synchronization is done using double-checked locking. The
 * template arguments available are:
 *
 *  Ty      - The underlying type
 *  Pointer - The type that is returned by get_instance. By default this is std::shared_ptr<Ty>. Note that this type
 *            must be copyable (i.e. cannot be std::unique_ptr)
 *  Synch   - The synchronization technique that will be used. By default, this uses double-checked locking through the
 *            lazy_initialization_t type
 *
 * Each synchronization technique provides a single non-static operator() function which returns a value of type
 * Pointer. There are a couple synchronization techniques provided. The first (the default) is lazy_initialization_t
 * which synchronizes using double-checked locking and allows for the following template arguments for further
 * customization:
 *
 *  Ty      - The underlying type
 *  Pointer - The type returned by the operator() function. Note that in order to accomplish double-checked locking,
 *            loading and storing the pointer must be atomic. Thus, this type can be of type std::shared_ptr<Ty>, or it
 *            can be of type Ty *. In the latter case, the underlying pointer is stored as std::atomic<Ty *>
 *  Alloc   - How to allocate/initialize and tear-down/deallocate the instance
 *
 * The second type is eager_initialization_t which creates the object instance on initialization. The template
 * arguments are the same as lazy_initialization_t without the restrictions on the type of Pointer.
 *
 * The final type is atomic_exchange_initialization_t which is a lock free implementation that is similar to double-
 * checked-locking in that it initially checks for null, but then allocates a new object and uses a compare-and-swap
 * operation to attempt and set the pointer. If the compare-and-swap fails (another thread set the pointer first), the
 * created object is destroyed and the value set by the other thread is returned. This is intended for use when
 * contention may be low and the overhead of a lock is not desired (and if it is okay to allocate more than one
 * instance of the object of course). The template arguments are again the same as lazy_initialization_t.
 */
#pragma once

#include <atomic>
#include <memory>
#include <mutex>

#include "../scope_guard.h"

namespace dhorn
{
    namespace experimental
    {
        namespace details
        {
            /*
             * Default allocation for the various initialization types
             */
            template <typename Ty, typename Alloc = std::allocator<Ty>>
            struct shared_ptr_lifetime_traits
            {
                shared_ptr_lifetime_traits(void) = default;
                shared_ptr_lifetime_traits(const Alloc &alloc) :
                    _alloc(alloc)
                {
                }

                std::shared_ptr<Ty> create(void)
                {
                    return std::allocate_shared<Ty>(this->_alloc);
                }

                void destroy(std::shared_ptr<Ty> &) const
                {
                    // shared_ptr handles its own lifetime
                }

            private:

                Alloc _alloc;
            };

            template <typename Ty, typename Alloc = std::allocator<Ty>>
            struct raw_ptr_lifetime_traits
            {
                raw_ptr_lifetime_traits(void) = default;
                raw_ptr_lifetime_traits(const Alloc &alloc) :
                    _alloc(alloc)
                {
                }

                Ty *create(void)
                {
                    auto ptr = this->_alloc.allocate(1);
                    try
                    {
                        std::allocator_traits<Alloc>::construct(this->_alloc, ptr);
                    }
                    catch (...)
                    {
                        this->_alloc.deallocate(ptr, 1);
                        throw;
                    }

                    return ptr;
                }

                void destroy(Ty *ptr)
                {
                    // Deallocate even if an exception is thrown in the destructor
                    auto deallocateOnExit = make_scope_guard([&]
                    {
                        this->_alloc.deallocate(ptr, 1);
                    });

                    std::allocator_traits<Alloc>::destroy(this->_alloc, ptr);
                }

            private:

                Alloc _alloc;
            };


            /*
             * Select the *lifetime_traits type from the pointer type
             */
            template <typename Pointer, typename Alloc>
            struct select_lifetime_traits;

            template <typename Ty, typename Alloc>
            struct select_lifetime_traits<std::shared_ptr<Ty>, Alloc>
            {
                using type = shared_ptr_lifetime_traits<Ty, Alloc>;
            };

            template <typename Ty, typename Alloc>
            struct select_lifetime_traits<Ty *, Alloc>
            {
                using type = raw_ptr_lifetime_traits<Ty, Alloc>;
            };



            /*
             * Select the underlying pointer type when atomic operations are desired
             */
            template <typename Pointer>
            struct select_pointer_for_atomic;

            template <typename Ty>
            struct select_pointer_for_atomic<std::shared_ptr<Ty>>
            {
                // Atomic operations can be done on std::shared_ptr itself
                using type = std::shared_ptr<Ty>;
            };

            template <typename Ty>
            struct select_pointer_for_atomic<Ty *>
            {
                // Atomic operations can't be done on raw pointers (for whatever reason...)
                using type = std::atomic<Ty *>;
            };
        }



        /*
         * Initialization Techniques
         */
#pragma region Initialization Techniques

        template <typename Ty, typename Pointer = std::shared_ptr<Ty>, typename Alloc = std::allocator<Ty>>
        struct lazy_initialization_t
        {
            lazy_initialization_t(void) :
                _instance(nullptr)
            {
            }

            lazy_initialization_t(const Alloc &alloc) :
                _instance(nullptr),
                _lifetimeTraits(alloc)
            {
            }

            ~lazy_initialization_t()
            {
                // Since we are in the destructor, we don't need to worry about synchronization since nobody should be
                // referencing this object
                auto ptr = std::atomic_load_explicit(&this->_instance, std::memory_order_relaxed);

                if (ptr)
                {
                    this->_lifetimeTraits.destroy(ptr);

                    Pointer null = nullptr;
                    std::atomic_store_explicit(&this->_instance, null, std::memory_order_relaxed);
                }
            }

            Pointer operator()(void)
            {
                auto ptr = std::atomic_load_explicit(&this->_instance, std::memory_order_relaxed);
                std::atomic_thread_fence(std::memory_order_acquire);

                if (!ptr)
                {
                    std::lock_guard<std::mutex> lock(this->_mutex);
                    ptr = std::atomic_load_explicit(&this->_instance, std::memory_order_relaxed);
                    if (!ptr)
                    {
                        ptr = this->_lifetimeTraits.create();
                        std::atomic_thread_fence(std::memory_order_release);
                        std::atomic_store_explicit(&this->_instance, ptr, std::memory_order_relaxed);
                    }
                }

                return ptr;
            }

        private:

            // We are using atomic operations, so we may need to change the storage type
            using pointer = typename details::select_pointer_for_atomic<Pointer>::type;
            using lifetime_traits = typename details::select_lifetime_traits<Pointer, Alloc>::type;

            lifetime_traits _lifetimeTraits;
            pointer _instance;
            std::mutex _mutex;
        };



        template <typename Ty, typename Pointer = std::shared_ptr<Ty>, typename Alloc = std::allocator<Ty>>
        struct eager_initialization_t
        {
            eager_initialization_t(void)
            {
                this->_instance = this->_lifetimeTraits.create();
            }

            eager_initialization_t(const Alloc &alloc) :
                _lifetimeTraits(alloc)
            {
                this->_instance = this->_lifetimeTraits.create();
            }

            ~eager_initialization_t()
            {
                if (this->_instance)
                {
                    this->_lifetimeTraits.destroy(this->_instance);
                    this->_instance = nullptr;
                }
            }

            Pointer operator()(void)
            {
                return this->_instance;
            }

        private:

            using pointer = Pointer;
            using lifetime_traits = typename details::select_lifetime_traits<Pointer, Alloc>::type;

            lifetime_traits _lifetimeTraits;
            pointer _instance;
        };



        template <typename Ty, typename Pointer = std::shared_ptr<Ty>, typename Alloc = std::allocator<Ty>>
        struct atomic_exchange_initialization_t
        {
            atomic_exchange_initialization_t(void) :
                _instance(nullptr)
            {
            }

            atomic_exchange_initialization_t(const Alloc &alloc) :
                _instance(nullptr),
                _lifetimeTraits(alloc)
            {
            }

            ~atomic_exchange_initialization_t()
            {
                // Since we are in the destructor, we don't need to worry about synchronization since nobody should be
                // referencing this object
                auto ptr = std::atomic_load_explicit(&this->_instance, std::memory_order_relaxed);

                if (ptr)
                {
                    this->_lifetimeTraits.destroy(ptr);
                    std::atomic_store_explicit(&this->_instance, pointer{ nullptr }, std::memory_order_relaxed);
                }
            }

            Pointer operator()(void)
            {
                auto ptr = std::atomic_load_explicit(&this->_instance, std::memory_order_relaxed);

                if (!ptr)
                {
                    // Not yet assigned, try to do so now
                    ptr = this->_lifetimeTraits.create();
                    Pointer existing = nullptr;
                    if (!std::atomic_compare_exchange_strong(&this->_instance, &existing, ptr))
                    {
                        // Someone else beat us to the punch. Destroy the one we just created and use theirs
                        this->_lifetimeTraits.destroy(ptr);
                        ptr = existing;
                    }
                }

                return ptr;
            }

        private:

            // We are using atomic operations, so we may need to change the storage type
            using pointer = typename details::select_pointer_for_atomic<Pointer>::type;
            using lifetime_traits = typename details::select_lifetime_traits<Pointer, Alloc>::type;

            lifetime_traits _lifetimeTraits;
            pointer _instance;
        };

#pragma endregion



        template <typename Ty, typename Pointer = std::shared_ptr<Ty>, typename Synch = lazy_initialization_t<Ty, Pointer>>
        class instance
        {
        public:

            /*
             * Public Types
             */
            using pointer = Pointer;



            /*
             * Constructor(s)/Destructor
             */
            instance()
            {
            }

            // Cannot copy or move
            instance(const instance &) = delete;
            instance &operator=(const instance &) = delete;



            /*
             * Accessors
             */
            pointer get(void)
            {
                return this->_synch();
            }

            Ty &operator*(void)
            {
                return *this->get();
            }

            pointer operator->(void)
            {
                return this->get();
            }

        private:

            Synch _synch;
        };



        /*
         * Useful typedefs
         */
#pragma region Typedefs

        template <typename Ty, typename Pointer = std::shared_ptr<Ty>>
        using lazy_init_instance_t = instance<Ty, Pointer, lazy_initialization_t<Ty, Pointer>>;

        template <typename Ty, typename Pointer = std::shared_ptr<Ty>>
        using eager_init_instance_t = instance<Ty, Pointer, eager_initialization_t<Ty, Pointer>>;

        template <typename Ty, typename Pointer = std::shared_ptr<Ty>>
        using atomic_exchange_instance_t = instance<Ty, Pointer, atomic_exchange_initialization_t<Ty, Pointer>>;

#pragma endregion
    }
}
