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
#include <cassert>
#include <set>

namespace dhorn
{
    /*
     * Tree iterator types. These types should never be referenced directly. 
     */
#pragma region Iterators

    template <typename _Tree>
    class _dhorn_tree_const_iterator :
        public std::iterator<std::bidirectional_iterator_tag, typename _Tree::value_type>
    {
    public:
        using target_type = _Tree;
        using value_type = typename _Tree::value_type;
        using difference_type = typename _Tree::allocator_type::difference_type;
        using pointer = typename _Tree::const_pointer;
        using reference = typename _Tree::const_reference;
        using size_type = typename _Tree::size_type;



    protected:


    };

    template <typename _Tree>
    class _dhorn_tree_iterator :
        public _dhorn_tree_const_iterator<_Tree>
    {
    public:
        using pointer = typename _Tree::pointer;
        using reference = typename _Tree::reference;
    };

#pragma endregion



    /*
     * Tree node class. This is an internal class and should never be referenced directly by client
     * code as it may change/disappear without notice.
     */
#pragma region Tree Node

    template <typename _Tree>
    class _dhorn_tree_node
    {
    public:
        using allocator_type = typename _Tree::allocator_type::template rebind<_dhorn_tree_node>::other;
        using value_type = typename _Tree::value_type;
        using size_type = typename _Tree::size_type;
        using node_pointer = _dhorn_tree_node *;
        using pnode_pointer = node_pointer *;

        // Nodes require the ability to hold valid references to to their parent, etc., so we need
        // the ability to allocate an array of node_pointers as well.
        using _Array_Alloc = typename _Tree::allocator_type::template rebind<node_pointer>::other;



        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_node(void) :
            _value{},
            _capacity(0),
            _front(nullptr),
            _back(nullptr)
        {
            this->_Allocate(min_capacity);
        }

        _dhorn_tree_node(_In_ const _dhorn_tree_node &other) :
            _capacity(0),
            _front(nullptr),
            _back(nullptr),
            _value(other._value),
            _alloc(other._alloc),
            _palloc(other._palloc)
        {
            this->_Copy(other);
        }

        _dhorn_tree_node(_Inout_ _dhorn_tree_node &&other) :
            _capacity(other._capacity),
            _front(other._front),
            _back(other._back),
            _value(std::move(other._value)),
            _alloc(std::move(other._alloc)),
            _palloc(std::move(other._palloc))
        {
            other._capacity = 0;
            other._front = nullptr;
            other._back = nullptr;
        }

        _dhorn_tree_node(_In_ const value_type &value) :
            _capacity(0),
            _front(nullptr),
            _back(nullptr),
            _value(value)
        {
            this->_Allocate(min_capacity);
        }

        _dhorn_tree_node(_Inout_ value_type &&value) :
            _capacity(0),
            _front(nullptr),
            _back(nullptr),
            _value(std::move(value))
        {
            this->_Allocate(min_capacity);
        }

        ~_dhorn_tree_node(void)
        {
            this->_Destroy();
        }



        /*
         * Assignment Operators
         */
        _dhorn_tree_node &operator=(_In_ const _dhorn_tree_node &other)
        {
            if (this != &other)
            {
                this->_Destroy();

                this->_value = other._value;
                this->_Copy(other);
            }

            return *this;
        }

        _dhorn_tree_node &operator=(_In_ _dhorn_tree_node &&other)
        {
            this->_value = std::move(other._value);
            this->_alloc = std::move(other._alloc);
            this->_palloc = std::move(other._palloc);
            this->_capacity = other._capacity;
            this->_front = other._front;
            this->_back = other._back;

            other._capacity = 0;
            other._front = nullptr;
            other._back = nullptr;

            return *this;
        }



        /*
         * Accessors
         */
        value_type &value(void) _NOEXCEPT
        {
            return this->_value;
        }

        const value_type &value(void) const _NOEXCEPT
        {
            return this->_value;
        }



        /*
         * Size
         */
        size_type size(void) const _NOEXCEPT
        {
            return this->_back - this->_front;
        }



    private:

        void _Allocate(_In_ size_type new_capacity = 0)
        {
            // Must allocate more than before
            assert(new_capacity >= this->_capacity);

            if (new_capacity < min_capacity)
            {
                new_capacity = min_capacity;
            }

            // Allocate space
            this->_capacity = new_capacity;
            auto data = this->_palloc.allocate(this->_capacity);

            for (size_type i = 0; i < this->size(); i++)
            {
                data[i] = this->_front[i];
            }

            this->_palloc.deallocate(this->_front, this->size());

            this->_back = data + this->size();
            this->_front = data;
        }

        void _Destroy(void)
        {
            for (size_type i = 0; i < this->size(); i++)
            {
                this->_alloc.destroy(this->_front[i]);
                this->_alloc.deallocate(this->_front[i], 1);
            }

            this->_palloc.deallocate(this->_front, this->size());

            this->_capacity = 0;
            this->_front = nullptr;
            this->_back = nullptr;
        }

        // Must be called after copying value. This is so we can insure that the copy constructor
        // is used for _value when appropriate
        void _Copy(_In_ const _dhorn_tree_node &other)
        {
            this->_Allocate(other.size());

            for (size_type i = 0; i < other.size(); i++)
            {
                this->_front[i] = this->_alloc.allocate(1);
                this->_alloc.construct(this->_front[i], *other._front[i]);
            }
        }

        static const size_type min_capacity = 5;

        allocator_type _alloc;
        _Array_Alloc   _palloc;
        value_type     _value;
        size_type      _capacity;
        pnode_pointer  _front;
        pnode_pointer  _back;
    };

#pragma endregion



    /*
     * dhorn::tree
     */
#pragma region tree

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



    private:

        size_type       _size;
    };

#pragma endregion
}



#if     (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD)

namespace std
{
    template <typename _Ty, typename _Alloc>
    inline void swap(_Inout_ dhorn::tree<_Ty, _Alloc> &lhs, _Inout_ dhorn::tree<_Ty, _Alloc> &rhs)
    {
        lhs.swap(rhs);
    }
}

#endif  /* (!defined _DHORN_NO_STD_SWAP) && (!defined _DHORN_NO_STD) */
