/*
 * Duncan Horn
 *
 * hresult_error.h
 *
 * 
 */
#pragma once

#include <system_error>
#include <windows.h>

namespace dhorn::windows
{
    /* 
     * hresult_category
     */
#pragma region hresult_category

    // Forward declare for use in details::hresult_error_category
    const std::error_category& hresult_category() noexcept;

    namespace details
    {
        class hresult_error_category :
            public std::error_category
        {
        public:

            explicit hresult_error_category() = default;

            /*
             * hresult_error_category
             */
            virtual const char* name() const noexcept override
            {
                return "hresult";
            }

            virtual std::error_condition default_error_condition(int err) const noexcept override
            {
                const auto facility = HRESULT_FACILITY(err);

                if (facility == FACILITY_WIN32)
                {
                    // system_category represents Win32 error codes. Check to see if that can map the error first before
                    // we try our (worse) mapping
                    const auto win32ErrCond = std::system_category().default_error_condition(HRESULT_CODE(err));
                    if (win32ErrCond.category() == std::generic_category())
                    {
                        return win32ErrCond;
                    }
                }

                // NOTE: We intentionally omit HRESULT failures that are FACILITY_WIN32
                switch (static_cast<HRESULT>(err))
                {
                case E_NOTIMPL:
                case E_ILLEGAL_METHOD_CALL:
                    return std::make_error_condition(std::errc::function_not_supported);

                case E_ABORT:
                    return std::make_error_condition(std::errc::operation_canceled);

                case E_PENDING:
                    return std::make_error_condition(std::errc::resource_unavailable_try_again);

                case E_BOUNDS:
                    return std::make_error_condition(std::errc::result_out_of_range);

                case E_POINTER:
                    return std::make_error_condition(std::errc::invalid_argument);

                default:
                    return std::error_condition(err, hresult_category());
                }
            }

            virtual std::string message(int err) const override
            {
                char errorString[256];

                sprintf_s(errorString, "HRESULT 0x%08X: ", err);
                const size_t offset = 20;

                auto size = ::FormatMessageA(
                    FORMAT_MESSAGE_FROM_SYSTEM,
                    nullptr, // Source
                    err, // MessageId
                    0, // LanguageId
                    errorString + offset,
                    std::size(errorString) - offset,
                    nullptr); // Args

                if (!size)
                {
                    sprintf_s(errorString + offset, std::size(errorString) - offset, "UNKNOWN FAILURE CODE");
                    size = 20;
                }
                else if (size >= 2)
                {
                    // FormatMessage adds a "\r\n" to the end...
                    size -= 2;
                }

                return std::string(errorString, offset + size);
            }
        };
    }

    inline const std::error_category& hresult_category() noexcept
    {
        static const details::hresult_error_category value;
        return value;
    }

#pragma endregion
}
