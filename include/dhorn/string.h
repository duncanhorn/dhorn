/*
 * Duncan Horn
 *
 * string.h
 *
 * Helpers for dealing with strings
 */
#pragma once

#include <algorithm>
#include <cassert>
#include <string>
#include <string_view>
#include <vector>

namespace dhorn
{
    /*
     * trim
     *
     * Removes all characters from the start and end of a string that match a particular criteria. There are three
     * overloads for the trim function: one that takes a function object whose signature should be `bool(CharT)`,
     * another that takes no extra arguments and calls the former overload with the `isspace` function, and a third that
     * takes two iterators and calls the first with a function object that returns true if any value in the range is
     * equal to the input character.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * auto str = "   foo bar  \r\n\t"s;
     *
     * // Overload 1
     * auto str1 = trim(str, [](char ch) { return ch == ' '; });
     *
     * // Overload 2
     * auto str2 = trim(str);
     *
     * // Overload 3
     * char arr[] = { ' ', '\r', '\t' };
     * auto str3 = trim(str, std::begin(arr), std::end(arr));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * The above example results in the following:
     *      str1: "foo bar  \r\n\t"
     *      str2: "foo bar"
     *      str3: "foo bar  \r\n"
     */
#pragma region trim

    template <typename CharT, typename Traits, typename Alloc, typename Func>
    inline std::basic_string<CharT, Traits, Alloc> trim(
        const std::basic_string<CharT, Traits, Alloc> &str,
        const Func &fn)
    {
        auto back = std::find_if(str.rbegin(), str.rend(), fn).base();
        auto front = std::find_if(std::begin(str), back, fn);
        return { front, back };
    }

    template <typename CharT, typename Traits, typename Alloc>
    inline std::basic_string<CharT, Traits, Alloc> trim(const std::basic_string<CharT, Traits, Alloc> &str)
    {
        return trim(str, [&](CharT ch) -> bool
        {
            return !isspace(static_cast<int>(ch));
        });
    }

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline std::basic_string<CharT, Traits, Alloc> trim(
        const std::basic_string<CharT, Traits, Alloc> &str,
        Itr begin,
        Itr end)
    {
        return trim(str, [&](CharT ch) -> bool
        {
            return !std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        });
    }

#pragma endregion



    /*
     * split
     *
     * Splits the specified string at each character matching a specified criteria. There are three overloads for the
     * split function: one that takes a function object whose signature should be `bool(CharT)`, another that takes a
     * single character to split at, and a third that takes a begin/end iterator representing a range of characters to
     * split at.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * auto str = "foo bar. foobar "s;
     *
     * // Overload 1
     * auto vec1 = split(str, [](char ch) { return ch == ' '; });
     *
     * // Overload 2
     * auto vec2 = split(str, ' ');
     *
     * // Overload 3
     * char arr[] = { ' ', '.' };
     * auto vec3 = split(str, std::begin(arr), std::end(arr));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * The above example results in the following:
     *      vec1: { "foo", "bar.", "foobar", "" }
     *      vec2: { "foo", "bar.", "foobar", "" }
     *      vec3: { "foo", "bar", "", "foobar", "" }
     */
#pragma region split

    template <typename CharT, typename Traits, typename Alloc, typename Func>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        const Func &fn)
    {
        using string_type = std::basic_string<CharT, Traits, Alloc>;
        std::vector<string_type> result;

        auto endItr = std::end(str);
        for (auto itr = std::begin(str); true; ++itr)
        {
            auto seqEnd = std::find_if(itr, endItr, fn);
            result.emplace_back(itr, seqEnd);

            itr = seqEnd;
            if (itr == endItr)
            {
                break;
            }
        }

        return result;
    }

    template <typename CharT, typename Traits, typename Alloc>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        CharT ch)
    {
        return split(str, [&](CharT other) -> bool
        {
            return other == ch;
        });
    }

    template <typename CharT, typename Traits, typename Alloc, typename Itr>
    inline std::vector<std::basic_string<CharT, Traits, Alloc>> split(
        const std::basic_string<CharT, Traits, Alloc> &str,
        Itr begin,
        Itr end)
    {
        return split(str, [&](CharT ch) -> bool
        {
            return std::any_of(begin, end, [&](CharT value) -> bool { return ch == value; });
        });
    }

#pragma endregion



    /*
     * basic_null_terminated_string_iterator
     *
     * A forward iterator for traversing null terminated strings.
     */
#pragma region basic_null_terminated_string_iterator

    template <typename CharT>
    class basic_null_terminated_string_iterator
    {
    public:
        /*
         * Iterator Types
         */
        using value_type = CharT;
        using reference = CharT&;
        using pointer = CharT*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;



        /*
         * Constructor(s)/Destructor
         */
        constexpr basic_null_terminated_string_iterator() :
            _ptr(nullptr)
        {
        }

        constexpr basic_null_terminated_string_iterator(CharT* ptr) :
            _ptr(ptr)
        {
        }



        /*
         * ForwardIterator
         */
        basic_null_terminated_string_iterator& operator++() noexcept
        {
            assert(this->_ptr);
            assert(*this->_ptr);
            ++this->_ptr;
            return *this;
        }

        basic_null_terminated_string_iterator operator++(int) noexcept
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        reference operator*() noexcept
        {
            return *this->_ptr;
        }

        pointer operator->() noexcept
        {
            return this->_ptr;
        }



        /*
         * Conversion
         */
        operator basic_null_terminated_string_iterator<const CharT>() const noexcept
        {
            return { this->_ptr };
        }



        /*
         * Implementation Functions
         */
        bool end() const noexcept
        {
            return !this->_ptr || !*this->_ptr;
        }

        pointer data() const noexcept
        {
            return this->_ptr;
        }



    private:

        CharT* _ptr;
    };

    template <
        typename LhsCharT,
        typename RhsCharT,
        std::enable_if_t<std::is_same_v<std::remove_const_t<LhsCharT>, std::remove_const_t<RhsCharT>>, int> = 0>
    bool operator==(
        basic_null_terminated_string_iterator<LhsCharT> lhs,
        basic_null_terminated_string_iterator<RhsCharT> rhs) noexcept
    {
        return lhs.end() ? rhs.end() : (lhs.data() == rhs.data());
    }

    template <
        typename LhsCharT,
        typename RhsCharT,
        std::enable_if_t<std::is_same_v<std::remove_const_t<LhsCharT>, std::remove_const_t<RhsCharT>>, int> = 0>
    bool operator!=(
        basic_null_terminated_string_iterator<LhsCharT> lhs,
        basic_null_terminated_string_iterator<RhsCharT> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    using null_terminated_string_iterator = basic_null_terminated_string_iterator<char>;
    using wnull_terminated_string_iterator = basic_null_terminated_string_iterator<wchar_t>;
    using u16null_terminated_string_iterator = basic_null_terminated_string_iterator<char16_t>;
    using u32null_terminated_string_iterator = basic_null_terminated_string_iterator<char32_t>;

#pragma endregion



    /*
     * basic_null_terminated_string
     *
     * Similar to std::basic_string_view, but without the length component. Intended mostly for iteration purposes with
     * basic_null_terminated_string_iterator.
     */
#pragma region basic_null_terminated_string

    template <typename CharT>
    class basic_null_terminated_string
    {
    public:
        /*
         * Types
         */
        using value_type = CharT;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = CharT&;
        using const_reference = const CharT&;
        using pointer = CharT*;
        using const_pointer = const CharT*;
        using iterator = basic_null_terminated_string_iterator<CharT>;
        using const_iterator = basic_null_terminated_string_iterator<const CharT>;



        /*
         * Constructor(s)/Destructor
         */
        constexpr basic_null_terminated_string() :
            _ptr(nullptr)
        {
        }

        constexpr basic_null_terminated_string(pointer ptr) :
            _ptr(ptr)
        {
        }



        /*
         * Iterators
         */
        constexpr iterator begin() noexcept
        {
            return iterator(this->_ptr);
        }

        constexpr const_iterator begin() const noexcept
        {
            return const_iterator(this->_ptr);
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(this->_ptr);
        }

        constexpr iterator end() noexcept
        {
            return iterator();
        }

        constexpr const_iterator end() const noexcept
        {
            return const_iterator();
        }

        constexpr const_iterator cend() const noexcept
        {
            return const_iterator();
        }



        /*
         * Assignment
         */
        constexpr basic_null_terminated_string& operator=(basic_null_terminated_string other) noexcept
        {
            this->_ptr = other._ptr;
            return *this;
        }

        template <
            typename OtherCharT,
            std::enable_if_t<std::is_same_v<std::remove_const_t<CharT>, OtherCharT>, int> = 0>
        constexpr basic_null_terminated_string& operator=(basic_null_terminated_string<OtherCharT> other) noexcept
        {
            // Allow assignment of non-const to const
            this->_ptr = other._ptr;
            return *this;
        }



        /*
         * Operators
         */
        constexpr operator basic_null_terminated_string<const CharT>() const noexcept
        {
            return basic_null_terminated_string<const CharT>(this->_ptr);
        }

        constexpr reference operator[](size_type pos) noexcept
        {
            return this->_ptr[pos];
        }

        constexpr const_reference operator[](size_type pos) const noexcept
        {
            return this->_ptr[pos];
        }



        /*
         * String Functions
         */
        constexpr const_pointer c_str() const noexcept
        {
            return this->_ptr;
        }

        constexpr pointer data() noexcept
        {
            return this->_ptr;
        }

        constexpr const_pointer data() const noexcept
        {
            return this->_ptr;
        }

        constexpr bool empty() const noexcept
        {
            return !this->_ptr || !*this->_ptr;
        }



    private:

        friend class basic_null_terminated_string<const CharT>;

        pointer _ptr;
    };

    using null_terminated_string = basic_null_terminated_string<char>;
    using const_null_terminated_string = basic_null_terminated_string<const char>;
    using wnull_terminated_string = basic_null_terminated_string<wchar_t>;
    using const_wnull_terminated_string = basic_null_terminated_string<const wchar_t>;
    using u16null_terminated_string = basic_null_terminated_string<char16_t>;
    using const_u16null_terminated_string = basic_null_terminated_string<const char16_t>;
    using u32null_terminated_string = basic_null_terminated_string<char32_t>;
    using const_u32null_terminated_string = basic_null_terminated_string<const char32_t>;

#pragma endregion



    /*
     * starts_with
     */
#pragma region starts_with

    template <typename RangeItr, typename CompareItr>
    inline bool starts_with(RangeItr rangeBegin, RangeItr rangeEnd, CompareItr compareBegin, CompareItr compareEnd)
    {
        return std::mismatch(compareBegin, compareEnd, rangeBegin, rangeEnd).first == compareEnd;
    }

    template <typename StringTy, typename CompareItr>
    inline bool starts_with(const StringTy& string, CompareItr compareBegin, CompareItr compareEnd)
    {
        return starts_with(std::begin(string), std::end(string), compareBegin, compareEnd);
    }

    template <typename StringTy, typename CompareStringTy>
    inline bool starts_with(const StringTy& string, const CompareStringTy& compareString)
    {
        return starts_with(std::begin(string), std::end(string), std::begin(compareString), std::end(compareString));
    }

    template <typename StringTy, typename CharT>
    inline bool starts_with(const StringTy& string, CharT* compareString)
    {
        return starts_with(string, basic_null_terminated_string<CharT>(compareString));
    }

    template <typename CharT, typename CompareItr>
    inline bool starts_with(CharT* string, CompareItr compareBegin, CompareItr compareEnd)
    {
        return starts_with(basic_null_terminated_string<CharT>(string), compareBegin, compareEnd);
    }

    template <typename CharT, typename StringTy>
    inline bool starts_with(CharT* string, const StringTy& compareString)
    {
        return starts_with(basic_null_terminated_string<CharT>(string), compareString);
    }

    template <
        typename CharT,
        typename OtherCharT,
        std::enable_if_t<std::is_same_v<std::remove_const_t<CharT>, std::remove_const_t<OtherCharT>>, int> = 0>
    inline bool starts_with(CharT* string, OtherCharT* compareString)
    {
        return starts_with(
            basic_null_terminated_string<CharT>(string),
            basic_null_terminated_string<OtherCharT>(compareString));
    }

#pragma endregion
}
