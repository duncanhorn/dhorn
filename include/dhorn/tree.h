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

#pragma warning(push)
#pragma warning(disable:4520)
#pragma warning(disable:4521)

namespace dhorn
{
    // Forward declare for friend declarations
    template <typename _Ty, typename _Alloc>
    class tree;

    template <typename _Tree>
    class _dhorn_tree_const_iterator;

    template <typename _Tree>
    class _dhorn_tree_iterator;



    /*
     * Enable the passing of types around without the need to have the tree type fully defined
     * before using it as a template argument
     */
    template <typename _Ty, typename _Alloc>
    struct _dhorn_tree_types
    {
        using value_type = _Ty;
        using allocator_type = _Alloc;
        using difference_type = typename allocator_type::difference_type;
        using size_type = typename allocator_type::size_type;
        using reference = typename allocator_type::reference;
        using const_reference = typename allocator_type::const_reference;
        using pointer = typename allocator_type::pointer;
        using const_pointer = typename allocator_type::const_pointer;
    };



    /*
     * Tree node class. This is an internal class and should never be referenced directly by client
     * code as it may change/disappear without notice.
     */
#pragma region Tree Node

    template <typename _TreeTypes, bool _IsSentinel = false>
    class _dhorn_tree_node
    {
        using storage_type = _dhorn_tree_node<_TreeTypes, false>;

    public:
        using allocator_type = typename _TreeTypes::allocator_type::template rebind<storage_type>::other;
        using value_type = typename _TreeTypes::value_type;
        using size_type = typename _TreeTypes::size_type;
        using node_pointer = storage_type *;
        using pnode_pointer = node_pointer *;

        // Nodes require the ability to hold valid references to to their parent, etc., so we need
        // the ability to allocate an array of node_pointers as well.
        using _Array_Alloc = typename _TreeTypes::allocator_type::template rebind<node_pointer>::other;



        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_node(void) :
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
            _alloc(other._alloc),
            _palloc(other._palloc)
        {
            this->_Copy(other);
        }

        _dhorn_tree_node(_Inout_ _dhorn_tree_node &&other) :
            _capacity(other._capacity),
            _front(other._front),
            _back(other._back),
            _alloc(std::move(other._alloc)),
            _palloc(std::move(other._palloc))
        {
            other._capacity = 0;
            other._front = nullptr;
            other._back = nullptr;
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
                this->_Copy(other);
            }

            return *this;
        }

        _dhorn_tree_node &operator=(_In_ _dhorn_tree_node &&other)
        {
            assert(this != &other);
            this->_Destroy();

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
         * Size
         */
        size_type size(void) const _NOEXCEPT
        {
            return this->_back - this->_front;
        }

        size_type recursive_size(void) _NOEXCEPT
        {
            size_type size = this->size();

            for (auto node = this->_front; node != this->_back; ++node)
            {
                size += (*node)->recursive_size();
            }

            return size;
        }



        /*
         * Modifiers
         */
        pnode_pointer insert(_In_ pnode_pointer pos, _In_ const value_type &val)
        {
            pos = this->_BuySpot(pos);
            assert(*pos == nullptr);

            *pos = this->_alloc.allocate(1);
            this->_alloc.construct(*pos, val);
            return pos;
        }

        pnode_pointer insert(_In_ pnode_pointer pos, _Inout_ value_type &&val)
        {
            pos = this->_BuySpot(pos);
            assert(*pos == nullptr);

            *pos = this->_alloc.allocate(1);
            this->_alloc.construct(*pos, std::move(val));
            return pos;
        }

        pnode_pointer erase(_In_ pnode_pointer pos)
        {
            assert(pos >= this->_front && pos < this->_back);
            node_pointer toDestroy = *pos;

            // Shift all elements down by one
            for (auto loc = pos + 1; loc != this->_back; ++loc)
            {
                *(loc - 1) = *loc;
            }
            --this->_back;

            this->_alloc.destroy(toDestroy);
            this->_alloc.deallocate(toDestroy, 1);

            // Since we never deallocate memory from the containing array, the output pointer (the
            // successor) is the same as the input
            return pos;
        }

        template <typename... _Args>
        pnode_pointer emplace(_In_ pnode_pointer pos, _In_ _Args&&... args)
        {
            pos = this->_BuySpot(pos);
            assert(*pos == nullptr);

            *pos = this->_alloc.allocate(1);
            this->_alloc.construct(*pos, std::forward<_Args>(args)...);
            return pos;
        }

        void clear(void)
        {
            this->_Destroy();
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

        void _Copy(_In_ const _dhorn_tree_node &other)
        {
            this->_Allocate(other.size());

            for (size_type i = 0; i < other.size(); i++)
            {
                this->_front[i] = this->_alloc.allocate(1);
                this->_alloc.construct(this->_front[i], *other._front[i]);
            }

            this->_back += other.size();
        }

        pnode_pointer _BuySpot(_In_ pnode_pointer pos)
        {
            assert(pos >= this->_front);
            assert(pos <= this->_back);

            // First, resize if needed
            if (this->size() == this->_capacity)
            {
                // Resizing causes back and front to change. We need to keep pos relative to the
                // new front and back
                size_type index = pos - this->_front;
                this->_Allocate(this->_capacity * 2);

                pos = this->_front + index;
            }

            // Shift all elements
            for (auto loc = this->_back; loc > pos; --loc)
            {
                *loc = *(loc - 1);
            }

            ++this->_back;
            *pos = nullptr;
            return pos;
        }

        static const size_type min_capacity = 8;

        allocator_type _alloc;
        _Array_Alloc   _palloc;
        size_type      _capacity;
        pnode_pointer  _front;
        pnode_pointer  _back;

        friend class tree<typename _TreeTypes::value_type, typename _TreeTypes::allocator_type>;
        friend class _dhorn_tree_const_iterator<tree<typename _TreeTypes::value_type, typename _TreeTypes::allocator_type>>;
        friend class _dhorn_tree_iterator<tree<typename _TreeTypes::value_type, typename _TreeTypes::allocator_type>>;
    };

    template <typename _TreeTypes>
    class _dhorn_tree_node<_TreeTypes, false> :
        public _dhorn_tree_node<_TreeTypes, true>
    {
        using _MyBase = _dhorn_tree_node<_TreeTypes, true>;

    public:

        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_node(void) :
            _value{}
        {
        }

        _dhorn_tree_node(_In_ _dhorn_tree_node &other) :
            _MyBase(other),
            _value(other._value)
        {
        }

        _dhorn_tree_node(_In_ const _dhorn_tree_node &other) :
            _MyBase(other),
            _value(other._value)
        {
        }

        _dhorn_tree_node(_Inout_ _dhorn_tree_node &&other) :
            _MyBase(std::move(other)),
            _value(std::move(other._value))
        {
        }

        _dhorn_tree_node(_In_ value_type &value) :
            _value(value)
        {
        }

        _dhorn_tree_node(_In_ const value_type &value) :
            _value(value)
        {
        }

        _dhorn_tree_node(_Inout_ value_type &&value) :
            _value(std::move(value))
        {
        }

        template <typename... _Args,
            typename = std::enable_if<std::is_constructible<value_type, _Args...>::value>::type>
        explicit _dhorn_tree_node(_Inout_ _Args&&... args) :
            _value(std::forward<_Args>(args)...)
        {
        }



        /*
         * Assignment Operators
         */
        _dhorn_tree_node &operator=(_In_ const _dhorn_tree_node &other)
        {
            if (this != &other)
            {
                _MyBase::operator=(other);
                this->_value = other._value;
            }

            return *this;
        }

        _dhorn_tree_node &operator=(_In_ _dhorn_tree_node &&other)
        {
            _MyBase::operator=(std::move(other));
            this->_value = std::move(other._value);

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



    private:

        value_type _value;
    };

#pragma endregion



    /*
     * Tree iterator types. These types should never be referenced directly.
     */
#pragma region Iterators

    template <typename _Tree>
    class _dhorn_tree_const_iterator :
        public std::iterator<std::random_access_iterator_tag, typename _Tree::value_type>
    {
    protected:
        using _MyType = _dhorn_tree_const_iterator<_Tree>;
        using _NodePointer = _dhorn_tree_node<typename _Tree::_TreeTypes> **;
        using _ParentPointer = const _dhorn_tree_node<typename _Tree::_TreeTypes, true> *;
        using _ReverseIterator = typename _Tree::const_reverse_iterator;

    public:
        using pointer = typename _Tree::const_pointer;
        using reference = typename _Tree::const_reference;
        using value_type = typename _Tree::value_type;
        using difference_type = typename _Tree::allocator_type::difference_type;
        using size_type = typename _Tree::size_type;



        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_const_iterator(void) :
            _node(nullptr),
            _parent(nullptr),
            _tree(nullptr)
        {
        }

        _dhorn_tree_const_iterator(
            _In_ _NodePointer node,
            _In_ _ParentPointer parent,
            _In_ const _Tree *tree) :
            _node(node),
            _parent(parent),
            _tree(tree)
        {
        }



        /*
         * Random Access Iterator Operators
         */
        bool operator==(_In_ const _MyType &itr) const
        {
            this->_validate_comparable(itr);
            return this->_node == itr._node;
        }

        bool operator!=(_In_ const _MyType &itr) const
        {
            return !(*this == itr);
        }

        bool operator<(_In_ const _MyType &itr) const
        {
            this->_validate_comparable(itr);
            return this->_node < itr._node;
        }

        bool operator<=(_In_ const _MyType &itr) const
        {
            this->_validate_comparable(itr);
            return this->_node <= itr._node;
        }

        bool operator>(_In_ const _MyType &itr) const
        {
            return !(*this <= itr);
        }

        bool operator>=(_In_ const _MyType &itr) const
        {
            return !(*this < itr);
        }

        reference operator*(void) const
        {
            this->_validate_dereferenceable();
            return (*this->_node)->value();
        }

        pointer operator->(void) const
        {
            this->_validate_dereferenceable();
            return &(!this->_node)->value();
        }

        _MyType &operator++(void)
        {
            this->_validate_dereferenceable();
            this->_node++;
            return *this;
        }

        _MyType operator++(_In_ int /*unused*/)
        {
            this->_validate_dereferenceable();
            auto node = *this;
            this->_node++;
            return node;
        }

        _MyType &operator--(void)
        {
            this->_node--;
            this->_validate_dereferenceable();
            return *this;
        }

        _MyType operator--(_In_ int /*unused*/)
        {
            auto node = *this;
            this->_node--;
            this->_validate_dereferenceable();
            return node;
        }

        _MyType operator+(_In_ difference_type amt) const
        {
            auto node = *this;
            node += amt;
            return node;
        }

        _MyType &operator+=(_In_ difference_type amt)
        {
            this->_node += amt;
            assert(this->_node <= this->_parent->_back);
            return *this;
        }

        _MyType operator-(_In_ difference_type amt) const
        {
            auto node = *this;
            node -= amt;
            return node;
        }

        _MyType operator-=(_In_ difference_type amt)
        {
            this->_node -= amt;
            assert(this->_node >= this->_parent->_front);
            return *this;
        }

        reference operator[](_In_ difference_type index) const
        {
            return *(*this + index);
        }

        difference_type operator-(_In_ _MyType other) const
        {
            this->_validate_comparable(other);
            return this->_node - other._node;
        }



        /*
         * Tree iterators are special as they themselves are iterable
         */
        _MyType begin(void) const
        {
            this->_validate_dereferenceable();
            return _MyType((*this->_node)->_front, *this->_node, this->_tree);
        }

        _MyType cbegin(void) const
        {
            return this->begin();
        }

        _ReverseIterator rbegin(void) const
        {
            return _ReverseIterator(this->end());
        }

        _ReverseIterator crbegin(void) const
        {
            return this->rbegin();
        }

        _MyType end(void) const
        {
            this->_validate_dereferenceable();
            return _MyType((*this->_node)->_back, *this->_node, this->_tree);
        }

        _MyType cend(void) const
        {
            return this->end();
        }

        _ReverseIterator rend(void) const
        {
            return _ReverseIterator(this->begin());
        }

        _ReverseIterator crend(void) const
        {
            return this->rend();
        }



    protected:

        void _validate_comparable(_In_ const _MyType &itr) const
        {
            assert(this->_tree);
            assert(this->_tree == itr._tree);
        }

        void _validate_dereferenceable(void) const
        {
            assert(this->_node);
            assert(this->_node < this->_parent->_back);
            assert(this->_node >= this->_parent->_front);
        }

        const _Tree    *_tree;
        _ParentPointer _parent;
        _NodePointer   _node;

        friend _Tree;
    };



    template <typename _Tree>
    class _dhorn_tree_iterator :
        public _dhorn_tree_const_iterator<_Tree>
    {
        using _MyType = _dhorn_tree_iterator<_Tree>;
        using _MyBase = _dhorn_tree_const_iterator<_Tree>;
        using _NodePointer = typename _MyBase::_NodePointer;
        using _ParentPointer = typename _MyBase::_ParentPointer;
        using _ReverseIterator = typename _Tree::reverse_iterator;
        using _ConstReverseIterator = typename _MyBase::_ReverseIterator;

    public:
        using pointer = typename _Tree::pointer;
        using reference = typename _Tree::reference;



        /*
         * Constructor(s)/Destructor
         */
        _dhorn_tree_iterator(void) :
            _MyBase()
        {
        }

        _dhorn_tree_iterator(
            _In_ _NodePointer node,
            _In_ _ParentPointer parent,
            _In_ const _Tree *tree) :
            _MyBase(node, parent, tree)
        {
        }



        /*
         * Random Access Iterator Operators
         */
        reference operator*(void) const
        {
            this->_validate_dereferenceable();
            return (*this->_node)->value();
        }

        pointer operator->(void) const
        {
            this->_validate_dereferenceable();
            return &(!this->_node)->value();
        }

        _MyType &operator++(void)
        {
            this->_validate_dereferenceable();
            this->_node++;
            return *this;
        }

        _MyType operator++(_In_ int /*unused*/)
        {
            this->_validate_dereferenceable();
            auto node = *this;
            this->_node++;
            return node;
        }

        _MyType &operator--(void)
        {
            this->_node--;
            this->_validate_dereferenceable();
            return *this;
        }

        _MyType operator--(_In_ int /*unused*/)
        {
            auto node = *this;
            this->_node--;
            this->_validate_dereferenceable();
            return node;
        }

        _MyType operator+(_In_ difference_type amt) const
        {
            auto node = *this;
            node += amt;
            return node;
        }

        _MyType &operator+=(_In_ difference_type amt)
        {
            this->_node += amt;
            assert(this->_node <= this->_parent->_back);
            return *this;
        }

        _MyType operator-(_In_ difference_type amt) const
        {
            auto node = *this;
            node -= amt;
            return node;
        }

        _MyType operator-=(_In_ difference_type amt)
        {
            this->_node -= amt;
            assert(this->_node >= this->_parent->_front);
            return *this;
        }

        reference operator[](_In_ difference_type index) const
        {
            return *(*this + index);
        }

        difference_type operator-(_In_ _MyType other) const
        {
            this->_validate_comparable(other);
            return this->_node - other._node;
        }



        /*
         * Tree iterators are special as they themselves are iterable
         */
        _MyType begin(void)
        {
            this->_validate_dereferenceable();
            return _MyType((*this->_node)->_front, *this->_node, this->_tree);
        }

        _MyBase begin(void) const
        {
            return _MyBase::begin();
        }

        _ReverseIterator rbegin(void)
        {
            return _ReverseIterator(this->end());
        }

        _ConstReverseIterator rbegin(void) const
        {
            return _MyBase::rbegin();
        }

        _MyType end(void)
        {
            this->_validate_dereferenceable();
            return _MyType((*this->_node)->_back, *this->_node, this->_tree);
        }

        _MyBase end(void) const
        {
            return _MyBase::end();
        }

        _ReverseIterator rend(void)
        {
            return _ReverseIterator(this->begin());
        }

        _ConstReverseIterator rend(void) const
        {
            return _MyBase::rend();
        }
    };

#pragma endregion



    /*
     * dhorn::tree
     */
#pragma region tree

    template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
    class tree
    {
        using _TreeTypes = _dhorn_tree_types<_Ty, _Alloc>;
        using _MyType = tree<_Ty, _Alloc>;

    public:

        /*
         * Type/Value Definitions
         */
        using value_type = typename _TreeTypes::value_type;
        using allocator_type = typename _TreeTypes::allocator_type;
        using difference_type = typename _TreeTypes::difference_type;
        using size_type = typename _TreeTypes::size_type;
        using reference = typename _TreeTypes::reference;
        using const_reference = typename _TreeTypes::const_reference;
        using pointer = typename _TreeTypes::pointer;
        using const_pointer = typename _TreeTypes::const_pointer;
        using iterator = _dhorn_tree_iterator<_MyType>;
        using const_iterator = _dhorn_tree_const_iterator<_MyType>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;



        /*
         * Constructor(s)/Destructor
         */
        tree(void) :
            _size(0)
        {
            // TODO
        }

        tree(_In_ const _MyType &other) :
            _size(other._size),
            _sentinelNode(other._sentinelNode)
        {
        }

        tree(_Inout_ _MyType &&other) :
            _size(other._size),
            _sentinelNode(std::move(other._sentinelNode))
        {
        }



        /*
         * Assignment Operators
         */
        _MyType &operator=(_In_ const _MyType &rhs)
        {
            if (this != &rhs)
            {
                this->_sentinelNode = rhs._sentinelNode;
                this->_size = rhs._size;
            }

            return *this;
        }

        _MyType &operator=(_Inout_ _MyType &&rhs)
        {
            this->_sentinelNode = std::move(rhs._sentinelNode);
            this->_size = rhs._size;

            return *this;
        }



        /*
         * Iterators
         */
        iterator begin(void) _NOEXCEPT
        {
            return iterator(this->_sentinelNode._front, &this->_sentinelNode, this);
        }

        const_iterator begin(void) const _NOEXCEPT
        {
            return const_iterator(this->_sentinelNode._front, &this->_sentinelNode, this);
        }

        iterator end(void) _NOEXCEPT
        {
            return iterator(this->_sentinelNode._back, &this->_sentinelNode, this);
        }

        const_iterator end(void) const _NOEXCEPT
        {
            return const_iterator(this->_sentinelNode._back, &this->_sentinelNode, this);
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
        iterator insert(_In_ const_iterator pos, _In_ const value_type &val)
        {
            assert(pos._tree == this);
            auto parent = const_cast<_dhorn_tree_node<_TreeTypes, true> *>(pos._parent);
            auto loc = pos._node;

            this->_size++;
            loc = parent->insert(loc, val);
            return iterator(loc, parent, this);
        }

        iterator insert(_In_ const_iterator pos, _Inout_ value_type &&val)
        {
            assert(pos._tree == this);
            auto parent = const_cast<_dhorn_tree_node<_TreeTypes, true> *>(pos._parent);
            auto loc = pos._node;

            this->_size++;
            loc = parent->insert(loc, std::move(val));
            return iterator(loc, parent, this);
        }

        template <typename _InputIterator>
        iterator insert(_In_ const_iterator pos, _In_ _InputIterator first, _In_ _InputIterator last)
        {
            // Cache the index of the first index
            auto index = pos._node - pos._parent->_front;

            std::for_each(first, last, [&](_In_ const value_type &val)
            {
                pos = this->insert(pos, val);
                ++pos;
            });

            return iterator(pos._parent->_front + index, pos._parent, this);
        }

        iterator insert(_In_ const_iterator pos, _In_ std::initializer_list<value_type> list)
        {
            return this->insert(pos, std::begin(list), std::end(list));
        }

        iterator erase(_In_ const_iterator pos)
        {
            assert(pos._tree == this);
            auto parent = const_cast<_dhorn_tree_node<_TreeTypes, true> *>(pos._parent);

            auto ptr = parent->erase(pos._node);
            this->_size--;

            return iterator(ptr, pos._parent, this);
        }

        iterator erase(_In_ const_iterator first, _In_ const_iterator last)
        {
            // Both iterators must have the same parent. Otherwise, they are inaccessible from each
            // other by normal iteration
            assert(first._parent == last._parent);
            assert(first <= last);
            auto parent = const_cast<_dhorn_tree_node<_TreeTypes, true> *>(first._parent);
            auto pos = first._node;

            // Since calling erase invalidates the iterators, we need to cache the number of
            // elements that we are removing in total
            auto count = std::distance(first, last);

            if (first._node == parent->_front && last._node == parent->_back)
            {
                this->_size -= parent->recursive_size();
                parent->clear();
                pos = parent->_front;
            }
            else
            {
                for (difference_type i = 0; i < count; i++)
                {
                    this->_size -= 1 + (*pos)->recursive_size();
                    pos = parent->erase(pos);
                }
            }

            return iterator(pos, parent, this);
        }

        void swap(_Inout_ tree &other)
        {
            size_type tempSize = other._size;
            other._size = this->_size;
            this->_size = tempSize;

            auto tempNode = std::move(this->_sentinelNode);
            this->_sentinelNode = std::move(other._sentinelNode);
            other._sentinelNode = std::move(tempNode);
        }

        void clear(void) _NOEXCEPT
        {
            this->_sentinelNode.clear();
            this->_size = 0;
        }

        template <typename... _Args>
        iterator emplace(_In_ const_iterator pos, _In_ _Args&&... args)
        {
            assert(pos._tree == this);
            auto parent = const_cast<_dhorn_tree_node<_TreeTypes, true> *>(pos._parent);
            auto loc = pos._node;

            this->_size++;
            loc = parent->emplace(loc, std::forward<_Args>(args)...);
            return iterator(loc, parent, this);
        }



    private:

        _dhorn_tree_node<_TreeTypes, true> _sentinelNode;
        size_type _size;

        friend class _dhorn_tree_const_iterator<_MyType>;
    };

#pragma endregion
}

#pragma warning(pop)



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
