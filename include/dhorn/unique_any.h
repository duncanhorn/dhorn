/*
 * Duncan Horn
 *
 * unique_any.h
 *
 * Defines the unique_any type which behaves pretty much exactly like std::unique_ptr does, only the first template
 * argument is stored as-is, not as a pointer. I.e. std::unique_ptr<Foo> would be equivalent to
 * dhorn::unique_any<Foo *>. The key difference is that dhorn::unique_any need not hold a pointer. E.g. it could hold a
 * HANDLE: dhorn::unique_any<HANDLE, garbage::close_handle>. The only requirements are that the type be default
 * constructable, and it must be comparable to itself.
 */
#pragma once

#include <memory>
#include <type_traits>

#ifdef WIN32
#include "windows/windows.h"
#endif

#include "type_traits.h"

namespace dhorn
{
    /*
     * Helpers
     */
    namespace garbage
    {
        template <typename Ty>
        struct no_op
        {
            void operator()(_In_ const Ty &)
            {
            }
        };

        template <typename Ty>
        struct unique_storage
        {
            using type = Ty;
        };

        template <typename Ty>
        struct unique_storage<Ty[]>
        {
            using type = Ty *;
        };

        template <typename Ty>
        struct unique_any_traits
        {
            static constexpr Ty invalid(void)
            {
                return Ty{};
            }
        };
    }



    /*
     * unique_any
     */
#pragma region unique_any

    template <
        typename Ty,
        typename DestroyType = typename std::conditional<
            std::is_pointer<typename garbage::unique_storage<Ty>::type>::value,
            std::default_delete<typename std::remove_pointer<Ty>::type>, // Use operator delete if it's a pointer
            garbage::no_op<Ty>>::type, // Just nop if it's a non-pointer type (i.e. has destructor)
        typename Traits = garbage::unique_any_traits<typename garbage::unique_storage<Ty>::type>>
    class unique_any
    {
        using storage_type = typename garbage::unique_storage<Ty>::type;

    public:
        unique_any(void) :
            _value(Traits::invalid())
        {
        }

        unique_any(_In_ storage_type val) :
            _value(std::move(val))
        {
        }

        unique_any(_Inout_ unique_any &&other) :
            unique_any() // Construct with invalid value
        {
            this->swap(other);
        }

        ~unique_any(void)
        {
            this->Destroy();
        }

        // Cannot copy (that's the whole point of unique_*)
        unique_any(_In_ const unique_any &other) = delete;
        unique_any &operator=(_In_ const unique_any &other) = delete;



        /*
         * Operators
         */
        unique_any &operator=(_Inout_ unique_any &&other)
        {
            this->swap(other);
            return *this;
        }

        unique_any &operator=(_In_ storage_type val)
        {
            this->Destroy();
            this->_value = std::move(val);
            return *this;
        }

        operator const storage_type &(void) const
        {
            return this->_value;
        }

        operator bool(void) const
        {
            return this->_value != Traits::invalid();
        }



        /*
         * Public Functions
         */
        const storage_type &get(void) const
        {
            return this->_value;
        }

        storage_type release(void)
        {
            storage_type val = std::move(this->_value);
            this->_value = Traits::invalid();

            return val;
        }

        void reset(void)
        {
            this->Destroy();
        }

        void reset(_In_ storage_type val)
        {
            *this = std::move(val);
        }

        void swap(_Inout_ unique_any &other)
        {
            std::swap(this->_value, other._value);
        }



    private:

        void Destroy(void)
        {
            if (this->_value != Traits::invalid())
            {
                DestroyType()(this->_value);
                this->_value = Traits::invalid();
            }
        }

        storage_type _value;
    };

#pragma endregion



    /*
     * Destroy Types
     */
    namespace garbage
    {
        /*
         * For Windows types
         */
#ifdef WIN32

        struct close_handle
        {
            void operator()(_In_ win32::handle_t handle)
            {
                win32::close_handle(handle);
            }
        };

        struct delete_dc
        {
            void operator()(_In_ win32::device_context_handle handle)
            {
                win32::delete_dc(handle);
            }
        };

        struct delete_object
        {
            void operator()(_In_ win32::gdi_object_handle handle)
            {
                win32::delete_object(handle);
            }
        };

        struct destroy_cursor
        {
            void operator()(_In_ win32::cursor_handle handle)
            {
                win32::destroy_cursor(handle);
            }
        };

        struct destroy_icon
        {
            void operator()(_In_ win32::icon_handle handle)
            {
                win32::destroy_icon(handle);
            }
        };

        struct destroy_menu
        {
            void operator()(_In_ win32::menu_handle handle)
            {
                win32::destroy_menu(handle);
            }
        };

        struct destroy_window
        {
            void operator()(_In_ win32::window_handle handle)
            {
                win32::destroy_window(handle);
            }
        };

        struct free_library
        {
            void operator()(_In_ win32::module_handle handle)
            {
                // NOTE: Also callable with type `instance_handle`
                win32::free_library(handle);
            }
        };

        struct release_dc
        {
            void operator()(_In_ win32::device_context_handle handle)
            {
                win32::release_dc(handle);
            }
        };

#endif
    }



    /*
     * Type Definitions
     */
#pragma region Type Definitions



    /*
     * Windows Types
     */
#pragma region Windows Types
#ifdef WIN32

    namespace garbage
    {
        template <typename HandleType>
        struct invalid_handle_traits
        {
        private:

            struct constexpr_handle_helper
            {
                HandleType handle;

                constexpr_handle_helper() :
                    handle(INVALID_HANDLE_VALUE)
                {
                }
            };

        public:

            static constexpr HandleType invalid(void)
            {
                return constexpr_handle_helper().handle;
            }
        };

        template <typename HandleType>
        struct null_handle_traits
        {
            static constexpr HandleType invalid(void)
            {
                return nullptr;
            }
        };
    }

    using unique_handle = unique_any<win32::handle_t, garbage::close_handle, garbage::invalid_handle_traits<win32::handle_t>>;
    using unique_bitmap = unique_any<win32::bitmap_handle, garbage::delete_object, garbage::null_handle_traits<win32::bitmap_handle>>;
    using unique_brush = unique_any<win32::brush_handle, garbage::delete_object, garbage::null_handle_traits<win32::brush_handle>>;
    using unique_cursor = unique_any<win32::cursor_handle, garbage::destroy_cursor, garbage::null_handle_traits<win32::cursor_handle>>;
    using unique_deletable_dc = unique_any<win32::device_context_handle, garbage::delete_dc, garbage::null_handle_traits<win32::device_context_handle>>;
    using unique_releasable_dc = unique_any<win32::device_context_handle, garbage::release_dc, garbage::null_handle_traits<win32::device_context_handle>>;
    using unique_gdi_object = unique_any<win32::gdi_object_handle, garbage::delete_object, garbage::null_handle_traits<win32::gdi_object_handle>>;
    using unique_icon = unique_any<win32::icon_handle, garbage::destroy_icon, garbage::null_handle_traits<win32::icon_handle>>;
    using unique_instance = unique_any<win32::instance_handle, garbage::free_library, garbage::null_handle_traits<win32::instance_handle>>;
    using unique_menu = unique_any<win32::menu_handle, garbage::destroy_menu, garbage::null_handle_traits<win32::menu_handle>>;
    using unique_module = unique_any<win32::module_handle, garbage::free_library, garbage::null_handle_traits<win32::module_handle>>;
    using unique_window = unique_any<win32::window_handle, garbage::destroy_window, garbage::null_handle_traits<win32::window_handle>>;

#endif
#pragma endregion

#pragma endregion
}



#ifndef _DHORN_NO_STD

namespace std
{
    template <typename Ty, typename DestroyType, typename Traits>
    void swap(
        _Inout_ dhorn::unique_any<Ty, DestroyType, Traits> &lhs,
        _Inout_ dhorn::unique_any<Ty, DestroyType, Traits> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif
