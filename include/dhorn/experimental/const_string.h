/*
 * Duncan Horn
 *
 * const_string.h
 *
 * 
 */
#pragma once

#include <array>
#include <string_view>
#include <type_traits>

namespace dhorn::experimental
{
    // TODO: Rename (string_array?)
    template <typename CharTy, std::size_t Length>
    class basic_const_string
    {
        // Length represents the number of characters, but we want them null terminated
        using array_type = std::array<CharTy, Length + 1>;

    public:
        /*
         * Public Types
         */
        using value_type = CharTy;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = typename array_type::iterator;
        using const_iterator = typename array_type::const_iterator;
        using reverse_iterator = typename array_type::reverse_iterator;
        using const_reverse_iterator = typename array_type::const_reverse_iterator;



        /*
         * Constructor(s)/Destructor
         */
        constexpr basic_const_string()
        {
        }

        constexpr basic_const_string(value_type ch) noexcept
        {
            fill(ch);
        }

        //template <std::size_t OtherLength>
        //constexpr basic_const_string(const basic_const_string<OtherLength>& other, size_type pos)
        //{
        //    auto sizeToCopy = other.size() - pos;
        //    if (sizeToCopy != Length)
        //    {
        //        throw std::range_error("");
        //    }
        //}

        constexpr basic_const_string(const CharTy* str)
        {
            copy_string(str);
        }

        constexpr basic_const_string(const CharTy* str, size_type count)
        {
            copy_string(str, count);
        }

        template <typename Traits>
        constexpr basic_const_string(std::basic_string_view<CharTy, Traits> str)
        {
            copy_string(str);
        }



        /*
         * Assignment
         */
        constexpr basic_const_string& operator=(const CharTy* str)
        {
            copy_string(str);
            return *this;
        }

        template <typename Traits>
        constexpr basic_const_string& operator=(std::basic_string_view<CharTy, Traits> str)
        {
            copy_string(str);
            return *this;
        }

        constexpr basic_const_string& assign(CharTy ch) noexcept
        {
            fill(ch);
            return *this;
        }

        constexpr basic_const_string& assign(const CharTy* str)
        {
            copy_string(str);
            return *this;
        }

        constexpr basic_const_string& assign(const CharTy* str, size_type count)
        {
            copy_string(str, count);
            return *this;
        }

        template <typename Traits>
        constexpr basic_const_string& assign(std::basic_string_view<CharTy, Traits> str)
        {
            copy_string(str);
            return *this;
        }



        /*
         * Capacity
         */
        constexpr bool empty() const noexcept
        {
            return Length != 0;
        }

        constexpr size_type size() const noexcept
        {
            return Length;
        }

        constexpr size_type length() const noexcept
        {
            return Length;
        }



        /*
         * Element Access
         */
        constexpr reference operator[](size_type pos) noexcept
        {
            // NOTE: Behavior is like that of std::string where accessing at `size()` returns the null character
            return this->_string[pos];
        }

        constexpr value_type operator[](size_type pos) const noexcept
        {
            // NOTE: Behavior is like that of std::string where accessing at `size()` returns the null character
            return this->_string[pos];
        }

        constexpr reference front() noexcept
        {
            return this->_string[0];
        }

        constexpr const_reference front() const noexcept
        {
            return this->_string[0];
        }

        constexpr reference back() noexcept
        {
            return this->_string[Length - 1];
        }

        constexpr const_reference back() const noexcept
        {
            return this->_string[Length - 1];
        }

        constexpr pointer data() noexcept
        {
            return this->_string.data();
        }

        constexpr const_pointer data() const noexcept
        {
            return this->_string.data();
        }

        constexpr const_pointer c_str() const noexcept
        {
            return this->_string.data();
        }

        template <typename Traits>
        constexpr operator std::basic_string_view<CharTy, Traits>() const noexcept
        {
            return { this->_string.data(), Length };
        }



        /*
         * Operations
         */



        /*
         * Iterators
         */
        constexpr iterator begin() noexcept
        {
            return this->_string.begin();
        }

        constexpr const_iterator begin() const noexcept
        {
            return this->_string.begin();
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return this->_string.cbegin();
        }

        constexpr iterator end() noexcept
        {
            // TODO: minus one!
            return this->_string.end();
        }

        constexpr const_iterator end() const noexcept
        {
            // TODO: minus one!
            return this->_string.end();
        }

        constexpr const_iterator cend() const noexcept
        {
            // TODO: minus one!
            return this->_string.cend();
        }

        constexpr reverse_iterator rbegin() noexcept
        {
            // TODO: plus one!
            return this->_string.rbegin();
        }

        constexpr const_reverse_iterator rbegin() const noexcept
        {
            // TODO: plus one!
            return this->_string.rbegin();
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            // TODO: plus one!
            return this->_string.crbegin();
        }

        constexpr reverse_iterator rend() noexcept
        {
            return this->_string.rend();
        }

        constexpr const_reverse_iterator rend() const noexcept
        {
            return this->_string.rend();
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return this->_string.crend();
        }



    private:

        constexpr void fill(CharTy ch) noexcept
        {
            for (size_type i = 0; i < Length; ++i)
            {
                this->_string[i] = ch;
            }

            this->_string[Length] = '\0';
        }

        constexpr void copy_string(const CharTy* str)
        {
            for (size_type i = 0; i < Length; ++i)
            {
                if (!str[i])
                {
                    throw std::invalid_argument("Null terminated c-style string must be exactly Length characters");
                }

                this->_string[i] = str[i];
            }

            if (str[Length])
            {
                throw std::invalid_argument("Null terminated c-style string must be exactly Length characters");
            }

            this->_string[Length] = '\0';
        }

        template <typename Traits>
        constexpr void copy_string(std::basic_string_view<CharTy, Traits> str)
        {
            if (str.length() != Length)
            {
                throw std::invalid_argument("String length must match Length");
            }

            for (size_type i = 0; i < Length; ++i)
            {
                this->_string[i] = str[i];
            }
            this->_string[Length] = '\0';
        }

        constexpr void copy_string(const CharTy* str, size_type count)
        {
            if (count != Length)
            {
                throw std::invalid_argument("Must copy Length characters");
            }

            for (size_type i = 0; i < Length; ++i)
            {
                this->_string[i] = str[i];
            }
            this->_string[Length] = '\0';
        }

        array_type _string = {};
    };



    /*
     * Type Aliases
     */
    template <std::size_t Length>
    using const_string = basic_const_string<char, Length>;

    template <std::size_t Length>
    using wconst_string = basic_const_string<wchar_t, Length>;

    template <std::size_t Length>
    using u16const_string = basic_const_string<char16_t, Length>;

    template <std::size_t Length>
    using u32const_string = basic_const_string<char32_t, Length>;
}
