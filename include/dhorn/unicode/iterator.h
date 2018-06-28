/*
 * Duncan Horn
 *
 * iterator.h
 *
 * 
 */
#pragma once

#include <iterator>

#include "encoding.h"

namespace dhorn::unicode
{
    /*
     * iterator
     *
     * A non-mutable iterator adapter that's used for reading Unicode encoded strings. It is at-best Bidirectional, but
     * may actually be less powerful, depending on the iterator category of the underlying iterator type. Note, however,
     * that the iterator type being adapted must at least satisfy ForwardIterator. This is because Iterator requires
     * CopyConstructible, and any copied iterator must be dereferencible (so long as no other copy is advanced), but the
     * act of dereferencing may require advancing the underlying iterator, effectively invalidating any copy of the
     * iterator. E.g. the following would break if allowed:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *  std::istream_iterator<char> streamItr = ...;
     *  auto itr = dhorn::unicode::iterator(streamItr);
     *  auto itrCopy = itr;
     *  *itr;       // Okay!
     *  *itrCopy;   // Undefined behavior if *itr >= 128
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
#pragma region iterator

    template <
        typename Itr,
        encoding Encoding = character_encoding_v<typename std::iterator_traits<Itr>::value_type>,
        typename Traits = encoding_traits<Encoding>>
    class iterator
    {
        using underlying_category = typename std::iterator_traits<Itr>::iterator_category;
        static_assert(std::is_base_of_v<std::forward_iterator_tag, underlying_category>,
            "dhorn::unicode::iterator must be used with iterators that satisfy at least ForwardIterator");

    public:
        /*
         * Public Types
         */
        using value_type = char32_t;
        using reference = char32_t;
        using pointer = void;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::common_type_t<std::bidirectional_iterator_tag, underlying_category>;



        /*
         * Constructor(s)/Destructor
         */
        iterator() = default;

        iterator(Itr itr) :
            _itr(itr)
        {
        }



        /*
         * Accessors
         */
        Itr base() const noexcept
        {
            return this->_itr;
        }



        /*
         * Forward Iterator
         */
        bool operator==(iterator other) const
        {
            return !(*this != other);
        }

        bool operator!=(iterator other) const
        {
            // Equality and inequality "forward" through to the underlying iterator
            return this->_itr != other._itr;
        }

        reference operator*() const
        {
            return Traits::read(this->_itr).first;
        }

        iterator& operator++()
        {
            this->_itr = Traits::next(this->_itr);
            return *this;
        }

        iterator operator++(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }



        /*
         * Bidirectional Iterator
         */
        template <
            typename Category = iterator_category,
            std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, Category>, int> = 0>
        iterator& operator--()
        {
            do
            {
                --this->_itr;
            }
            while (!Traits::is_initial_code_unit(*this->_itr));

            return *this;
        }

        template <
            typename Category = iterator_category,
            std::enable_if_t<std::is_base_of_v<std::bidirectional_iterator_tag, Category>, int> = 0>
        iterator operator--(int)
        {
            auto copy = *this;
            --(*this);
            return copy;
        }



    private:

        Itr _itr{};
    };

#pragma endregion



    /*
     * output_iterator
     */
#pragma region output_iterator
    
    template <
        typename Itr,
        encoding Encoding = character_encoding_v<typename std::iterator_traits<Itr>::value_type>,
        typename Traits = encoding_traits<Encoding>>
    class output_iterator
    {
    public:
        /*
         * Public Types
         */
        using value_type = void;
        using reference = void;
        using pointer = void;
        using difference_type = void;
        using iterator_category = std::output_iterator_tag;



        /*
         * Constructor(s)/Destructor
         */
        output_iterator(Itr itr) :
            _itr(itr)
        {
        }



        /*
         * Accessors
         */
        Itr base() const noexcept
        {
            return this->_itr;
        }



        /*
         * Output Iterator
         */
        output_iterator& operator*()
        {
            return *this;
        }

        output_iterator& operator++()
        {
            return *this;
        }

        output_iterator& operator++(int)
        {
            return *this;
        }



        /*
         * Assignment
         */
        output_iterator& operator=(char32_t value)
        {
            this->_itr = Traits::write(this->_itr, value);
            return *this;
        }



    private:

        Itr _itr;
    };



    // Adapter Functions
    template <typename Itr>
    inline auto utf8_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_8>(itr);
    }

    template <typename Itr>
    inline auto utf16_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_16>(itr);
    }

    template <typename Itr>
    inline auto utf16le_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_16le>(itr);
    }

    template <typename Itr>
    inline auto utf16be_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_16be>(itr);
    }

    template <typename Itr>
    inline auto utf32_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_32>(itr);
    }

    template <typename Itr>
    inline auto utf32le_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_32le>(itr);
    }

    template <typename Itr>
    inline auto utf32be_output_iterator(Itr itr)
    {
        return output_iterator<Itr, encoding::utf_32be>(itr);
    }

    template <encoding Encoding, typename Itr>
    inline auto make_output_iterator(Itr itr)
    {
        return output_iterator<Itr, Encoding>(itr);
    }

#pragma endregion
}
