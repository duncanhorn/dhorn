/*
 * windows.h
 *
 * Duncan Horn
 *
 * Mostly just wraps Win32 functions and "C++-ifys" them, throwing exceptions on failure
 */

#ifndef WIN32
static_assert(false, "Cannot include dhorn\windows\windows.h on a non-Windows machine")
#endif  /* WIN32 */

#include <cstdint>
#include <exception>
#include <Windows.h>

namespace dhorn
{
    namespace win32
    {
        /*
         * Type declarations
         */
#pragma region Type Declarations

        using tstring = std::basic_string<TCHAR>;

#pragma endregion



        /*
         * Exception Types
         */
#pragma region Exceptions

        class windows_exception :
            public std::exception
        {
        public:
            virtual HRESULT get_hresult(void) const = 0;
        };

        class hresult_exception final :
            public windows_exception
        {
        public:
            hresult_exception(_In_ HRESULT hr) :
                _hr(hr)
            {
            }

            virtual HRESULT get_hresult(void) const
            {
                return this->_hr;
            }

        private:

            HRESULT _hr;
        };

        class win32_exception final :
            public windows_exception
        {
        public:
            win32_exception(_In_ uint32_t status) :
                _status(status)
            {
            }

            uint32_t get_status(void) const
            {
                return this->_status;
            }

            virtual HRESULT get_hresult(void) const
            {
                return HRESULT_FROM_WIN32(this->_status);
            }

        private:
            uint32_t _status;
        };

#pragma endregion



        /*
         * Helpers
         */
        template <typename HandleTraits>
        class basic_handle;

        namespace garbage
        {
            template <typename Func>
            inline void make_boolean_call(Func &func)
            {
                if (func(std::forward<Args>(args)...))
                {
                    throw win32_exception(GetLastError());
                }
            }

            template <typename Func, typename... Args>
            inline void make_boolean_call(Func &func, Args&&... args)
            {
                if (!func(std::forward<Args>(args)...))
                {
                    throw win32_exception(GetLastError());
                }
            }

            template <typename HandleTraits, typename Func>
            inline basic_handle<HandleTraits> make_handle_call(Func &func)
            {
                HANDLE result = func();
                if (result == INVALID_HANDLE_VALUE)
                {
                    throw win32_exception(GetLastError());
                }

                return result;
            }

            template <typename HandleTraits, typename Func, typename... Args>
            inline basic_handle<HandleTraits> make_handle_call(Func &func, Args&&... args)
            {
                HANDLE result = func(std::forward<Args>(args)...);
                if (result == INVALID_HANDLE_VALUE)
                {
                    throw win32_exception(GetLastError());
                }

                return result;
            }

            template <typename Func>
            inline void make_hresult_call(Func &func)
            {
                HRESULT hr = func();

                if (FAILED(hr))
                {
                    throw hresult_exception(hr);
                }
            }

            template <typename Func, typename... Args>
            inline void make_hresult_call(Func &func, Args&&... args)
            {
                HRESULT hr = func(std::forward<Args>(args)...);

                if (FAILED(hr))
                {
                    throw hresult_exception(hr);
                }
            }
        }



        /*
         * HANDLE Wrapper
         */
#pragma region handle

        struct unique_handle_traits
        {
            static void close(_In_ HANDLE handle)
            {
                // NOTE: It is *okay* to call with INVALID_HANDLE_VALUE
                if (handle != INVALID_HANDLE_VALUE)
                {
                    garbage::make_boolean_call(CloseHandle, handle);
                }
            }
        };

        struct shared_handle_traits :
            public unique_handle_traits
        {
            static HANDLE copy(_In_ HANDLE handle)
            {
                // NOTE: It is *okay* to call with INVALID_HANDLE_VALUE
                if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
                {
                    // We only duplicate for current process with same access rights
                    HANDLE result;
                    HANDLE processHandle = GetCurrentProcess();

                    garbage::make_boolean_call(
                        DuplicateHandle,
                        processHandle,              // Source process
                        handle,                     // Handle to duplicate
                        processHandle,              // Destination process
                        &result,                    // Output handle
                        0,                          // Ingored (same access rights)
                        false,                      // Not inheritable
                        DUPLICATE_SAME_ACCESS);     // Same access rights as source handle

                    return result;
                }

                return handle;
            }
        };

        template <typename Traits>
        class basic_handle
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            basic_handle(void) :
                _handle(INVALID_HANDLE_VALUE)
            {
            }

            basic_handle(_In_ HANDLE handle) :
                _handle(handle)
            {
            }

            basic_handle(_In_ const basic_handle &other)
            {
                this->_handle = Traits::copy(other._handle);
            }

            basic_handle(_Inout_ basic_handle &&other) :
                _handle(other._handle)
            {
                other._handle = INVALID_HANDLE_VALUE;
            }

            ~basic_handle(void)
            {
                this->Release();
            }



            /*
             * Operators
             */
            basic_handle &operator=(_In_ const basic_handle &other)
            {
                if (this != &other)
                {
                    this->Release();
                    this->_handle = Traits::copy(other._handle);
                }

                return *this;
            }

            basic_handle &operator=(_Inout_ basic_handle &&other)
            {
                this->Release();
                this->_handle = other._handle;
                other._handle = INVALID_HANDLE_VALUE;

                return *this;
            }

            basic_handle &operator=(_In_ HANDLE basic_handle)
            {
                this->Release();
                this->_handle = basic_handle;

                return *this;
            }

            operator HANDLE(void)
            {
                return this->_handle;
            }



            /*
             * Public functions
             */
            bool invalid(void) const
            {
                // Null handles have special meaning, but they are assumed to be invalid
                return this->_handle == INVALID_HANDLE_VALUE || this->_handle == nullptr;
            }

            void release(void)
            {
                this->Release();
            }

            void swap(_Inout_ basic_handle &other)
            {
                auto temp = other._handle;
                other._handle = this->_handle;
                this->_handle = temp;
            }



        private:

            void Release(void)
            {
                auto temp = this->_handle;

                if (!this->invalid())
                {
                    this->_handle = INVALID_HANDLE_VALUE;
                    garbage::make_boolean_call(CloseHandle, temp);
                }
            }



            HANDLE _handle;
        };

        using unique_handle = basic_handle<unique_handle_traits>;
        using shared_handle = basic_handle<shared_handle_traits>;

#pragma endregion



        /*
         * Process and Thread Functions
         */
#pragma region Process and Thread

        // Add as needed

#pragma endregion



        /*
         * File Management Functions
         */
#pragma region File Management

        template <
            typename ResultHandleTraits = unique_handle_traits,
            typename SourceHandleTraits = unique_handle_traits>
        basic_handle<ResultHandleTraits> create_file(
            _In_ const tstring &fileName,
            _In_ uint32_t desiredAccess,
            _In_ uint32_t shareMode,
            _In_opt_ LPSECURITY_ATTRIBUTES securityAttributes,
            _In_ uint32_t creationDisposition,
            _In_ uint32_t flagsAndAttributes,
            _In_opt_ basic_handle<SourceHandleTraits> templateFile = unique_handle(nullptr))
        {
            return garbage::make_handle_call<ResultHandleTraits>(CreateFile, fileName.c_str(),
                desiredAccess, shareMode, securityAttributes, creationDisposition,
                flagsAndAttributes, templateFile);
        }

#pragma endregion
    }
}
