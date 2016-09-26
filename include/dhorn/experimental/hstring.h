/*
 * Duncan Horn
 *
 * hstring.h
 *
 * A smart wrapper around the WinRT HSTRING type
 */
#pragma once

#include <cassert>
#include <string>
#include <winstring.h>

#include "../type_traits.h"
#include "windows_exception.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * hstring_reference
         */
        class hstring_reference final
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            hstring_reference(void) = default;

            hstring_reference(const hstring_reference &other)
            {
                Assign(other);
            }

            hstring_reference(const wchar_t *str, size_t length)
            {
                Assign(str, length);
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring_reference(StringT str)
            {
                Assign(str);
            }

            template <size_t Size>
            hstring_reference(const wchar_t (&str)[Size])
            {
                Assign(str);
            }

            template <size_t Size>
            hstring_reference(wchar_t (&str)[Size])
            {
                Assign(str);
            }

            hstring_reference(const std::wstring &str)
            {
                Assign(str);
            }



            /*
             * Operators
             */
            hstring_reference &operator=(const hstring_reference &other)
            {
                Assign(other);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring_reference &operator=(StringT str)
            {
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring_reference &operator=(const wchar_t (&str)[Size])
            {
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring_reference &operator=(wchar_t (&str)[Size])
            {
                Assign(str);
                return *this;
            }

            hstring_reference &operator=(const std::wstring &str)
            {
                Assign(str);
                return *this;
            }

            operator bool(void) const noexcept
            {
                return !empty();
            }



            /*
             * Functions
             */
            hstring_reference &assign(const hstring_reference &other)
            {
                Assign(other);
                return *this;
            }

            hstring_reference &assign(const wchar_t *str, size_t length)
            {
                Assign(str, length);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring_reference &assign(StringT str)
            {
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring_reference &assign(const wchar_t (&str)[Size])
            {
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring_reference &assign(wchar_t (&str)[Size])
            {
                Assign(str);
                return *this;
            }

            hstring_reference &assign(const std::wstring &str)
            {
                Assign(str);
                return *this;
            }

            const wchar_t *c_str(void) const noexcept
            {
                return data();
            }

            void clear(void) noexcept
            {
                this->_hstr = nullptr;
            }

            void copy_to(HSTRING* target) const
            {
                throw_if_failed(::WindowsDuplicateString(this->_hstr, target));
            }

            const wchar_t *data(void) const noexcept
            {
                return ::WindowsGetStringRawBuffer(this->_hstr, nullptr);
            }

            bool empty(void) const noexcept
            {
                return !!::WindowsIsStringEmpty(this->_hstr);
            }

            HSTRING get(void) const noexcept
            {
                return this->_hstr;
            }

            size_t length(void) const noexcept
            {
                return ::WindowsGetStringLen(this->_hstr);
            }

            void reset(void) noexcept
            {
                clear();
            }

            size_t size(void) const noexcept
            {
                return length();
            }

            void swap(hstring_reference &other)
            {
                // HSTRING holds a reference to the HSTRING_HEADER. Therefore we cannot just do a normal swap. This needs
                // to be full fledged copies
                auto copy = other;
                other = *this;
                *this = copy;
            }



        private:

            void Assign(const hstring_reference &other)
            {
                if (this != &other)
                {
                    UINT32 length;
                    auto buffer = ::WindowsGetStringRawBuffer(other._hstr, &length);
                    Assign(buffer, length);
                }
            }

            void Assign(const wchar_t *str, size_t length)
            {
                throw_if_failed(::WindowsCreateStringReference(str, length, &this->_header, &this->_hstr));
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            void Assign(StringT str)
            {
                Assign(str, wcslen(str));
            }

            template <size_t Size>
            void Assign(const wchar_t (&str)[Size])
            {
                Assign(str, Size - 1);
            }

            template <size_t Size>
            void Assign(wchar_t (&str)[Size])
            {
                Assign(static_cast<const wchar_t *>(str));
            }

            void Assign(const std::wstring &str)
            {
                Assign(str.c_str(), str.length());
            }



            HSTRING _hstr = nullptr;
            HSTRING_HEADER _header;
        };



        /*
         * hstring
         */
        class hstring final
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            hstring(void) = default;

            hstring(const hstring &other) :
                hstring(other._hstr)
            {
            }

            hstring(hstring &&other) noexcept
            {
                other.swap(*this);
            }

            hstring(HSTRING str)
            {
                Assign(str);
            }

            hstring(const wchar_t *str, size_t length)
            {
                Assign(str, length);
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring(StringT str)
            {
                Assign(str);
            }

            template <size_t Size>
            hstring(const wchar_t (&str)[Size])
            {
                Assign(str);
            }

            template <size_t Size>
            hstring(wchar_t (&str)[Size])
            {
                Assign(str);
            }

            hstring(const std::wstring &str)
            {
                Assign(str);
            }

            template <typename Itr>
            hstring(Itr front, Itr back)
            {
                Assign(front, back);
            }

            ~hstring(void)
            {
                Destroy();
            }



            /*
             * Operators
             */
            hstring &operator=(const hstring &other)
            {
                if (this != &other)
                {
                    Destroy();
                    Assign(other._hstr);
                }

                return *this;
            }

            hstring &operator=(hstring &&other) noexcept
            {
                other.swap(*this);
                return *this;
            }

            hstring &operator=(HSTRING str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring &operator=(StringT str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring &operator=(const wchar_t (&str)[Size])
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring &operator=(wchar_t (&str)[Size])
            {
                Destroy();
                Assign(str);
                return *this;
            }

            hstring &operator=(const std::wstring &str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            hstring &operator+=(const hstring &other)
            {
                Append(other._hstr);
                return *this;
            }

            hstring &operator+=(HSTRING str)
            {
                Append(str);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring &operator+=(StringT str)
            {
                Append(str);
                return *this;
            }

            template <size_t Size>
            hstring &operator+=(const wchar_t (&str)[Size])
            {
                Append(str);
                return *this;
            }

            template <size_t Size>
            hstring &operator+=(wchar_t (&str)[Size])
            {
                Append(str);
                return *this;
            }

            hstring &operator+=(const std::wstring &str)
            {
                Append(str);
                return *this;
            }

            HSTRING *operator&(void) noexcept
            {
                Destroy();
                return &this->_hstr;
            }

            operator bool(void) const noexcept
            {
                return !empty();
            }



            /*
             * Functions
             */
            hstring &append(const hstring &str)
            {
                Append(str._hstr);
                return *this;
            }

            hstring &append(HSTRING str)
            {
                Append(str);
                return *this;
            }

            hstring &append(const wchar_t *str, size_t length)
            {
                Append(str, length);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring &append(StringT str)
            {
                Append(str);
                return *this;
            }

            template <size_t Size>
            hstring &append(const wchar_t (&str)[Size])
            {
                Append(str);
                return *this;
            }

            template <size_t Size>
            hstring &append(wchar_t (&str)[Size])
            {
                Append(str);
                return *this;
            }

            hstring &append(const std::wstring &str)
            {
                Append(str);
                return *this;
            }

            hstring &assign(const hstring &other)
            {
                if (this != &other)
                {
                    Destroy();
                    Assign(other._hstr);
                }

                return *this;
            }

            hstring &assign(hstring &&other) noexcept
            {
                other.swap(*this);
                return *this;
            }

            hstring &assign(HSTRING str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            hstring &assign(const wchar_t *str, size_t length)
            {
                Destroy();
                Assign(str, length);
                return *this;
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            hstring &assign(StringT str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring &assign(const wchar_t (&str)[Size])
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <size_t Size>
            hstring &assign(wchar_t (&str)[Size])
            {
                Destroy();
                Assign(str);
                return *this;
            }

            template <typename Itr>
            hstring &assign(Itr front, Itr back)
            {
                Destroy();
                Assign(front, back);
                return *this;
            }

            hstring &assign(const std::wstring &str)
            {
                Destroy();
                Assign(str);
                return *this;
            }

            void attach(HSTRING str) noexcept
            {
                Attach(str);
            }

            const wchar_t *c_str(void) const noexcept
            {
                return data();
            }

            void clear() noexcept
            {
                Destroy();
            }

            void copy_to(HSTRING* target) const
            {
                throw_if_failed(::WindowsDuplicateString(this->_hstr, target));
            }

            const wchar_t *data(void) const noexcept
            {
                return ::WindowsGetStringRawBuffer(this->_hstr, nullptr);
            }

            HSTRING detach(void) noexcept
            {
                auto result = this->_hstr;
                this->_hstr = nullptr;
                return result;
            }

            bool empty(void) const noexcept
            {
                return !!::WindowsIsStringEmpty(this->_hstr);
            }

            HSTRING get(void) const noexcept
            {
                return this->_hstr;
            }

            size_t length(void) const noexcept
            {
                return ::WindowsGetStringLen(this->_hstr);
            }

            HSTRING release(void) noexcept
            {
                return detach();
            }

            void reset(void) noexcept
            {
                Destroy();
            }

            size_t size(void) const noexcept
            {
                return length();
            }

            void swap(hstring &other) noexcept
            {
                std::swap(this->_hstr, other._hstr);
            }



        private:

            void Assign(HSTRING str)
            {
                assert(!this->_hstr);
                throw_if_failed(::WindowsDuplicateString(str, &this->_hstr));
            }

            void Assign(const wchar_t *str, size_t length)
            {
                assert(!this->_hstr);
                throw_if_failed(::WindowsCreateString(str, length, &this->_hstr));
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            void Assign(StringT str)
            {
                Assign(str, wcslen(str));
            }

            template <size_t Size>
            void Assign(const wchar_t (&str)[Size])
            {
                Assign(str, Size - 1);
            }

            template <size_t Size>
            void Assign(wchar_t (&str)[Size])
            {
                Assign(static_cast<const wchar_t *>(str));
            }

            void Assign(const std::wstring &str)
            {
                Assign(str.c_str(), str.length());
            }

            template <typename Itr>
            void Assign(Itr front, Itr back)
            {
                // We can't guarantee that front and back are located in contiguous memory locations
                std::wstring str(front, back);
                Assign(str);
            }

            void Destroy(void) noexcept
            {
                ::WindowsDeleteString(this->_hstr);
                this->_hstr = nullptr;
            }

            void Attach(HSTRING str)
            {
                Destroy();
                this->_hstr = str;
            }

            void Append(HSTRING str)
            {
                HSTRING result;
                throw_if_failed(::WindowsConcatString(this->_hstr, str, &result));
                Attach(result);
            }

            void Append(const wchar_t *str, size_t length)
            {
                Append(hstring_reference(str, length).get());
            }

            template <typename StringT, typename = std::enable_if_t<is_c_string<StringT>::value>>
            void Append(StringT str)
            {
                Append(hstring_reference(str).get());
            }

            template <size_t Size>
            void Append(const wchar_t (&str)[Size])
            {
                Append(hstring_reference(str).get());
            }

            template <size_t Size>
            void Append(wchar_t (&str)[Size])
            {
                Append(hstring_reference(str).get());
            }

            void Append(const std::wstring &str)
            {
                Append(hstring_reference(str).get());
            }



            HSTRING _hstr = nullptr;
        };



    #pragma region compare_helper

        namespace details
        {
            template <bool InhibitArrayReferences>
            struct hstring_helper
            {
                /*
                 * Comparison
                 */
                template <typename LhsTy, typename RhsTy>
                static int compare(LhsTy &&lhs, RhsTy &&rhs)
                {
                    int result;
                    ::WindowsCompareStringOrdinal(get(as_hstring(lhs)), get(as_hstring(rhs)), &result);
                    return result;
                }

                template <typename LhsTy, typename RhsTy>
                static bool equals(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) == 0;
                }

                template <typename LhsTy, typename RhsTy>
                static bool not_equals(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) != 0;
                }

                template <typename LhsTy, typename RhsTy>
                static bool less(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) < 0;
                }

                template <typename LhsTy, typename RhsTy>
                static bool less_equals(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) <= 0;
                }

                template <typename LhsTy, typename RhsTy>
                static bool greater(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) > 0;
                }

                template <typename LhsTy, typename RhsTy>
                static bool greater_equals(LhsTy &&lhs, RhsTy &&rhs)
                {
                    return compare(lhs, rhs) >= 0;
                }



                /*
                 * Append
                 */
                template <typename LhsTy, typename RhsTy>
                static hstring append(LhsTy &&lhs, RhsTy &&rhs)
                {
                    hstring result;
                    throw_if_failed(::WindowsConcatString(get(as_hstring(lhs)), get(as_hstring(rhs)), &result));
                    return result;
                }



                /*
                 * Helper Functions
                 */
                static constexpr HSTRING as_hstring(HSTRING hstr)
                {
                    return hstr;
                }

                static constexpr const hstring &as_hstring(const hstring &hstr)
                {
                    return hstr;
                }

                static constexpr const hstring_reference &as_hstring(const hstring_reference &hstr)
                {
                    return hstr;
                }

                template <typename = std::enable_if_t<InhibitArrayReferences>>
                static hstring_reference as_hstring(const wchar_t *str)
                {
                    return hstring_reference(str);
                }

                template <
                    typename StringT,
                    typename = std::enable_if_t<is_c_string<StringT>::value>,
                    typename = std::enable_if_t<!InhibitArrayReferences>>
                static hstring_reference as_hstring(StringT str)
                {
                    return hstring_reference(str);
                }

                template <size_t Size, typename = std::enable_if_t<!InhibitArrayReferences>>
                static hstring_reference as_hstring(const wchar_t (&str)[Size])
                {
                    return hstring_reference(str);
                }

                template <size_t Size, typename = std::enable_if_t<!InhibitArrayReferences>>
                static hstring_reference as_hstring(wchar_t (&str)[Size])
                {
                    return hstring_reference(str);
                }

                static hstring_reference as_hstring(const std::wstring &str)
                {
                    return hstring_reference(str);
                }



                static constexpr HSTRING get(HSTRING hstr)
                {
                    return hstr;
                }

                static HSTRING get(const hstring &hstr)
                {
                    return hstr.get();
                }

                static HSTRING get(const hstring_reference &hstr)
                {
                    return hstr.get();
                }
            };
        }

    #pragma endregion



    #pragma region equality operators

        template <typename Ty>
        auto operator==(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator==(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::equals(lhs, rhs))
        {
            return details::hstring_helper<false>::equals(lhs, rhs);
        }

    #pragma endregion



    #pragma region inequality operators

        template <typename Ty>
        auto operator!=(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator!=(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::not_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::not_equals(lhs, rhs);
        }

    #pragma endregion



    #pragma region less than operators

        template <typename Ty>
        auto operator<(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

        template <typename Ty>
        auto operator<(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::less(lhs, rhs))
        {
            return details::hstring_helper<false>::less(lhs, rhs);
        }

    #pragma endregion



    #pragma region less than or equals operators

        template <typename Ty>
        auto operator<=(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator<=(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::less_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::less_equals(lhs, rhs);
        }

    #pragma endregion



    #pragma region greater than operators

        template <typename Ty>
        auto operator>(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

        template <typename Ty>
        auto operator>(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::greater(lhs, rhs))
        {
            return details::hstring_helper<false>::greater(lhs, rhs);
        }

    #pragma endregion



    #pragma region greater than or equals operators

        template <typename Ty>
        auto operator>=(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

        template <typename Ty>
        auto operator>=(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::greater_equals(lhs, rhs))
        {
            return details::hstring_helper<false>::greater_equals(lhs, rhs);
        }

    #pragma endregion



    #pragma region append operators

        template <typename Ty>
        auto operator+(hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(const hstring &lhs, Ty &&rhs) ->
            decltype(details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(Ty &&lhs, hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(Ty &&lhs, const hstring &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(const hstring_reference &lhs, Ty &&rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(Ty &&lhs, hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

        template <typename Ty>
        auto operator+(Ty &&lhs, const hstring_reference &rhs) -> decltype(
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring>::value>(),
            std::enable_if_t<!std::is_same<std::decay_t<Ty>, hstring_reference>::value>(),
            details::hstring_helper<false>::append(lhs, rhs))
        {
            return details::hstring_helper<false>::append(lhs, rhs);
        }

#pragma endregion
    }
}
