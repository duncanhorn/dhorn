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
 * command_line useful, is that each command_line_iterator is itself iterable. These "switch iterators" should be
 * thought of as "references" to the iterator that was used to create it, but with different semantics. I.e. these
 * switch iterators are forward only - not random access like their source - and thus the end iterator does not
 * reference a particular element in the collection. The semantics for these different iterator types are as follows:
 *
 *  command_line_iterator:
 *      1. Behaves like any other non-mutable random access iterator (i.e. just like std::vector<Ty>::const_iterator)
 *
 *  command_line_switch_iterator:
 *      1. When created from a command_line_iterator that does NOT represent a switch:
 *          ~ Parent iterator is not advanced
 *          ~ Evaluates equal to an end iterator created from an equivalent command_line_iterator
 *      2. When created from a command_line_iterator that DOES represent a switch:
 *          ~ Parent iterator is not advanced on creation, but *is* advanced whenever the switch iterator is advanced
 *          ~ Dereferencing always references the "next" iterator relevant to the parent iterator
 *          ~ Once the "next" element is itself a switch, or references past the end of the collection, then the switch
 *            iterator will evaluate equal to the corresponding switch iterator.
 *          ~ Once the switch iterator reaches the "end," its parent iterator will naturally reference the last
 *            non-switch element in the collection
 *
 * Conceptually this can be thought of as the switch iterator "leading" the parent command_line_iterator by one
 * element, stopping when the parent reaches the last element in the contiguous set of non-switch elements. This gives
 * the most ideal iterating experience:
 *
 *      for (auto itr = std::begin(cmd); itr != std::end(cmd); ++itr)
 *      {
 *          if (is_command_line_switch(*itr))
 *          {
 *              HandleNonSwitchArg(*itr);
 *          }
 *          else
 *          {
 *              if (*itr == "/foo")
 *              {
 *                  for (auto &arg : itr)
 *                  {
 *                      ProcessFooArg(arg);
 *                  }
 *              }
 *          }
 *      }
 *
 */
#pragma once

#include <string>
#include <vector>

namespace dhorn
{
    namespace experimental
    {
        /*
         * Default is_command_line_switch implementation
         */
        template <typename StringTy>
        inline constexpr bool is_command_line_switch(const StringTy &str) noexcept
        {
            return !str.empty() && ((str[0] == '/') || (str[0] == '-'));
        }

        template <typename StringTy>
        struct is_command_line_switch_t
        {
            inline constexpr bool operator()(const StringTy &str) const noexcept
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

            basic_command_line(const container &data) :
                _data(data)
            {
            }

            basic_command_line(container &&data) :
                _data(std::move(data))
            {
            }

            template <typename Itr>
            basic_command_line(Itr front, Itr back) :
                _data(front, back)
            {
            }

            basic_command_line(const std::initializer_list<value_type> &data) :
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
            command_line_iterator<my_type, CustomIsSwitch> begin(const CustomIsSwitch &isSwitch) const
            {
                return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, 0, isSwitch);
            }

            const_iterator cbegin(void) const
            {
                return const_iterator(&this->_data, 0);
            }

            template <typename CustomIsSwitch>
            command_line_iterator<my_type, CustomIsSwitch> cbegin(const CustomIsSwitch &isSwitch) const
            {
                return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, 0, isSwitch);
            }

            iterator end(void) const
            {
                return iterator(&this->_data, this->_data.size());
            }

            template <typename CustomIsSwitch>
            command_line_iterator<my_type, CustomIsSwitch> end(const CustomIsSwitch &isSwitch) const
            {
                return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, this->_data.size(), isSwitch);
            }

            const_iterator cend(void) const
            {
                return const_iterator(&this->_data, this->_data.size());
            }

            template <typename CustomIsSwitch>
            command_line_iterator<my_type, CustomIsSwitch> cend(const CustomIsSwitch &isSwitch) const
            {
                return command_line_iterator<my_type, CustomIsSwitch>(&this->_data, this->_data.size(), isSwitch);
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
            using parent_type = command_line_iterator<CmdLine, IsSwitch>;
            using container = typename const CmdLine::container;
            using index_t = typename container::size_type;

        public:
            /*
             * Constructor(s)/Destructor
             */
            command_line_switch_iterator(const IsSwitch &isSwitch) :
                _parent(nullptr),
                _isEnd(true),
                _isSwitch(isSwitch)
            {
            }

            command_line_switch_iterator(parent_type *parent, const IsSwitch &isSwitch) :
                _parent(parent),
                _isEnd(false),
                _isSwitch(isSwitch)
            {
                this->UpdateIsEnd();

                // We also have the initial constraint that if our parent is not a switch, this is an end iterator
                if (!this->_isEnd)
                {
                    this->_isEnd = !this->_isSwitch(**this->_parent);
                }
            }



            /*
             * Forward Iterator Functions
             */
            bool operator==(const command_line_switch_iterator &other) const
            {
                if (!this->_isEnd && !other._isEnd)
                {
                    return (*this->_parent) == (*other._parent);
                }

                // At least one is an end iterator; therefore, they evaluate equal only if both are end iterators
                return this->_isEnd && other._isEnd;
            }

            bool operator!=(const command_line_switch_iterator &other) const
            {
                return !(*this == other);
            }

            value_type &operator*(void) const
            {
                return (*this->_parent->_container)[this->_parent->_index + 1];
            }

            value_type *operator->(void) const
            {
                return &(**this);
            }

            command_line_switch_iterator &operator++(void)
            {
                ++(*this->_parent);
                this->UpdateIsEnd();
                return *this;
            }

            command_line_switch_iterator operator++(int /*unused*/)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }



        private:

            void UpdateIsEnd(void)
            {
                // The conditions for an end iterator are:
                //      1. Our parent is at the last element in its collection
                //      2. Our parent is one away from another switch
                if ((this->_parent->_index == this->_parent->_container->size() - 1) || this->_isSwitch(**this))
                {
                    this->_isEnd = true;
                }
            }

            parent_type *_parent;
            bool _isEnd;
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

            command_line_iterator(const IsSwitch &isSwitch) :
                _container(nullptr),
                _index(0),
                _isSwitch(isSwitch)
            {
            }

            command_line_iterator(container *container, index_t index) :
                _container(container),
                _index(index)
            {
            }

            command_line_iterator(container *container, index_t index, const IsSwitch &isSwitch) :
                _container(container),
                _index(index),
                _isSwitch(isSwitch)
            {
            }



            /*
             * Switch Iterators
             */
            iterator begin(void)
            {
                return iterator(this, this->_isSwitch);
            }

            const_iterator cbegin(void)
            {
                return begin();
            }

            iterator end(void)
            {
                // Empty command_line_const_iterator represents an end iterator
                return iterator(this->_isSwitch);
            }

            const_iterator cend(void)
            {
                return end();
            }



            /*
             * Forward Iterator Functions
             */
            bool operator==(const command_line_iterator &other) const
            {
                return (other._container == this->_container) && (other._index == this->_index);
            }

            bool operator!=(const command_line_iterator &other) const
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

            command_line_iterator operator++(int /*unused*/)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }



        private:

            friend class command_line_switch_iterator<CmdLine, IsSwitch>;

            container *_container;
            index_t _index;
            IsSwitch _isSwitch;
        };

#pragma endregion
    }
}
