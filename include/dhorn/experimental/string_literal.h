/*
 * Duncan Horn
 *
 * string_literal.h
 *
 * 
 */
#pragma once

#include <array>
#include <cassert>
#include <string_view>

namespace dhorn::experimental
{
    /*
     * basic_string_buffer_iterator
     *
     * A reusable iterator type for strings whose lengths are known ahead of time
     */
    template <typename CharTy>
    class basic_string_buffer_iterator
    {
    public:
        /*
         * Public Types
         */
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_cv_t<CharTy>;
        using pointer = CharTy*;
        using reference = CharTy&;
        using iterator_category = std::random_access_iterator_tag;



        /*
         * Constructor(s)/Destructor
         */
        constexpr basic_string_buffer_iterator() noexcept = default;

        constexpr basic_string_buffer_iterator(CharTy* ptr) noexcept :
            _ptr(ptr)
        {
        }



        /*
         * ForwardIterator
         */
        constexpr reference operator*() const noexcept
        {
            return *this->_ptr;
        }

        constexpr pointer operator->() const noexcept
        {
            return this->_ptr;
        }

        constexpr basic_string_buffer_iterator& operator++() noexcept
        {
            ++this->_ptr;
            return *this;
        }

        constexpr basic_string_buffer_iterator operator++(int) noexcept
        {
            auto copy = *this;
            ++this->_ptr;
            return copy;
        }

        friend constexpr bool operator==(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr == rhs._ptr;
        }

        friend constexpr bool operator!=(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr != rhs._ptr;
        }



        /*
         * BidirectionalIterator
         */
        constexpr basic_string_buffer_iterator& operator--() noexcept
        {
            --this->_ptr;
            return *this;
        }

        constexpr basic_string_buffer_iterator operator--(int) noexcept
        {
            auto copy = *this;
            --this->_ptr;
            return copy;
        }



        /*
         * RandomAccessIterator
         */
        constexpr reference operator[](difference_type index) const noexcept
        {
            return this->_ptr[index];
        }

        constexpr basic_string_buffer_iterator& operator+=(difference_type diff) noexcept
        {
            this->_ptr += diff;
            return *this;
        }

        constexpr basic_string_buffer_iterator& operator-=(difference_type diff) noexcept
        {
            this->_ptr -= diff;
            return *this;
        }

        friend constexpr basic_string_buffer_iterator operator+(basic_string_buffer_iterator lhs, difference_type rhs) noexcept
        {
            return lhs += rhs;
        }

        friend constexpr basic_string_buffer_iterator operator+(difference_type lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return rhs += lhs;
        }

        friend constexpr basic_string_buffer_iterator operator-(basic_string_buffer_iterator lhs, difference_type rhs) noexcept
        {
            return lhs -= rhs;
        }

        friend constexpr difference_type operator-(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr - rhs._ptr;
        }

        friend constexpr bool operator<(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr < rhs._ptr;
        }

        friend constexpr bool operator<=(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr <= rhs._ptr;
        }

        friend constexpr bool operator>(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr > rhs._ptr;
        }

        friend constexpr bool operator>=(basic_string_buffer_iterator lhs, basic_string_buffer_iterator rhs) noexcept
        {
            return lhs._ptr >= rhs._ptr;
        }



    private:

        CharTy* _ptr = nullptr;
    };



    /*
     * basic_string_literal
     */
    template <typename CharTy, std::size_t Length>
    class basic_string_literal
    {
        template <std::size_t... Indices>
        constexpr basic_string_literal([[maybe_unused]] const CharTy* value, std::index_sequence<Indices...>) :
            _data{ value[Indices]... }
        {
        }



    public:
        /*
         * Public Types and Constants
         */
        using value_type = CharTy;
        using pointer = value_type*;
        using const_pointer = const CharTy*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = basic_string_buffer_iterator<CharTy>;
        using const_iterator = basic_string_buffer_iterator<const CharTy>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        static constexpr size_type npos = std::numeric_limits<size_type>::max();



        /*
         * Constructor(s)/Destructor
         */
        constexpr basic_string_literal() noexcept = default;

        constexpr basic_string_literal(const CharTy (&value)[Length + 1]) :
            basic_string_literal(value, std::make_index_sequence<Length>{})
        {
        }



        /*
         * Iterators
         */
        constexpr iterator begin() noexcept
        {
            return iterator(this->_data);
        }

        constexpr const_iterator begin() const noexcept
        {
            return const_iterator(this->_data);
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(this->_data);
        }

        constexpr iterator end() noexcept
        {
            return iterator(this->_data + Length);
        }

        constexpr const_iterator end() const noexcept
        {
            return const_iterator(this->_data + Length);
        }

        constexpr const_iterator cend() const noexcept
        {
            return const_iterator(this->_data + Length);
        }

        constexpr reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        constexpr const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(cend());
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(cend());
        }

        constexpr reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }



        /*
         * Element Access
         */
        constexpr reference operator[](size_type pos) noexcept
        {
            assert(pos <= Length);
            return this->_data[pos];
        }

        constexpr const_reference operator[](size_type pos) const noexcept
        {
            assert(pos <= Length);
            return this->_data[pos];
        }

        constexpr reference at(size_type pos)
        {
            check_position(pos);
            return this->_data[pos];
        }

        constexpr const_reference at(size_type pos) const
        {
            check_position(pos);
            return this->_data[pos];
        }

        constexpr reference front() noexcept
        {
            return this->_data[0];
        }

        constexpr const_reference front() const noexcept
        {
            return this->_data[0];
        }

        constexpr reference back() noexcept
        {
            return this->_data[Length - 1];
        }

        constexpr const_reference back() const noexcept
        {
            return this->_data[Length - 1];
        }

        constexpr const_pointer c_str() const noexcept
        {
            return this->_data;
        }

        constexpr pointer data() noexcept
        {
            return this->_data;
        }

        constexpr const_pointer data() const noexcept
        {
            return this->_data;
        }

        constexpr operator std::basic_string_view<CharTy>() const noexcept
        {
            return view();
        }



        /*
         * Capacity
         */
        constexpr size_type size() const noexcept
        {
            return Length;
        }

        constexpr size_type length() const noexcept
        {
            return Length;
        }

        constexpr size_type max_size() const noexcept
        {
            return Length; // TODO: numeric_limits? Is this at all useful at any time?
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return Length == 0;
        }



        /*
         * Modifiers
         */
        constexpr void swap(basic_string_literal& other) noexcept
        {
            // NOTE: std::swap is not constexpr
            auto tmp = this->_data;
            this->_data = other._data;
            other._data = tmp;
        }



        /*
         * Operations
         */
        template <std::size_t OtherLength>
        friend constexpr basic_string_literal<CharTy, Length + OtherLength> operator+(
            const basic_string_literal& lhs,
            const basic_string_literal<CharTy, OtherLength>& rhs)
        {
            basic_string_literal<CharTy, Length + OtherLength> result;
            lhs.copy(result.data(), Length);
            rhs.copy(result.data() + Length, OtherLength);
            return result;
        }

        constexpr size_type copy(CharTy* dest, size_type count, size_type pos = 0) const
        {
            check_position(pos);
            count = std::min(count, Length - pos);
            for (size_type i = 0; i < count; ++i)
            {
                dest[i] = this->_data[pos + i];
            }

            return count;
        }

        template <size_type Position = 0, size_type Count = npos, std::enable_if_t<Position <= Length, int> = 0>
        constexpr auto substr() const noexcept
        {
            constexpr size_type Size = std::min(Length - Position, Count);
            return basic_string_literal<CharTy, Size>(this->_data + Position, std::make_index_sequence<Size>{});
        }

        // TODO: compare (w/ traits?) (how about compare substrings?)

        constexpr bool starts_with(value_type ch) const noexcept
        {
            return (Length > 0) && (this->_data[0] == ch);
        }

        constexpr bool starts_with(const_pointer str) const noexcept
        {
            // NOTE: Since we assume str is null terminated, we don't need to bounds check 'i' since the comparison will
            // fail once we reach the end of _data
            for (std::size_t i = 0; str[i]; ++i)
            {
                // TODO: Traits compare?
                if (this->_data[i] != str[i])
                {
                    return false;
                }
            }

            return true;
        }

        constexpr bool starts_with(std::basic_string_view<CharTy> str) const noexcept
        {
            if (str.length() > Length)
            {
                return false;
            }

            for (std::size_t i = 0; i < str.length(); ++i)
            {
                // TODO: Traits compare?
                if (this->_data[i] != str[i])
                {
                    return false;
                }
            }

            return true;
        }

        constexpr bool ends_with(const_pointer str) const noexcept
        {
            return ends_with(std::basic_string_view<CharTy>{str});
        }

        constexpr bool ends_with(std::basic_string_view<CharTy> str) const noexcept
        {
            if (str.length() > Length)
            {
                return false;
            }

            auto pos = Length - str.length();
            for (std::size_t i = 0; i < str.length(); ++i)
            {
                // TODO: Traits compare?
                if (this->_data[pos++] != str[i])
                {
                    return false;
                }
            }

            return true;
        }



        /*
         * Search
         */
        constexpr size_type find(std::basic_string_view<CharTy> str, size_type pos = 0) const noexcept
        {
            return view().find(str, pos);
        }

        constexpr size_type find(CharTy ch, size_type pos = 0) const noexcept
        {
            return view().find(ch, pos);
        }

        constexpr size_type find(const CharTy* str, size_type pos, size_type count) const noexcept
        {
            return view().find(str, pos, count);
        }

        constexpr size_type find(const CharTy* str, size_type pos = 0) const noexcept
        {
            return view().find(str, pos);
        }

        constexpr size_type rfind(std::basic_string_view<CharTy> str, size_type pos = npos) const noexcept
        {
            return view().rfind(str, pos);
        }

        constexpr size_type rfind(CharTy ch, size_type pos = npos) const noexcept
        {
            return view().rfind(ch, pos);
        }

        constexpr size_type rfind(const CharTy* str, size_type pos, size_type count) const noexcept
        {
            return view().rfind(str, pos, count);
        }

        constexpr size_type rfind(const CharTy* str, size_type pos = npos) const noexcept
        {
            return view().rfind(str, pos);
        }

        constexpr size_type find_first_of(std::basic_string_view<CharTy> str, size_type pos = 0) const noexcept
        {
            return view().find_first_of(ch, pos);
        }

        constexpr size_type find_first_of(CharTy ch, size_type pos = 0) const noexcept
        {
            return view().find_first_of(ch, pos);
        }

        constexpr size_type find_first_of(const CharTy* str, size_type pos, size_type count) const
        {
            return view().find_first_of(str, pos, count);
        }

        constexpr size_type find_first_of(const CharTy* str, size_type pos = 0) const
        {
            return view().find_first_of(str, pos);
        }

        constexpr size_type find_last_of(std::basic_string_view<CharTy> str, size_type pos = 0) const noexcept
        {
            return view().find_last_of(ch, pos);
        }

        constexpr size_type find_last_of(CharTy ch, size_type pos = 0) const noexcept
        {
            return view().find_last_of(ch, pos);
        }

        constexpr size_type find_last_of(const CharTy* str, size_type pos, size_type count) const
        {
            return view().find_last_of(str, pos, count);
        }

        constexpr size_type find_last_of(const CharTy* str, size_type pos = 0) const
        {
            return view().find_last_of(str, pos);
        }

        constexpr size_type find_first_not_of(std::basic_string_view<CharTy> str, size_type pos = 0) const noexcept
        {
            return view().find_first_not_of(ch, pos);
        }

        constexpr size_type find_first_not_of(CharTy ch, size_type pos = 0) const noexcept
        {
            return view().find_first_not_of(ch, pos);
        }

        constexpr size_type find_first_not_of(const CharTy* str, size_type pos, size_type count) const
        {
            return view().find_first_not_of(str, pos, count);
        }

        constexpr size_type find_first_not_of(const CharTy* str, size_type pos = 0) const
        {
            return view().find_first_not_of(str, pos);
        }

        constexpr size_type find_last_not_of(std::basic_string_view<CharTy> str, size_type pos = 0) const noexcept
        {
            return view().find_last_not_of(ch, pos);
        }

        constexpr size_type find_last_not_of(CharTy ch, size_type pos = 0) const noexcept
        {
            return view().find_last_not_of(ch, pos);
        }

        constexpr size_type find_last_not_of(const CharTy* str, size_type pos, size_type count) const
        {
            return view().find_last_not_of(str, pos, count);
        }

        constexpr size_type find_last_not_of(const CharTy* str, size_type pos = 0) const
        {
            return view().find_last_not_of(str, pos);
        }

        //template <std::size_t Count, std::enable_if_t<Count <= Length, int> = 0>
        //constexpr basic_string_literal<CharTy, Length - Count> remove_prefix()
        //{
        //    return basic_string_literal<CharTy, Length - Count>(
        //        this->_data + Count,
        //        std::make_index_sequence<Length - Count>{});
        //}

        //template <std::size_t Count, std::enable_if_t<Count <= Length, int> = 0>
        //constexpr basic_string_literal<CharTy, Length - Count> remove_suffix()
        //{
        //    return basic_string_literal<CharTy, Length - Count>(
        //        this->_data,
        //        std::make_index_sequence<Length - Count>{});
        //}



    private:

        constexpr std::basic_string_view<CharTy> view() const noexcept
        {
            return std::basic_string_view<CharTy>(this->_data, Length);
        }

        constexpr void check_position(size_type pos) const
        {
            if (pos > Length)
            {
                throw std::out_of_range("invalid basic_string_literal position");
            }
        }

        CharTy _data[Length + 1] = {};
    };



    /*
     * Deduction Guides
     */
    template <typename CharTy, std::size_t Length>
    basic_string_literal(const CharTy (&)[Length]) -> basic_string_literal<CharTy, Length - 1>;



    /*
     * Type Aliases
     */
    template <std::size_t Length>
    using string_literal = basic_string_literal<char, Length>;

    template <std::size_t Length>
    using wstring_literal = basic_string_literal<wchar_t, Length>;

    template <std::size_t Length>
    using u16string_literal = basic_string_literal<char16_t, Length>;

    template <std::size_t Length>
    using u32string_literal = basic_string_literal<char32_t, Length>;
}
