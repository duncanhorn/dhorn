/*
 * Duncan Horn
 *
 * auto_handle.h
 *
 * Defines the auto_handle and auto_shared_handle type along with various typedef's
 */
#pragma once

#include <atomic>
#include <cassert>
#include <memory>

#include "windows.h"

namespace dhorn
{
    namespace win32
    {
        /*
         * Handle Traits
         */
#pragma region Handle Traits

        template <typename Ty>
        struct handle_traits;

        template <>
        struct handle_traits<HANDLE>
        {
            static const/*expr*/ HANDLE invalid(void)
            {
                return INVALID_HANDLE_VALUE;
            }
        };

        template <>
        struct handle_traits<HBITMAP>
        {
            static const/*expr*/ HBITMAP invalid(void)
            {
                return nullptr;
            }
        };

#pragma endregion



        /*
         * auto_handle class
         */
#pragma region auto_handle

        template <
            typename HandleType,
            typename DestroyType,
            DestroyType DestroyFunc,
            typename TraitsType = handle_traits<HandleType>>
        class auto_handle final
        {
        public:
            /*
             * Types
             */
            using handle_type = HandleType;
            using traits_type = TraitsType;



            /*
             * Constructor(s)/Destructor
             */
            auto_handle(void) :
                _handle(traits_type::invalid())
            {
            }

            auto_handle(_In_ handle_type handle) :
                _handle(handle)
            {
            }

            auto_handle(_Inout_ auto_handle &&other) :
                auto_handle()
            {
                this->swap(other);
            }

            ~auto_handle(void)
            {
                this->Destroy();
            }



            /*
             * Cannot copy
             */
            auto_handle(_In_ const auto_handle &) = delete;
            auto_handle &operator=(_In_ const auto_handle &) = delete;



            /*
             * Operators
             */
            auto_handle &operator=(_Inout_ auto_handle &&other)
            {
                assert(this != &other);
                this->swap(other);

                return *this;
            }

            auto_handle &operator=(_In_ handle_type handle)
            {
                this->assign(handle);
                return *this;
            }

            operator handle_type(void) const
            {
                return this->_handle;
            }

            operator bool(void) const
            {
                return this->valid();
            }



            /*
             * Other Functions
             */
            void swap(_Inout_ auto_handle &other)
            {
                auto temp = this->_handle;
                this->_handle = other._handle;
                other._handle = temp;
            }

            handle_type detach(void)
            {
                auto result = this->_handle;
                this->_handle = traits_type::invalid();
                return result;
            }

            void release(void)
            {
                this->Destroy();
            }

            void assign(_In_ handle_type handle)
            {
                this->Destroy();
                this->_handle = handle;
            }

            handle_type get(void) const
            {
                return this->_handle;
            }

            bool valid(void) const
            {
                return (this->_handle != traits_type::invalid());
            }



        private:

            inline void Destroy(void)
            {
                if (this->valid())
                {
                    DestroyFunc(this->_handle);
                    this->_handle = traits_type::invalid();
                }
            }

            HandleType _handle;
        };

#pragma endregion



        /*
         * auto_shared_handle class
         */
#pragma region auto_shared_handle

        template <
            typename HandleType,
            typename DestroyType,
            DestroyType DestroyFunc,
            typename TraitsType = handle_traits<HandleType>>
        class auto_shared_handle final
        {
        public:
            /*
             * Types
             */
            using handle_type = HandleType;
            using traits_type = TraitsType;



        private:
            /*
             * Reference Counted Object
             */
            struct ref_count_container
            {
                ref_count_container(_In_ handle_type handle) :
                    _handle(handle),
                    _refCount(1)
                {
                    assert(handle != traits_type::invalid());
                }

                ~ref_count_container(void)
                {
                    assert(!this->_refCount.load());
                }



                handle_type get(void) const
                {
                    return this->_handle.get();
                }

                size_t AddRef(void)
                {
                    assert(this->_refCount.load());
                    return ++this->_refCount;
                }

                size_t Release(void)
                {
                    assert(this->_refCount.load());
                    auto result = --this->_refCount;

                    if (!result)
                    {
                        delete this;
                    }

                    return result;
                }

            private:

                auto_handle<HandleType, DestroyType, DestroyFunc, TraitsType> _handle;
                std::atomic_size_t _refCount;
            };

            struct ref_count
            {
                /*
                 * Constructor(s)/Destructor
                 */
                ref_count(void) :
                    _obj(nullptr)
                {
                }

                ref_count(_In_ handle_type handle) :
                    ref_count()
                {
                    if (handle != traits_type::invalid())
                    {
                        this->_obj = new ref_count_container(handle);
                    }
                }

                ref_count(_In_ const ref_count &other) :
                    _obj(other._obj)
                {
                    if (this->_obj)
                    {
                        this->_obj->AddRef();
                    }
                }

                ref_count(_Inout_ ref_count &&other) :
                    ref_count()
                {
                    this->swap(other);
                }

                ~ref_count(void)
                {
                    this->Destroy();
                }



                /*
                 * Assignment
                 */
                ref_count &operator=(_In_ const ref_count &other)
                {
                    if (this != &other)
                    {
                        this->Destroy();
                        if (other._obj)
                        {
                            this->_obj = other._obj;
                            this->_obj->AddRef();
                        }
                    }

                    return *this;
                }

                ref_count &operator=(_Inout_ ref_count &&other)
                {
                    assert(this != &other);
                    this->Destroy();
                    this->_obj = other._obj;
                    other._obj = nullptr;

                    return *this;
                }

                ref_count &operator=(_In_ handle_type handle)
                {
                    this->Destroy();
                    this->Assign(handle);

                    return *this;
                }



                handle_type get(void) const
                {
                    if (this->_obj)
                    {
                        return this->_obj->get();
                    }

                    return traits_type::invalid();
                }

                void release(void)
                {
                    this->Destroy();
                }

                void swap(_Inout_ ref_count &other)
                {
                    auto temp = this->_obj;
                    this->_obj = other._obj;
                    other._obj = temp;
                }

            private:

                void Destroy(void)
                {
                    if (this->_obj)
                    {
                        this->_obj->Release();
                        this->_obj = nullptr;
                    }
                }

                void Assign(_In_ handle_type handle)
                {
                    assert(!this->_obj);
                    if (handle != traits_type::invalid())
                    {
                        this->_obj = new ref_count_container(handle);
                    }
                }

                ref_count_container *_obj;
            };

        public:
            /*
             * Constructor(s)/Destructor
             */
            auto_shared_handle(void)
            {
            }

            auto_shared_handle(_In_ handle_type handle) :
                _handle(handle)
            {
            }

            auto_shared_handle(_In_ const auto_shared_handle &other) = default;

            auto_shared_handle(_Inout_ auto_shared_handle &&other) :
                _handle(std::move(other._handle))
            {
            }



            /*
             * Operators
             */
            auto_shared_handle &operator=(_In_ const auto_shared_handle &other) = default;

            auto_shared_handle &operator=(_Inout_ auto_shared_handle &&other)
            {
                this->swap(other);
                return *this;
            }

            auto_shared_handle &operator=(_In_ handle_type handle)
            {
                this->assign(handle);
                return *this;
            }

            operator handle_type(void) const
            {
                return this->_handle.get();
            }

            operator bool(void) const
            {
                return this->valid();
            }



            /*
             * Modifiers
             */
            void swap(_Inout_ auto_shared_handle &other)
            {
                this->_handle.swap(other._handle);
            }

            void release(void)
            {
                this->_handle.release();
            }

            void assign(_In_ handle_type handle)
            {
                this->_handle = handle;
            }

            bool valid(void) const
            {
                return (this->_handle.get() != traits_type::invalid());
            }



        private:

            ref_count _handle;
        };

#pragma endregion



        /*
         * Type Definitions
         */
#pragma region Type Definitions

        using unique_handle = auto_handle<handle_t, decltype(&close_handle), &close_handle>;
        using shared_handle = auto_shared_handle<handle_t, decltype(&close_handle), &close_handle>;

        using unique_bitmap = auto_handle<bitmap_handle, decltype(&delete_object), &delete_object>;
        using shared_bitmap = auto_shared_handle<bitmap_handle, decltype(&delete_object), &delete_object>;

#pragma endregion
    }
}



#ifndef _DHORN_NO_STD

namespace std
{
    template <
        typename HandleTy,
        typename DestroyTy,
        DestroyTy DestroyFunc,
        typename TraitsTy>
    inline void swap(
        _Inout_ dhorn::win32::auto_handle<HandleTy, DestroyTy, DestroyFunc, TraitsTy> &lhs,
        _Inout_ dhorn::win32::auto_handle<HandleTy, DestroyTy, DestroyFunc, TraitsTy> &rhs)
    {
        return lhs.swap(rhs);
    }

    template <
        typename HandleTy,
        typename DestroyTy,
        DestroyTy DestroyFunc,
        typename TraitsTy>
    inline void swap(
        _Inout_ dhorn::win32::auto_shared_handle<HandleTy, DestroyTy, DestroyFunc, TraitsTy> &lhs,
        _Inout_ dhorn::win32::auto_shared_handle<HandleTy, DestroyTy, DestroyFunc, TraitsTy> &rhs)
    {
        return lhs.swap(rhs);
    }
}

#endif  /*_DHORN_NO_STD*/
