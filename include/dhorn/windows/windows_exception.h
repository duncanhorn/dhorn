/*
 * Duncan Horn
 *
 * windows_exception.h
 *
 * Exception wrappers for Windows error codes. Some exceptions (e.g. sockets) are defined outside of the win32
 * namespace/folder, and thus don't derive from dhorn::win32::windows_exception, so that should be kept in mind.
 */
#pragma once

#include <exception>

#if !(defined WIN32_LEAN_AND_MEAN) && !(defined _DHORN_NO_WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN 1
#endif

#if !(defined NOMINMAX) && !(defined _DHORN_NO_NOMINMAX)
#define NOMINMAX 1
#endif

#include <Windows.h>

namespace dhorn
{
    namespace win32
    {
        /*
         * Generic Windows Exception
         */
        class windows_exception :
            public std::exception
        {
        public:
            virtual HRESULT get_hresult(void) const = 0;
        };



        /*
         * Exception for HRESULT
         */
#pragma region HRESULT Exception

        class hresult_exception final :
            public windows_exception
        {
        public:
            explicit hresult_exception(_In_ HRESULT hr) :
                _hr(hr)
            {
            }

            virtual const char *what(void)
            {
                return "hresult_exception";
            }

            virtual HRESULT get_hresult(void) const
            {
                return this->_hr;
            }

        private:

            HRESULT _hr;
        };

        inline void throw_if_failed(_In_ HRESULT hr)
        {
            if (FAILED(hr))
            {
                throw hresult_exception(hr);
            }
        }

        inline void throw_hr_if_false(_In_ bool expression, _In_ HRESULT hr)
        {
            if (!expression)
            {
                throw hresult_exception(hr);
            }
        }

        inline void throw_hr_if_true(_In_ bool expression, _In_ HRESULT hr)
        {
            if (expression)
            {
                throw hresult_exception(hr);
            }
        }

        template <typename Ty>
        inline void throw_hr_if_null(_In_ const Ty *ptr, _In_ HRESULT hr = E_OUTOFMEMORY)
        {
            throw_hr_if_true(ptr == nullptr, hr);
        }

#pragma endregion



        /*
         * Exception for Win32 errors
         */
#pragma region WIN32 Exception

        class win32_exception final :
            public windows_exception
        {
        public:
            explicit win32_exception(_In_ uint32_t status) :
                _status(status)
            {
            }

            virtual const char *what(void)
            {
                return "win32_exception";
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

        inline void throw_last_error(void)
        {
            auto error = ::GetLastError();
            if (error)
            {
                throw win32_exception(error);
            }
        }

        inline void expect_error(_In_ uint32_t expect)
        {
            auto error = ::GetLastError();

            // We don't throw in a non-error case, even if we are expecting an error
            if (error && (error != expect))
            {
                throw win32_exception(error);
            }
        }

#pragma endregion
    }
}
