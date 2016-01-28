/*
 * Duncan Horn
 *
 * command_line.h
 *
 * TODO: Move this to the dhorn repo
 * The command_line type is for helping deal with command line input parsing. The command_line type is (generally)
 * interfaced with using iterators. Calling begin() on a command_line type will give you a command_line_iterator which,
 * if enumerated until end(), will give all command line arguments one by one. This is, of course, not desirable since
 * the expected input for command_line is itself an iterable collection that does just that. Instead, what makes
 * command_line useful, is that each command_line_iterator is itself iterable. If the current command_line_iterator is
 * not a switch (see below for how this is defined), then begin() == end(). Otherwise, if the command_line_iterator is
 * a switch, then *begin() == *itr, and incrementing the resulting command_line_switch_iterator will give a non end()
 * command_line_switch_iterator until another switch is encountered. For example, assuming that a switch is defined as
 * having a leading character of either '-' or '/', and assuming the example command line arguments:
 *
 *      /bar opt1 opt2 /doit /doit2
 *
 * Then calling begin() on the command_line will give an command_line_iterator that, when dereferenced, references
 * "/bar". Calling begin on this iterator will give a non-end command_line_switch_iterator that, when dereferenced,
 * also references "/bar". If you then increment this switch iterator, it would give a non-end switch iterator that
 * references "opt1". Incrementing again will give a non-end switch iterator that references "opt2". Incrementing one
 * last time will give an end switch iterator. This end switch iterator is implicitly convertible back to a non-switch
 * command_line_iterator that references "/doit". Calling begin() on that converted iterator will give a non-end switch
 * iterator that refernces "/doit", and incrementing that iterator will give an end switch iterator that is implicitly
 * convertible to a command_line_iterator that references "/doit2". Calling begin() on this iterator will again give a
 * non-end switch iterator that references "/doit2". Incrementing this iterator will give an end switch iterator that,
 * when converted back to a command_line_iterator, is also the end iterator for the command_line object.
 *
 * NOTE: the conversion from command_line_switch_iterator -> command_line_iterator is only valid for switch iterators
 * that were NOT obtained by calling end() on the command_line_iterator. This limitation exists because
 * command_line_switch_iterator is a forward iterator and thus its end() iterator cannot define an explicit location
 * within the container. If this is attempted, the assignment will throw std::invalid_argument.
 */
#pragma once

#include <string>
#include <vector>

namespace dhorn
{
    /*
     * Default is_command_line_switch implementation
     */
    template <typename StringTy>
    inline constexpr bool is_command_line_switch(_In_ const StringTy &str) noexcept
    {
        return !str.empty() && ((str[0] == '/') || (str[1] == '-'));
    }

    template <typename StringTy>
    struct is_command_line_switch_t
    {
        inline constexpr bool operator()(_In_ const StringTy &str) const noexcept
        {
            return is_command_line_switch(str);
        }
    };



    /*
     * Forward declare iterator types
     */
    template <typename CmdLine, typename IsSwitch>
    class command_line_switch_iterator;

    template <typename CmdLine, typename IsSwitch>
    class command_line_iterator;



    template <typename CharT, typename CharTraits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
    class basic_command_line
    {
        /*
         * Private Types
         */
        using my_type = basic_command_line<CharT, CharTraits, Alloc>;
        using string_type = std::basic_string<CharT, CharTraits, Alloc>;

    public:
        /*
         * Public types
         */
        using value_type = string_type;
        using container = std::vector<value_type>;
        using size_type = typename container::size_type;
        using iterator = command_line_iterator<my_type, is_command_line_switch_t<value_type>>;
        using const_iterator = iterator;



        /*
         * Constructor(s)/Destructor
         */
        basic_command_line(void)
        {
        }

        basic_command_line(_In_ const container &data) :
            _data(data)
        {
        }

        basic_command_line(_Inout_ container &&data) :
            _data(std::move(data))
        {
        }

        template <typename Itr>
        basic_command_line(_In_ Itr front, _In_ Itr back) :
            _data(front, back)
        {
        }

        basic_command_line(_In_ const std::initializer_list<value_type> &data) :
            _data(data)
        {
        }



        /*
         * Data
         */
        size_type size(void) const noexcept
        {
            return this->_data.size();
        }



        /*
         * Iterators
         */
        iterator begin(void) const
        {
            return iterator(&this->_data, 0);
        }

        template <typename CustomIsSwitch>
        command_line_iterator<my_type, CustomIsSwitch> begin(_In_ const CustomIsSwitch &isSwitch) const
        {
            return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, 0);
        }

        const_iterator cbegin(void) const
        {
            return const_iterator(&this->_data, 0);
        }

        template <typename CustomIsSwitch>
        command_line_iterator<my_type, CustomIsSwitch> cbegin(_In_ const CustomIsSwitch &isSwitch) const
        {
            return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, 0);
        }

        iterator end(void) const
        {
            return iterator(&this->_data, this->_data.size());
        }

        template <typename CustomIsSwitch>
        command_line_iterator<my_type, CustomIsSwitch> end(_In_ const CustomIsSwitch &isSwitch) const
        {
            return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, this->_data.size());
        }

        const_iterator cend(void) const
        {
            return const_iterator(&this->_data, this->_data.size());
        }

        template <typename CustomIsSwitch>
        command_line_iterator<my_type, CustomIsSwitch> cend(_In_ const CustomIsSwitch &isSwitch) const
        {
            return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, this->_data.size());
        }



    private:

        container _data;
    };



    /*
     * Type definitions
     */
    using command_line = basic_command_line<char>;
    using wcommand_line = basic_command_line<wchar_t>;



    /*
     * Iterator types
     */
#pragma region Iterator Types

    template <typename CmdLine, typename IsSwitch>
    class command_line_switch_iterator :
        public std::iterator<std::forward_iterator_tag, typename const CmdLine::value_type>
    {
        /*
         * Private types
         */
        using container = typename const CmdLine::container;
        using index_t = typename container::size_type;

    public:
        /*
         * Constructor(s)/Destructor
         */
        command_line_switch_iterator(void) :
            _container(nullptr),
            _index(0)
        {
        }

        command_line_switch_iterator(_In_ const IsSwitch &isSwitch) :
            _container(nullptr),
            _index(0),
            _isSwitch(isSwitch)
        {
        }

        command_line_switch_iterator(_In_ container *container, _In_ index_t index) :
            _container(container),
            _index(index)
        {
        }

        command_line_switch_iterator(_In_ container *container, _In_ index_t index, _In_ const IsSwitch &isSwitch) :
            _container(container),
            _index(index),
            _isSwitch(isSwitch)
        {
        }



        /*
         * Forward Iterator Functions
         */
        bool operator==(_In_ const command_line_switch_iterator &other) const
        {
            if ((other._container == this->_container) && (other._index == this->_index))
            {
                // Guaranteed equal
                return true;
            }

            // Otherwise, if one is an empty end iterator, we need to check to see if the other is a logical end
            if (!other._container)
            {
                // this->_container guaranteed to be non-null; else the equality test would have passed
                return (this->_index == this->_container->size()) || IsSwitch()((*this->_container)[this->_index]);
            }
            else if (!this->_container)
            {
                // other._container guaranteed to be non-null; else the equality test would have passed
                return (other._index == other._container->size()) || IsSwitch()((*other._container)[other._index]);
            }

            return false;
        }

        bool operator!=(_In_ const command_line_switch_iterator &other) const
        {
            return !(*this == other);
        }

        value_type &operator*(void) const
        {
            return (*this->_container)[this->_index];
        }

        value_type *operator->(void) const
        {
            return &(*this->_container)[this->_index];
        }

        command_line_switch_iterator &operator++(void)
        {
            ++this->_index;
            return *this;
        }

        command_line_switch_iterator operator++(_In_ int /*unused*/)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }



    private:

        friend class command_line_iterator<CmdLine, IsSwitch>;

        container *_container;
        index_t _index;
        IsSwitch _isSwitch;
    };



    template <typename CmdLine, typename IsSwitch>
    class command_line_iterator :
        public std::iterator<std::forward_iterator_tag, typename const CmdLine::value_type>
    {
        /*
         * Private types
         */
        using container = typename const CmdLine::container;
        using index_t = typename container::size_type;

    public:
        /*
         * Public Types
         */
        using iterator = command_line_switch_iterator<CmdLine, IsSwitch>;
        using const_iterator = iterator;



        /*
         * Constructor(s)/Destructor
         */
        command_line_iterator(void) :
            _container(nullptr),
            _index(0)
        {
        }

        command_line_iterator(_In_ const IsSwitch &isSwitch) :
            _container(nullptr),
            _index(0),
            _isSwitch(isSwitch)
        {
        }

        command_line_iterator(_In_ container *container, _In_ index_t index) :
            _container(container),
            _index(index)
        {
        }

        command_line_iterator(_In_ container *container, _In_ index_t index, _In_ const IsSwitch &isSwitch) :
            _container(container),
            _index(index),
            _isSwitch(isSwitch)
        {
        }



        /*
         * Switch Iterators
         */
        command_line_iterator &operator=(_In_ const iterator &rhs)
        {
            if (!rhs._container)
            {
                throw std::invalid_argument("Cannot assign a command_line_switch_iterator to a command_line_switch"
                    " iterator that was obtained using end()");
            }

            this->_container = rhs._container;
            this->_index = rhs._index;
            return *this;
        }

        iterator begin(void) const
        {
            return iterator(this->_container, this->_index, this->_isSwitch);
        }

        const_iterator cbegin(void) const
        {
            return const_iterator(this->_container, this->_index, this->_isSwitch);
        }

        iterator end(void) const
        {
            // Empty command_line_const_iterator represents an end iterator
            return iterator(this->_isSwitch);
        }

        const_iterator cend(void) const
        {
            // Empty command_line_const_iterator represents an end iterator
            return const_iterator(this->_isSwitch);
        }



        /*
         * Forward Iterator Functions
         */
        bool operator==(_In_ const command_line_iterator &other)
        {
            return (other._container == this->_container) && (other._index == this->_index);
        }

        bool operator!=(_In_ const command_line_iterator &other)
        {
            return !(*this == other);
        }

        value_type &operator*(void) const
        {
            return (*this->_container)[this->_index];
        }

        value_type *operator->(void) const
        {
            return &(*this->_container)[this->_index];
        }

        command_line_iterator &operator++(void)
        {
            ++this->_index;
            return *this;
        }

        command_line_iterator operator++(_In_ int /*unused*/)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }



    private:

        container *_container;
        index_t _index;
        IsSwitch _isSwitch;
    };

#pragma endregion
}
