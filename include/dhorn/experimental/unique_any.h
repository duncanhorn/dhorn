/*
 * Duncan Horn
 *
 * unique_any.h
 *
 * Defines the unique_any type which behaves pretty much exactly like std::unique_ptr does, only the first template
 * argument is stored as-is, not as a pointer. I.e. std::unique_ptr<Foo> would be equivalent to
 * dhorn::unique_any<Foo *>. The key difference is that dhorn::unique_any need not hold a pointer. E.g. it could hold a
 * HANDLE: dhorn::unique_any<HANDLE, details::close_handle>. The only requirements are that the type be default
 * constructable, and it must be comparable to itself.
 */
#pragma once

#include <memory>
#include <type_traits>

#if defined(WIN32) || defined(_WIN64)
#include "windows/windows.h"
#endif

#include "../type_traits.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * Helpers
         */
        namespace details
        {
            template <typename Ty>
            struct no_op
            {
                void operator()(const Ty &) noexcept
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
            using unique_storage_t = typename unique_storage<Ty>::type;

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
            typename DestroyType = std::conditional_t<std::is_pointer_v<details::unique_storage_t<Ty>>,
                std::default_delete<std::remove_pointer_t<Ty>>, // Use operator delete if it's a pointer
                details::no_op<Ty>>, // Just nop if it's a non-pointer type (i.e. has destructor)
            typename Traits = details::unique_any_traits<details::unique_storage_t<Ty>>>
        class unique_any
        {
            using storage_type = details::unique_storage_t<Ty>;

        public:
            unique_any(void) :
                _value(Traits::invalid())
            {
            }

            unique_any(storage_type val) :
                _value(std::move(val))
            {
            }

            unique_any(unique_any &&other) :
                unique_any() // Construct with invalid value
            {
                this->swap(other);
            }

            ~unique_any(void) noexcept(noexcept(std::declval<DestroyType>()(this->_value)))
            {
                this->Destroy();
            }

            // Cannot copy (that's the whole point of unique_*)
            unique_any(const unique_any &other) = delete;
            unique_any &operator=(const unique_any &other) = delete;



            /*
             * Operators
             */
            unique_any &operator=(unique_any &&other)
            {
                this->swap(other);
                return *this;
            }

            unique_any &operator=(const storage_type &val)
            {
                this->Destroy();
                this->_value = val;
                return *this;
            }

            unique_any &operator=(storage_type &&val)
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
                auto val = std::move(this->_value);
                this->_value = Traits::invalid();

                return val;
            }

            void reset(void)
            {
                this->Destroy();
            }

            void reset(const storage_type &val)
            {
                *this = val;
            }

            void reset(storage_type &&val)
            {
                *this = std::move(val);
            }

            void swap(unique_any &other)
            {
                using std::swap;
                swap(this->_value, other._value);
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

        template <typename Ty, typename DestroyType, typename Traits>
        void swap(unique_any<Ty, DestroyType, Traits> &lhs, unique_any<Ty, DestroyType, Traits> &rhs)
        {
            lhs.swap(rhs);
        }

#pragma endregion



        /*
         * Destroy Types
         */
        namespace details
        {
            /*
             * For Windows types
             */
#if defined(WIN32) || defined(_WIN64)

            struct close_handle
            {
                void operator()(win32::handle_t handle)
                {
                    win32::close_handle(handle);
                }
            };

            struct delete_dc
            {
                void operator()(win32::device_context_handle handle)
                {
                    win32::delete_dc(handle);
                }
            };

            struct delete_object
            {
                void operator()(win32::gdi_object_handle handle)
                {
                    win32::delete_object(handle);
                }
            };

            struct destroy_cursor
            {
                void operator()(win32::cursor_handle handle)
                {
                    win32::destroy_cursor(handle);
                }
            };

            struct destroy_icon
            {
                void operator()(win32::icon_handle handle)
                {
                    win32::destroy_icon(handle);
                }
            };

            struct destroy_menu
            {
                void operator()(win32::menu_handle handle)
                {
                    win32::destroy_menu(handle);
                }
            };

            struct destroy_window
            {
                void operator()(win32::window_handle handle)
                {
                    win32::destroy_window(handle);
                }
            };

            struct free_library
            {
                void operator()(win32::module_handle handle)
                {
                    // NOTE: Also callable with type `instance_handle`
                    win32::free_library(handle);
                }
            };

            struct release_dc
            {
                void operator()(win32::device_context_handle handle)
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
#if defined(WIN32) || defined(_WIN64)

        namespace details
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

        using unique_handle = unique_any<win32::handle_t, details::close_handle, details::invalid_handle_traits<win32::handle_t>>;
        using unique_bitmap = unique_any<win32::bitmap_handle, details::delete_object, details::null_handle_traits<win32::bitmap_handle>>;
        using unique_brush = unique_any<win32::brush_handle, details::delete_object, details::null_handle_traits<win32::brush_handle>>;
        using unique_cursor = unique_any<win32::cursor_handle, details::destroy_cursor, details::null_handle_traits<win32::cursor_handle>>;
        using unique_deletable_dc = unique_any<win32::device_context_handle, details::delete_dc, details::null_handle_traits<win32::device_context_handle>>;
        using unique_releasable_dc = unique_any<win32::device_context_handle, details::release_dc, details::null_handle_traits<win32::device_context_handle>>;
        using unique_gdi_object = unique_any<win32::gdi_object_handle, details::delete_object, details::null_handle_traits<win32::gdi_object_handle>>;
        using unique_icon = unique_any<win32::icon_handle, details::destroy_icon, details::null_handle_traits<win32::icon_handle>>;
        using unique_instance = unique_any<win32::instance_handle, details::free_library, details::null_handle_traits<win32::instance_handle>>;
        using unique_menu = unique_any<win32::menu_handle, details::destroy_menu, details::null_handle_traits<win32::menu_handle>>;
        using unique_module = unique_any<win32::module_handle, details::free_library, details::null_handle_traits<win32::module_handle>>;
        using unique_window = unique_any<win32::window_handle, details::destroy_window, details::null_handle_traits<win32::window_handle>>;

#endif
#pragma endregion

#pragma endregion
    }
}
