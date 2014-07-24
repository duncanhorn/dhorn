/*
 * tree.h
 *
 * Duncan Horn
 *
 *
 */
#pragma once

#ifdef  WIN32
#include <sal.h>
#endif  /* WIN32 */

#include <algorithm>
#include <set>

namespace dhorn
{
    /*
     * Tree Iterator Types. These types should never be referenced directly. 
     */
    template <typename _Tree>
    class _dhorn_tree_const_iterator :
        public std::iterator<std::bidirectional_iterator_tag, _Ty>
    {
    public:
        using value_type = typename _Tree::value_type;
        using difference_type = typename _Tree::difference_type;
        using pointer = typename _Tree::const_pointer;
        using reference = typename _Tree::const_reference;
    };

    template <typename _Tree>
    class _dhorn_tree_iterator :
        public _dhorn_tree_const_iterator<_Tree>
    {
    public:
        using pointer = typename _Tree::pointer;
        using reference = typename _Tree::reference;

    };



    template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
    class tree
    {
    public:

        /*
         * Type/Value Definitions
         */
        using _MyType = tree<_Ty, _Alloc>;
        using value_type = _Ty;
        using allocator_type = _Alloc;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
        using iterator = _dhorn_tree_iterator<_MyType>;
        using const_iterator = _dhorn_tree_const_iterator<_MyType>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using difference_type = typename std::iterator_traits<iterator>::difference_type;
        using size_type = typename allocator_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        tree(void)
        {
            // TODO
        }

        tree(_In_ const tree &other)
        {
            // TODO
        }

        tree(_Inout_ tree &&other)
        {
            // TODO
        }



        /*
         * Assignment Operators
         */
        tree &operator=(_In_ const tree &other)
        {
            if (this != &other)
            {
                // TODO
            }

            return *this;
        }

        tree &operator=(_Inout_ tree &&other)
        {
            // TODO

            return *this;
        }



        /*
         * Iterators
         */
        iterator begin(void) _NOEXCEPT
        {
            // TODO
        }

        const_iterator begin(void) const _NOEXCEPT
        {
            // TODO
        }

        iterator end(void) _NOEXCEPT
        {
            // TODO
        }

        const_iterator end(void) const _NOEXCEPT
        {
            // TODO
        }

        reverse_iterator rbegin(void) _NOEXCEPT
        {
            return reverse_iterator(this->end());
        }

        const_reverse_iterator rbegin(void) const _NOEXCEPT
        {
            return const_reverse_iterator(this->end());
        }

        reverse_iterator rend(void) _NOEXCEPT
        {
            return reverse_iterator(this->begin());
        }

        const_reverse_iterator rend(void) const _NOEXCEPT
        {
            return const_reverse_iterator(this->begin());
        }

        const_iterator cbegin(void) const _NOEXCEPT
        {
            return this->begin();
        }

        const_iterator cend(void) const _NOEXCEPT
        {
            return this->end();
        }

        const_reverse_iterator crbegin(void) const _NOEXCEPT
        {
            return this->rbegin();
        }

        const_reverse_iterator crend(void) const _NOEXCEPT
        {
            return this->rend();
        }



        /*
         * Capacity
         */
        size_type size(void) const _NOEXCEPT
        {
            return this->_size;
        }

        size_type max_size(void) const _NOEXCEPT
        {
            return this->_alloc.max_size();
        }

        bool empty(void) const _NOEXCEPT
        {
            return this->_size == 0;
        }



        /*
         * Modifiers
         */
        iterator insert(_In_ const_iterator parent, _In_ const value_type &val)
        {
            // TODO
        }

        iterator insert(_In_ const_iterator parent, _Inout_ value_type &&val)
        {
            // TODO
        }

        template <typename _InputIterator>
        void insert(_In_ const_iterator parent, _In_ _InputIterator first, _In_ _InputIterator last)
        {
            std::for_each(first, last, [&](_In_ const value_type &val)
            {
                // TODO
            });
        }

        void insert(_In_ const_iterator parent, _In_ std::initializer_list<value_type> list)
        {
            for (auto &val : list)
            {
                // TODO
            }
        }

        iterator erase(_In_ const_iterator position)
        {
            // TODO
        }

        iterator erase(_In_ const_iterator first, _In_ const_iterator last)
        {
            // TODO
        }

        void swap(_Inout_ tree &other)
        {
            // TODO
        }

        void clear(void) _NOEXCEPT
        {
            // TODO

            this->_size = 0;
        }

        template <typename... _Args>
        iterator emplace(_In_ const_iterator parent, _In_ _Args&&... args)
        {
            // TODO
        }



        /*
         * Allocator
         */
        allocator_type get_allocator(void) const _NOEXCEPT
        {
            return this->_alloc;
        }



    private:

        allocator_type  _alloc;
        size_type       _size;
    };
}



#if     (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD)

namespace std
{
    template <typename _Ty, typename _Alloc>
    void swap(_Inout_ dhorn::tree<_Ty, _Alloc> &lhs, _Inout_ dhorn::tree<_Ty, _Alloc> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif  /* (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD) */
