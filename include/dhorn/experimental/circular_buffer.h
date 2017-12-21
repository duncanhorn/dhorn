/*
 * Duncan Horn
 *
 * circular_buffer.h
 *
 * 
 */
#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

namespace dhorn::experimental
{
    template <typename Ty, std::size_t Size>
    class circular_buffer
    {
        union storage
        {
            std::uint8_t _;
            Ty value;

            storage() {}
            ~storage() {}
        };

        template <typename Buffer>
        class circular_buffer_iterator
        {
        public:
            /*
             * Public Types
             */
            using value_type = typename Buffer::value_type;
            using reference = std::conditional_t<std::is_const_v<Buffer>,
                typename Buffer::const_reference,
                typename Buffer::reference>;
            using pointer = std::conditional_t<std::is_const_v<Buffer>,
                typename Buffer::const_pointer,
                typename Buffer::pointer>;
            using difference_type = typename Buffer::difference_type;
            using iterator_category = std::random_access_iterator_tag;



            /*
             * Constructor(s)/Destructor
             */
            circular_buffer_iterator() = default;

            circular_buffer_iterator(Buffer* buffer, std::size_t logical_index) noexcept :
                _buffer(buffer),
                _logicalIndex(logical_index)
            {
                assert_range();
            }



            /*
             * Increment/Decrement
             */
            circular_buffer_iterator& operator++() noexcept
            {
                return *this += 1;
            }

            circular_buffer_iterator operator++(int) noexcept
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }

            circular_buffer_iterator& operator+=(difference_type diff) noexcept
            {
                this->_logicalIndex += diff;
                assert_range();
                return *this;
            }

            friend circular_buffer_iterator operator+(circular_buffer_iterator lhs, difference_type rhs) noexcept
            {
                return lhs += rhs;
            }

            friend circular_buffer_iterator operator+(difference_type lhs, circular_buffer_iterator rhs) noexcept
            {
                return rhs += lhs;
            }

            circular_buffer_iterator& operator--() noexcept
            {
                return *this -= 1;
            }

            circular_buffer_iterator operator--(int) noexcept
            {
                auto copy = *this;
                --(*this);
                return copy;
            }

            circular_buffer_iterator& operator-=(difference_type diff) noexcept
            {
                return *this += -diff;
            }

            friend circular_buffer_iterator operator-(circular_buffer_iterator lhs, difference_type rhs) noexcept
            {
                return lhs -= rhs;
            }

            friend difference_type operator-(
                const circular_buffer_iterator& lhs,
                const circular_buffer_iterator& rhs) noexcept
            {
                return static_cast<difference_type>(lhs._logicalIndex - rhs._logicalIndex);
            }



            /*
             * Element Access
             */
            reference operator*() const noexcept
            {
                return this->_buffer->access(this->_logicalIndex);
            }

            pointer operator->() const noexcept
            {
                return &this->_buffer->access(this->_logicalIndex);
            }

            reference operator[](difference_type index) const noexcept
            {
                return this->_buffer->access(this->_logicalIndex + index);
            }



            /*
             * Comparison
             */
            bool operator==(const circular_buffer_iterator& other) const noexcept
            {
                assert(this->_buffer == other._buffer);
                return this->_logicalIndex == other._logicalIndex;
            }

            bool operator<(const circular_buffer_iterator& other) const noexcept
            {
                assert(this->_buffer == other._buffer);
                return this->_logicalIndex < other._logicalIndex;
            }

            bool operator>(const circular_buffer_iterator& other) const noexcept
            {
                assert(this->_buffer == other._buffer);
                return this->_logicalIndex > other._logicalIndex;
            }

            bool operator!=(const circular_buffer_iterator& other) const noexcept
            {
                return !(*this == other);
            }

            bool operator<=(const circular_buffer_iterator& other) const noexcept
            {
                return !(*this > other);
            }

            bool operator>=(const circular_buffer_iterator& other) const noexcept
            {
                return !(*this < other);
            }



        private:

            void assert_range() const noexcept
            {
                assert(this->_logicalIndex >= 0);
                assert(this->_logicalIndex <= this->_buffer->_size);
            }

            Buffer* _buffer = nullptr;
            std::size_t _logicalIndex = 0;
        };

    public:
        /*
         * Public Types
         */
        using value_type = Ty;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type & ;
        using const_reference = const value_type&;
        using pointer = value_type * ;
        using const_pointer = const value_type*;
        using iterator = circular_buffer_iterator<circular_buffer>;
        using const_iterator = circular_buffer_iterator<const circular_buffer>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;



        /*
         * Constructor(s)/Destructor
         */
        circular_buffer() = default;

        explicit circular_buffer(size_type count)
        {
            if (count > Size)
            {
                throw std::bad_alloc();
            }

            std::uninitialized_default_construct_n(std::addressof(this->_data.front().value), count);
            this->_size = count;
        }

        circular_buffer(size_type count, const Ty& value)
        {
            if (count > Size)
            {
                throw std::bad_alloc();
            }

            std::uninitialized_fill_n(std::addressof(this->_data.front().value), count, value);
            this->_size = count;
        }

        template <typename InputItr>
        circular_buffer(InputItr first, InputItr last)
        {
            try
            {
                for (; first != last; ++first)
                {
                    // TODO: if constexpr this check earlier for better perf?
                    if (this->_size == Size)
                    {
                        throw std::bad_alloc();
                    }

                    new (std::addressof(this->_data.front().value)) value_type(*first);
                    ++this->_size;
                }
            }
            catch (...)
            {
                std::destroy_n(std::addressof(this->_data.front().value), this->_size);
                this->_size = 0;

                throw;
            }
        }

        // TODO: Allow different size? Perhaps at least greater?
        circular_buffer(const circular_buffer& other)
        {
            std::uninitialized_copy_n(
                std::addressof(other._data.front().value),
                other._size,
                std::addressof(this->_data.front().value));
            this->_size = other._size;
        }

        // TODO: Allow different size? Perhaps at least greater?
        circular_buffer(circular_buffer&& other)
        {
            std::uninitialized_move_n(
                std::addressof(other._data.front().value),
                other._size,
                std::addressof(this->_data.front().value));
            this->_size = other._size;
        }

        explicit circular_buffer(std::initializer_list<Ty> values)
        {
            if (values.size() > Size)
            {
                throw std::bad_alloc();
            }

            std::uninitialized_copy(values.begin(), values.end(), std::addressof(this->_data.front().value));
            this->_size = values.size();
        }

        ~circular_buffer()
        {
            clear();
        }



        /*
         * Assignment
         */
        circular_buffer& operator=(const circular_buffer& other)
        {
            if (this != &other)
            {
                // TODO
            }

            return *this;
        }

        circular_buffer& operator=(circular_buffer&& other)
        {
            assert(this != &other);
            // TODO

            return *this;
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
            if (pos >= this->_size)
            {
                throw std::out_of_range("Accessing element past the end of circular_buffer");
            }

            return access(pos);
        }

        const_reference at(size_type pos) const
        {
            if (pos >= this->_size)
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
            assert(!empty());
            return this->_data[this->_start];
        }

        const_reference front() const noexcept
        {
            assert(!empty());
            return this->_data[this->_start];
        }

        reference back() noexcept
        {
            return access(this->_size - 1);
        }

        const_reference back() const noexcept
        {
            return access(this->_size - 1);
        }



        /*
         * Capacity
         */
        bool empty() const noexcept
        {
            return this->_size == 0;
        }

        size_type size() const noexcept
        {
            return this->_size;
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
            std::destroy(begin(), end());
            this->_start = 0;
            this->_size = 0;
        }

        // TODO: insert/emplace/erase once we have iterators

        void push_back(const Ty& value)
        {
            allocate_spot(value);
        }

        void push_back(Ty&& value)
        {
            allocate_spot(std::move(value));
        }

        template <typename... Args>
        reference emplace_back(Args&&... args)
        {
            return allocate_spot(std::forward<Args>(args)...);
        }

        void pop_back()
        {
            assert(!empty());
            std::destroy_at(std::addressof(back()));
            --this->_size;
        }

        void resize(size_type count)
        {
            // TODO
        }

        void resize(size_type count, const value_type& value)
        {
            // TODO
        }



        /*
         * Iterators
         */
        iterator begin() noexcept
        {
            return iterator(this, 0);
        }

        const_iterator begin() const noexcept
        {
            return cbegin();
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator(this, 0);
        }

        iterator end() noexcept
        {
            return iterator(this, this->_size);
        }

        const_iterator end() const noexcept
        {
            return cend();
        }

        const_iterator cend() const noexcept
        {
            return const_iterator(this, this->_size);
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rbegin() const noexcept
        {
            return crbegin();
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator(begin());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(end());
        }

        const_reverse_iterator rend() const noexcept
        {
            return crend();
        }

        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(end());
        }



    private:

        size_type logical_index_to_relative_unchecked(size_type index) const noexcept
        {
            auto result = this->_start + index;
            if ((result >= Size) || (result < this->_start))
            {
                result -= Size;
            }

            assert(result < Size);
            return result;
        }

        size_type logical_index_to_relative(size_type index) const noexcept
        {
            assert(index < this->_size);
            return logical_index_to_relative_unchecked(index);
        }

        reference access(size_type logical_index) noexcept
        {
            // NOTE: This is the unoptimized access path. Pre-sanitized is faster!
            auto relative_index = logical_index_to_relative(logical_index);
            return this->_data[relative_index].value;
        }

        const_reference access(size_type logical_index) const noexcept
        {
            // NOTE: This is the unoptimized access path. Pre-sanitized is faster!
            auto relative_index = logical_index_to_relative(logical_index);
            return this->_data[relative_index].value;
        }

        // NOTE: Does not increment '_size'. May remove the first element (and will thus decrement '_size')
        value_type* reserve_spot() noexcept
        {
            if (this->_size == Size)
            {
                // Buffer is full; need to evict the first element to make room for a new one
                auto ptr = &this->_data[this->_start].value;
                std::destroy_at(ptr);

                ++this->_start;
                if (this->_start >= Size)
                {
                    this->_start -= Size;
                }

                return ptr;
            }
            else
            {
                // Buffer not full; no need to evict
                auto relative_index = logical_index_to_relative_unchecked(this->_size);
                return &this->_data[relative_index].value;
            }
        }

        template <typename... Args>
        reference allocate_spot(Args&&... args)
        {
            auto ptr = reserve_spot();

            // NOTE: It's a bit difficult to provide a strong exception guarantee if the buffer is full since we need to
            // deallocate a spot before we allocate a new one. Thus, we deem it acceptable to deallocate a spot at the
            // front of the buffer even if an exception is thrown trying to add a new element. The reserve_spot will
            // take care of this for us, so if an exception is thrown, we're all good
            new (ptr) Ty(std::forward<Args>(args));
            ++this->_size;

            return *ptr;
        }



        size_type _size = 0;
        size_type _start = 0;
        std::array<storage, Size> _data;
    };
}
