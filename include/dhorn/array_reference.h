/*
 * Duncan Horn
 *
 * array_reference.h
 *
 * The array_reference type is meant to act a lot like std::vector, only with pre-allocated data. That is, it provides
 * bounds checking, iterating, etc. Its functionality also diverges a bit by acting more like a pointer than a vector
 * (e.g. you can increment, etc.). Note that while incrementing can do a bounds check, decrement cannot (since the
 * array_reference may have been initially created in the middle of the array)
 */
#pragma once

#include <cassert>
#include <iterator>

namespace dhorn
{
    /*
     * Iterators
     */
#pragma region Iterators

    template <typename Ty>
    class array_reference_iterator :
        public std::iterator<std::random_access_iterator_tag, Ty>
    {
    public:
        /*
         * Constructor(s)/Destructor
         */
        array_reference_iterator(void) :
            _ptr(nullptr)
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
            , _end(nullptr)
#endif
        {
        }

        array_reference_iterator(
            _In_ pointer ptr
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
            , _In_ pointer end
#endif
            ) :
            _ptr(ptr)
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
            , _end(end)
#endif
        {
            CheckBounds();
        }



        /*
         * Operators
         */
        reference operator*(void) const
        {
            return *this->_ptr;
        }

        reference operator[](_In_ difference_type index) const
        {
            return this->_ptr[index];
        }

        pointer operator->(void) const
        {
            return this->_ptr;
        }

        array_reference_iterator &operator++(void)
        {
            ++this->_ptr;
            CheckBounds();
            return *this;
        }

        array_reference_iterator operator++(_In_ int /*unused*/)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        array_reference_iterator &operator+=(_In_ difference_type diff)
        {
            this->_ptr += diff;
            CheckBounds();
            return *this;
        }

        array_reference_iterator operator+(_In_ difference_type diff)
        {
            auto copy = *this;
            copy += diff;
            return copy;
        }

        array_reference_iterator &operator--(void)
        {
            // No bounds check since we can only ever move away from our limit
            --this->_ptr;
            return *this;
        }

        array_reference_iterator operator--(_In_ int /*unused*/)
        {
            auto copy = *this;
            --(*this);
            return copy;
        }

        array_reference_iterator &operator-=(_In_ difference_type diff)
        {
            // No bounds check since we can only ever move away from our limit
            this->_ptr -= diff;
            return *this;
        }

        array_reference_iterator operator-(_In_ difference_type diff)
        {
            auto copy = *this;
            copy -= diff;
            return copy;
        }

        difference_type operator-(_In_ const array_reference_iterator &other)
        {
            return this->_ptr - other._ptr;
        }

        operator array_reference_iterator<const Ty>(void) const
        {
            return array_reference_iterator<const Ty>(
                this->_ptr
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
                , this->_end
#endif
                );
        }



    private:

        inline void CheckBounds(void)
        {
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
            assert(this->_ptr <= this->_end);
#endif
        }



        pointer _ptr;
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
        pointer _end;
#endif
    };

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator==(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        // We don't really care if their ends are different since we could be comparing sub-arrays
        return &(*lhs) == &(*rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator!=(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        return &(*lhs) != &(*rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator<(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        return &(*lhs) < &(*rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator<=(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        return &(*lhs) <= &(*rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator>(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        return &(*lhs) > &(*rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value>>
    inline bool operator>=(
        _In_ const array_reference_iterator<LhsTy> &lhs,
        _In_ const array_reference_iterator<RhsTy> &rhs)
    {
        return &(*lhs) >= &(*rhs);
    }

#pragma endregion



    /*
     * array_reference
     */
    template <typename Ty>
    class array_reference
    {
    public:
        /*
         * Types
         */
        using value_type = Ty;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = Ty &;
        using const_reference = const Ty &;
        using pointer = Ty *;
        using const_pointer = const Ty *;
        using iterator = array_reference_iterator<Ty>;
        using const_iterator = array_reference_iterator<const Ty>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;



        /*
         * Constructor(s)/Destructor
         */
        array_reference(void) :
            _ptr(nullptr),
            _size(0)
        {
        }

        array_reference(_In_ pointer ptr, _In_ size_type size) :
            _ptr(ptr),
            _size(size)
        {
        }

        template <size_t Size>
        array_reference(_In_ Ty(&arr)[Size]) :
            _ptr(arr),
            _size(Size)
        {
        }

        // Default copy/move



        /*
         * Operators
         */
        reference operator[](_In_ size_type pos)
        {
            assert(pos < this->_size);
            return this->_ptr[pos];
        }

        const_reference operator[](_In_ size_type pos) const
        {
            assert(pos < this->_size);
            return this->_ptr[pos];
        }

        reference operator*(void)
        {
            assert(this->_size);
            return *this->_ptr;
        }

        const_reference operator*(void) const
        {
            assert(this->_size);
            return *this->_ptr;
        }

        array_reference &operator+=(_In_ difference_type delta)
        {
            assert((delta < 0) || (static_cast<size_t>(delta) <= this->_size));
            this->_ptr += delta;
            this->_size -= delta;

            return *this;
        }

        array_reference &operator-=(_In_ difference_type delta)
        {
            assert((delta > 0) || (static_cast<size_t>(-delta) <= this->_size));
            this->_ptr -= delta;
            this->_size += delta;

            return *this;
        }

        array_reference &operator++(void)
        {
            return *this += 1;
        }

        array_reference operator++(_In_ int /*unused*/)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        array_reference &operator--(void)
        {
            return *this -= 1;
        }

        array_reference operator--(_In_ int /*unused*/)
        {
            auto copy = *this;
            --(*this);
            return copy;
        }

        array_reference operator+(_In_ difference_type delta) const
        {
            auto copy = *this;
            copy += delta;
            return copy;
        }

        array_reference operator-(_In_ difference_type delta) const
        {
            auto copy = *this;
            copy -= delta;
            return copy;
        }



        /*
         * Functions
         */
        reference at(_In_ size_type pos)
        {
            if (pos >= this->_size)
            {
                throw std::out_of_range("array_reference index out of range");
            }

            return this->_ptr[pos];
        }

        const_reference at(_In_ size_type pos) const
        {
            if (pos >= this->_size)
            {
                throw std::out_of_range("array_reference index out of range");
            }

            return this->_ptr[pos];
        }

        reference front(void)
        {
            return (*this)[0];
        }

        const_reference front(void) const
        {
            return (*this)[0];
        }

        reference back(void)
        {
            return (*this)[this->_size - 1];
        }

        const_reference back(void) const
        {
            return (*this)[this->_size - 1];
        }

        pointer data(void)
        {
            return this->_ptr;
        }

        const_pointer data(void) const
        {
            return this->_ptr;
        }

        bool empty(void) const noexcept
        {
            return this->_size == 0;
        }

        size_type size(void) const noexcept
        {
            return this->_size;
        }

        size_type max_size(void) const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }



        /*
         * Resizing/casting
         */
        void resize(_In_ size_type size)
        {
            // It's impossible to check the safety of this operation since it is, by definition, already unsafe
            this->_size = size;
        }

        template <typename TargetTy>
        array_reference<TargetTy> cast_to(void) const
        {
            // Convert our size to units of bytes. This has potential overflow, but if that's the case, then the array
            // is not valid (it, by definition, cannot hold all the values it says it holds). If anyone tries to do
            // that, they will get undefined behavior when calling this function
            auto sizeBytes = this->_size * sizeof(Ty);

            // This will round down (if rounding is necessary), which is what we want
            return array_reference<TargetTy>(reinterpret_cast<TargetTy *>(this->_ptr), sizeBytes / sizeof(TargetTy));
        }



        /*
         * Iterators
         */
        iterator begin(void)
        {
            return MakeIterator(this->_ptr);
        }

        const_iterator begin(void) const
        {
            return MakeIterator(this->_ptr);
        }

        const_iterator cbegin(void) const
        {
            return MakeIterator(this->_ptr);
        }

        iterator end(void)
        {
            return MakeIterator(this->_ptr + this->_size);
        }

        const_iterator end(void) const
        {
            return MakeIterator(this->_ptr + this->_size);
        }

        const_iterator cend(void) const
        {
            return MakeIterator(this->_ptr + this->_size);
        }

        reverse_iterator rbegin(void)
        {
            return reverse_iterator(MakeIterator(this->_ptr + this->_size));
        }

        const_reverse_iterator rbegin(void) const
        {
            return const_reverse_iterator(MakeIterator(this->_ptr + this->_size));
        }

        const_reverse_iterator crbegin(void) const
        {
            return const_reverse_iterator(MakeIterator(this->_ptr + this->_size));
        }

        reverse_iterator rend(void)
        {
            return reverse_iterator(MakeIterator(this->_ptr));
        }

        const_reverse_iterator rend(void) const
        {
            return const_reverse_iterator(MakeIterator(this->_ptr));
        }

        const_reverse_iterator crend(void) const
        {
            return const_reverse_iterator(MakeIterator(this->_ptr));
        }



    private:

        iterator MakeIterator(_In_ pointer ptr)
        {
            return iterator(
                ptr
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
                , this->_ptr + this->_size
#endif
                );
        }

        const_iterator MakeIterator(_In_ const_pointer ptr) const
        {
            return const_iterator(
                ptr
#if (defined DEBUG) || (defined _DEBUG) || (defined DBG)
                , this->_ptr + this->_size
#endif
                );
        }



        pointer _ptr;
        size_type _size;
    };



    /*
     * Operators
     */
#pragma region Operators

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator==(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        if (lhs.data() == rhs.data() && lhs.size() == rhs.size())
        {
            return true;
        }
        else if (lhs.size() != rhs.size())
        {
            return false;
        }

        return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs));
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator!=(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        return !(lhs == rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator<(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        return std::lexicographical_compare(
            std::begin(lhs), std::end(lhs),
            std::begin(rhs), std::end(rhs),
            std::less<const LhsTy>());
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator>(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        return std::lexicographical_compare(
            std::begin(rhs), std::end(rhs),
            std::begin(lhs), std::end(lhs),
            std::less<const LhsTy>());
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator<=(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        return !(lhs > rhs);
    }

    template <
        typename LhsTy,
        typename RhsTy,
        typename = std::enable_if_t<std::is_same<const LhsTy, const RhsTy>::value >>
    inline bool operator>=(_In_ const array_reference<LhsTy> &lhs, _In_ const array_reference<RhsTy> &rhs)
    {
        return !(lhs < rhs);
    }

#pragma endregion
}
