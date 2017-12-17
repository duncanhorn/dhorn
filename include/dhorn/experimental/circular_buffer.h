/*
 * Duncan Horn
 *
 * circular_buffer.h
 *
 * 
 */
#pragma once

#include <cassert>
#include <cstdint>
#include <array>

namespace dhorn::experimental
{
    template <typename Ty, std::size_t Size>
    class circular_buffer
    {
        union storage
        {
            std::uint8_t _;
            Ty value;
        };

    public:
        /*
         * Public Types
         */
        using value_type = Ty;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        // TODO: iterator, const_iterator, reverse_iterator, const_reverse_iterator



        /*
         * Constructor(s)/Destructor
         */
        circular_buffer() = default;

        explicit circular_buffer(size_type count)
        {
            // TODO
        }

        circular_buffer(size_type count, const Ty& value)
        {
            // TODO
        }

        template <typename Itr>
        circular_buffer(Itr first, Itr last)
        {
            // TODO
        }

        circular_buffer(const circular_buffer& other)
        {
            // TODO
        }

        circular_buffer(circular_buffer&& other)
        {
            // TODO
        }

        circular_buffer(std::initializer_list<Ty> values)
        {
            // TODO
        }

        ~circular_buffer()
        {
            // TODO: std::destroy ???
        }



        /*
         * Assignment
         */
        circular_buffer& operator=(const circular_buffer& other)
        {
            // TODO
        }

        circular_buffer& operator=(circular_buffer&& other)
        {
            // TODO
        }

        void assign(size_type count, const Ty& value)
        {
            // TODO
        }

        template <typename Itr>
        void assign(Itr first, Itr last)
        {
            // TODO
        }

        void assign(std::initializer_list<Ty> list)
        {
            // TODO
        }



        /*
         * Element Access
         */
        reference at(size_type pos)
        {
            if (pos >= this->size)
            {
                throw std::out_of_range("Accessing element past the end of circular_buffer");
            }

            return access(pos);
        }

        const_reference at(size_type pos) const
        {
            if (pos >= this->size)
            {
                throw std::out_of_range("Accessing element past the end of circular_buffer");
            }

            return access(pos);
        }

        reference operator[](size_type pos) noexcept
        {
            return access(pos);
        }

        const_reference operator[](size_type pos) const noexcept
        {
            return access(pos);
        }

        reference front() noexcept
        {
            assert(this->size > 0);
            return this->data[this->start];
        }

        const_reference front() const noexcept
        {
            assert(this->size > 0);
            return this->data[this->start];
        }

        reference back() noexcept
        {
            return access(this->size - 1);
        }

        const_reference back() const noexcept
        {
            return access(this->size - 1);
        }



        /*
         * Capacity
         */
        bool empty() const noexcept
        {
            return this->size == 0;
        }

        size_type size() const noexcept
        {
            return this->size;
        }

        size_type max_size() const noexcept
        {
            return Size;
        }

        size_type capacity() const noexcept
        {
            return Size; // TODO: Is this actually necessary?
        }



        /*
         * Modifiers
         */
        void clear() noexcept
        {
            // TODO
        }

        // TODO: insert/emplace/erase once we have iterators

        void push_back(const Ty& value)
        {

        }

        void push_back(Ty&& value)
        {

        }



        /*
         * Iterators
         */
        // TODO



    private:

        size_type sanitize_index(size_type pos) const noexcept
        {
            assert(pos < this->size);

            pos += this->start;
            if (pos >= Size)
            {
                pos -= Size;
            }

            assert(pos < Size);
            return pos;
        }

        reference access(size_type pos) noexcept
        {
            // NOTE: This is the unoptimized access path. Pre-sanitized is faster!
            pos = sanitize_index(pos);
            return this->data[pos].value;
        }

        const_reference access(size_type pos) const noexcept
        {
            // NOTE: This is the unoptimized access path. Pre-sanitized is faster!
            pos = sanitize_index(pos);
            return this->data[pos].value;
        }



        size_type size = 0;
        size_type start = 0;
        std::array<storage, Size> data;
    };
}
