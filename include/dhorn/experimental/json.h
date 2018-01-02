/*
 * Duncan Horn
 *
 * json.h
 *
 * A C++ implementation of JSON that can be used to convert between various C++ types and JSON strings using various
 * C++ concepts. The base type is json_value which contains to functions: type and as<Ty>. The type function returns
 * an enum of the values: object, array, number, string, boolean, or null. This indicates the types that can be
 * converted to. The as<Ty> function dynamically casts the pointer to type std::shared_ptr<Ty> and throws
 * json_exception on failure. The possible types are json_object, json_array, json_number, json_string, json_boolean,
 * and json_null.
 *
 * The json_object type is simply just a std::map that maps string names to json_value instances. Iterating over a
 * json_object will give key/value pairs of std::pair<dhorn::utf8_string, std::shared_ptr<json_value>>. Note that due
 * to the nature of json_object representing a map, the order of properties is not preserved. E.g. The JSON string:
 *
 *  {
 *      "foo" : { ... },
 *      "bar" : { ... }
 *  }
 *
 * may or may not result in a json_object that, when iterating, "foo" will come before "bar."
 *
 * The json_array type is simply just a std::vector of json_value instances. Iterating over a json_array will give
 * values of type std::shared_ptr<json_value>. Note that order *is* preserved for arrays.
 *
 * The json_number type is kept as a string so as to prevent data loss. This string can be retrieved via the str
 * function, or it can be converted using the numeric_cast<Ty> function where Ty is a numeric type (e.g. int, float,
 * double, etc.). Due to the fact that it might be desired that Ty be a user-defined type, it is suggested that clients
 * always use the dhorn::numeric_cast<Ty> (non-member) function which can be overloaded for any type Ty.
 *
 * The json_string type is simply just a front for dhorn::utf8_string that can be retrieved with the str function.
 *
 * The json_boolean type is simply just a bool that can be retrieved with the value function.
 *
 * The json_null type is significant only by name.
 *
 *
 * json_value instances can be converted to C++ objects using the json_cast function that can be overloaded for any
 * type Ty:
 *
 *      std::shared_ptr<json_value> obj = get_object();
 *      auto myFoo = json_cast<foo>(obj.get());
 *
 * The json_cast function is already defined for a set of standard types (e.g. std::vector, std::basic_string,
 * dhorn::utf_string, std::map, etc.). The expecetd input form is specified with the definition, but for the most part,
 * it is pretty obvious (e.g. json_cast<std::basic_string<...>> requires that the json_value be a json_string,
 * json_cast<std::vector<...>> requires that the json_value be a json_array. json_cast<std::map<...>> is a little less
 * straight forward, however... If the input to json_cast is a json_array, then it is expected that this array be an
 * array of other json_arrays, each containing two values each representing a key/value pair that can each be
 * json_cast-ed to the types of the desired std::map. E.g. if the cast type is std::map<int, foo> where foo is a type
 * that has a single property - value - of type int, valid input json would look like:
 *
 *      [
 *          [ 0, { "value" : 1 } ],
 *          [ 1, { "value" : 2 } ]
 *      ]
 *
 * Otherwise, the only other allowable input type is json_object, in which case, the key type for std::map must be a
 * string type, and the result will just map the property values as strings to objects json_cast-ed to the value type.
 *
 * Similarly, C++ objects can be converted to json values using the make_json function that can be overloaded for any
 * user-defined type:
 *
 *      foo myFoo = get_foo();
 *      std::shared_ptr<json_value> value = make_json(myFoo);
 *
 * And of course, the make_json function is already overloaded for a set of standard types. In general, these do the
 * reverse of what json_cast does for these types. For std::map, the result is always the array of arrays type.
 */
#pragma once

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "numeric.h"
#include "utf_string.h"

namespace dhorn
{
    namespace experimental
    {
        /*
         * Forward declare types
         */
        class json_value;
        class json_object;
        class json_array;
        class json_number;
        class json_string;
        class json_boolean;
        class json_null;



        /*
         * json_type
         */
        enum class json_type
        {
            object = 0,
            array = 1,
            number = 2,
            string = 3,
            boolean = 4,
            null = 5,
        };



        namespace details
        {
            std::shared_ptr<json_value> copy_json_value(const std::shared_ptr<json_value> &other);
        }



        /*
         * json_exception
         */
        class json_exception :
            public std::exception
        {
        public:

            // No new functionality here... just inherit all constructors
            using std::exception::exception;
        };



#pragma region JSON Types

        /*
         * json_value
         */
        class json_value :
            public std::enable_shared_from_this<json_value>
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            virtual ~json_value(void)
            {
            }



            /*
             * Functions
             */
            virtual json_type type(void) const noexcept = 0;

            template <typename Ty>
            std::shared_ptr<Ty> as(void)
            {
                auto result = std::dynamic_pointer_cast<Ty>(this->shared_from_this());
                if (!result)
                {
                    throw json_exception("Invalid json cast");
                }

                return result;
            }

            template <typename Ty>
            std::shared_ptr<const Ty> as(void) const
            {
                auto result = std::dynamic_pointer_cast<const Ty>(this->shared_from_this());
                if (!result)
                {
                    throw json_exception("Invalid json cast");
                }

                return result;
            }
        };



        /*
         * json_object
         */
#pragma region json_object

        class json_object;

        namespace details
        {
            template <typename MapItr>
            class json_object_iterator_base
            {
            protected:
                friend class json_object;

                using internal_iterator = MapItr;

            public:
                /*
                 * Iterator Types
                 */
                using iterator_category = typename MapItr::iterator_category;
                using value_type = typename MapItr::value_type;
                using difference_type = typename MapItr::difference_type;
                using pointer = typename MapItr::pointer;
                using reference = typename MapItr::reference;



                /*
                 * Constructor(s)/Destructor
                 */
                json_object_iterator_base(void) = default;

                json_object_iterator_base(internal_iterator itr) :
                    _itr(itr)
                {
                }



                /*
                 * Operators
                 */
                bool operator==(const json_object_iterator_base &other)
                {
                    return this->_itr == other._itr;
                }

                bool operator!=(const json_object_iterator_base &other)
                {
                    return this->_itr != other._itr;
                }

                reference operator*(void)
                {
                    return *this->_itr;
                }

                pointer operator->(void)
                {
                    return this->_itr.operator->();
                }

                json_object_iterator_base &operator++(void)
                {
                    ++this->_itr;
                    return *this;
                }

                json_object_iterator_base operator++(int)
                {
                    return json_object_iterator_base(this->_itr++);
                }

                json_object_iterator_base &operator--(void)
                {
                    --this->_itr;
                    return *this;
                }

                json_object_iterator_base operator--(int)
                {
                    return json_object_iterator_base(this->_itr--);
                }

            protected:

                internal_iterator _itr;
            };



            class json_object_iterator :
                public json_object_iterator_base<std::map<utf8_string, std::shared_ptr<json_value>>::iterator>
            {
                using MyBase = json_object_iterator_base<std::map<utf8_string, std::shared_ptr<json_value>>::iterator>;

            public:
                using MyBase::MyBase;
            };

            class json_object_const_iterator :
                public json_object_iterator_base<std::map<utf8_string, std::shared_ptr<json_value>>::const_iterator>
            {
                using MyBase =
                    json_object_iterator_base<std::map<utf8_string, std::shared_ptr<json_value>>::const_iterator>;

            public:
                using MyBase::MyBase;
            };
        }



        class json_object final :
            public json_value
        {
            using pointer_type = std::shared_ptr<json_value>;
            using container_type = std::map<utf8_string, pointer_type>;

        public:

            /*
             * Public Types
             */
            using size_type = container_type::size_type;
            using iterator = details::json_object_iterator;
            using const_iterator = details::json_object_const_iterator;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;



            /*
             * Constructor(s)/Destructor
             */
            json_object()
            {
            }

            json_object(const json_object &other)
            {
                this->copy(other);
            }

            json_object(json_object &&) = default;



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::object;
            }



            /*
             * Operators
             */
            json_object &operator=(const json_object &other)
            {
                if (&other != this)
                {
                    this->copy(other);
                }

                return *this;
            }

            json_object &operator=(json_object &&) = default;

            std::shared_ptr<json_value> operator[](const utf8_string &str) const
            {
                auto itr = this->_map.find(str);

                if (itr != this->_map.end())
                {
                    return itr->second;
                }

                // Does not exist
                return nullptr;
            }



            /*
             * Iterators
             */
            iterator begin(void)
            {
                return iterator(this->_map.begin());
            }

            const_iterator begin(void) const
            {
                return this->cbegin();
            }

            const_iterator cbegin(void) const
            {
                return const_iterator(this->_map.begin());
            }

            reverse_iterator rbegin(void)
            {
                return reverse_iterator(this->_map.end());
            }

            const_reverse_iterator rbegin(void) const
            {
                return this->crbegin();
            }

            const_reverse_iterator crbegin(void) const
            {
                return const_reverse_iterator(this->_map.end());
            }

            iterator end(void)
            {
                return iterator(this->_map.end());
            }

            const_iterator end(void) const
            {
                return this->cend();
            }

            const_iterator cend(void) const
            {
                return const_iterator(this->_map.end());
            }

            reverse_iterator rend(void)
            {
                return reverse_iterator(this->_map.begin());
            }

            const_reverse_iterator rend(void) const
            {
                return this->crend();
            }

            const_reverse_iterator crend(void) const
            {
                return const_reverse_iterator(this->_map.begin());
            }



            /*
             * Functions
             */
            void clear()
            {
                this->_map.clear();
            }

            bool empty() const noexcept
            {
                return this->_map.empty();
            }

            size_type size() const noexcept
            {
                return this->_map.size();
            }

            void swap(json_object &other)
            {
                this->_map.swap(other._map);
            }



        private:

            void copy(const json_object &other)
            {
                this->_map.clear();

                for (auto &pair : other._map)
                {
                    this->_map.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(pair.first),
                        std::forward_as_tuple(details::copy_json_value(pair.second)));
                }
            }

            container_type _map;
        };

#pragma endregion



        /*
         * json_array
         */
        class json_array final :
            public json_value
        {
            using pointer_type = std::shared_ptr<json_value>;
            using container_type = std::vector<pointer_type>;

        public:
            /*
             * Public Types
             */
            using size_type = container_type::size_type;



            /*
             * Constructor(s)/Destructor
             */
            json_array()
            {
            }

            json_array(const json_array &other)
            {
                this->copy(other);
            }

            json_array(json_array &&) = default;

            json_array(const container_type &array)
            {
                this->copy(array);
            }

            json_array(container_type &&array) :
                _array(std::move(array))
            {
            }



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::array;
            }



            /*
             * Operators
             */
            json_array &operator=(const json_array &other)
            {
                if (&other != this)
                {
                    this->copy(other);
                }

                return *this;
            }

            json_array &operator=(json_array &&) = default;

            const pointer_type &operator[](size_type index)
            {
                return this->_array[index];
            }



            /*
             * Functions
             */
            const container_type &array() const noexcept
            {
                return this->_array;
            }

            void clear()
            {
                this->_array.clear();
            }

            bool empty() const noexcept
            {
                return this->_array.empty();
            }

            size_type size() const noexcept
            {
                return this->_array.size();
            }

            void swap(json_array &other)
            {
                this->_array.swap(other._array);
            }



        private:

            void copy(const json_array &other)
            {
                this->copy(other._array);
            }

            void copy(const container_type &array)
            {
                this->_array.clear();

                for (auto &value : array)
                {
                    this->_array.emplace_back(details::copy_json_value(value));
                }
            }

            container_type _array;
        };



        /*
         * json_number
         */
        class json_number final :
            public json_value
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            json_number(utf8_string str) :
                _str(std::move(str))
            {
            }

            // Default copy/move
            json_number(const json_number &) = default;
            json_number(json_number &&) = default;

            json_number &operator=(const json_number &) = default;
            json_number &operator=(json_number &&) = default;



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::number;
            }



            /*
             * Functions
             */
            const utf8_string &str() const noexcept
            {
                return this->_str;
            }

            void swap(json_number &other)
            {
                this->_str.swap(other._str);
            }



        private:

            utf8_string _str;
        };



        /*
         * json_string
         */
        class json_string final :
            public json_value
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            json_string(utf8_string str) :
                _str(std::move(str))
            {
            }

            // Default copy/move
            json_string(const json_string &) = default;
            json_string(json_string &&) = default;

            json_string &operator=(const json_string &) = default;
            json_string &operator=(json_string &&) = default;



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::string;
            }



            /*
             * Functions
             */
            const utf8_string &str() const noexcept
            {
                return this->_str;
            }

            void swap(json_string &other)
            {
                this->_str.swap(other._str);
            }



        private:

            utf8_string _str;
        };



        /*
         * json_boolean
         */
        class json_boolean final :
            public json_value
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            json_boolean(bool value) :
                _value(value)
            {
            }

            // Default copy/move
            json_boolean(const json_boolean &) = default;
            json_boolean(json_boolean &&) = default;

            json_boolean &operator=(const json_boolean &) = default;
            json_boolean &operator=(json_boolean &&) = default;



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::boolean;
            }



            /*
             * Functions
             */
            bool value() const noexcept
            {
                return this->_value;
            }

            void swap(json_boolean &other)
            {
                this->_value = other._value;
            }



        private:

            bool _value;
        };



        /*
         * json_null
         */
        class json_null final :
            public json_value
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            json_null()
            {
            }

            // Default copy/move
            json_null(const json_null &) = default;
            json_null(json_null &&) = default;

            json_null &operator=(const json_null &) = default;
            json_null &operator=(json_null &&) = default;



            /*
             * json_value
             */
            virtual json_type type(void) const noexcept override
            {
                return json_type::null;
            }



            /*
             * Functions
             */
            std::nullptr_t value() const noexcept
            {
                return nullptr;
            }

            void swap(json_boolean &)
            {
            }
        };



        namespace details
        {
            std::shared_ptr<json_value> copy_json_value(const std::shared_ptr<json_value> &other)
            {
                auto type = other->type();

                switch (type)
                {
                case json_type::object:
                    return std::make_shared<json_object>(*other->as<json_object>());
                    break;

                case json_type::array:
                    return std::make_shared<json_array>(*other->as<json_array>());
                    break;

                case json_type::number:
                    return std::make_shared<json_number>(*other->as<json_number>());
                    break;

                case json_type::string:
                    return std::make_shared<json_string>(*other->as<json_string>());
                    break;

                case json_type::boolean:
                    return std::make_shared<json_boolean>(*other->as<json_boolean>());
                    break;

                case json_type::null:
                    return std::make_shared<json_null>(*other->as<json_null>());
                    break;

                default:
                    assert(false && "Unexpected json_type");
                    break;
                }

                return nullptr;
            }
        }

#pragma endregion



        /*
         * json_cast and make_json declarations
         */
#pragma region Conversion Function/Type Declarations

        /*
         * json_cast
         */
        template <typename Ty, typename /*enable*/ = void>
        struct json_cast_t;

        template <typename Ty, typename CastTy = json_cast_t<Ty>>
        inline Ty json_cast(const json_value *value)
        {
            return CastTy()(value);
        }

        template <typename Ty, typename CastTy>
        inline Ty json_cast(const json_value *value, const CastTy &cast)
        {
            return cast(value);
        }



        /*
         * make_json
         */
        template <typename Ty, typename /*enable*/ = void>
        struct make_json_t;

        template <typename Ty, typename MakeTy = make_json_t<Ty>>
        inline std::shared_ptr<json_value> make_json(const Ty &value)
        {
            return MakeTy()(value);
        }

        template <typename Ty, typename MakeTy>
        inline std::shared_ptr<json_value> make_json(const Ty &value, const MakeTy &make)
        {
            return make(value);
        }

#pragma endregion



        /*
         * Arithmetic casting
         */
#pragma region Integer Casting

        template <typename Ty>
        struct json_cast_t<Ty, std::enable_if_t<std::is_arithmetic<Ty>::value>>
        {
            using value_type = Ty;

            inline value_type operator()(const json_value *value) const
            {
                return numeric_cast<Ty>(value->as<json_number>()->str());
            }
        };

        template <typename Ty>
        struct make_json_t<Ty, std::enable_if_t<std::is_arithmetic<Ty>::value>>
        {
            inline std::shared_ptr<json_value> operator()(const Ty &value) const
            {
                // TODO: Are string streams really the best option here? Also, should we attempt to do scientific notation?
                std::stringstream stream;
                stream << value;
                return std::make_shared<json_number>(stream.str().c_str());
            }
        };

#pragma endregion



        /*
         * std::basic_string (currently don't support std::wstring)
         */
#pragma region basic_string Casting

        template <typename CharT>
        struct json_cast_t<std::basic_string<CharT>>
        {
            using value_type = std::basic_string<CharT>;

            inline value_type operator()(const json_value *value) const
            {
                auto jsonString = value->as<json_string>();
                utf_string<CharT> utfString = jsonString->str();
                return utfString.c_str();
            }
        };

        template <typename CharT>
        struct make_json_t<std::basic_string<CharT>>
        {
            inline std::shared_ptr<json_value> operator()(const std::basic_string<CharT> &value) const
            {
                return std::make_shared<json_string>(value);
            }
        };

#pragma endregion



        /*
         * dhorn::utf_string
         */
#pragma region utf_string Casting

        template <typename CharT>
        struct json_cast_t<utf_string<CharT>>
        {
            using value_type = utf_string<CharT>;

            inline value_type operator()(const json_value *value) const
            {
                auto jsonString = value->as<json_string>();
                return jsonString->str();
            }
        };

        template <typename CharT>
        struct make_json_t<utf_string<CharT>>
        {
            inline std::shared_ptr<json_value> operator()(const utf_string<CharT> &value) const
            {
                return std::make_shared<json_string>(value);
            }
        };

#pragma endregion



        /*
         * Container base-classes
         */
#pragma region Base classes for container json_cast_t and make_json_t

        namespace details
        {
            template <typename Ty>
            struct emplace_back_json_cast_t
            {
                using value_type = Ty;

                inline value_type operator()(const json_value *value) const
                {
                    auto jsonArray = value->as<json_array>();
                    auto &array = jsonArray->array();

                    value_type result;
                    for (auto &obj : array)
                    {
                        result.emplace_back(json_cast<typename value_type::value_type>(obj.get()));
                    }

                    return result;
                }
            };

            template <typename Ty>
            struct emplace_after_json_cast_t
            {
                using value_type = Ty;

                inline value_type operator()(const json_value *value) const
                {
                    auto jsonArray = value->as<json_array>();
                    auto &array = jsonArray->array();

                    value_type result;
                    auto itr = result.before_begin();
                    for (auto &obj : array)
                    {
                        itr = result.emplace_after(itr, json_cast<typename value_type::value_type>(obj.get()));
                    }

                    return result;
                }
            };

            template <typename Ty>
            struct emplace_json_cast_t
            {
                using value_type = Ty;

                inline value_type operator()(const json_value *value) const
                {
                    auto jsonArray = value->as<json_array>();
                    auto &array = jsonArray->array();

                    value_type result;
                    for (auto &obj : array)
                    {
                        result.emplace(json_cast<typename value_type::value_type>(obj.get()));
                    }

                    return result;
                }
            };

            template <typename Ty>
            struct random_access_json_cast_t
            {
                using value_type = Ty;

                inline value_type operator()(const json_value *value) const
                {
                    value_type result;
                    constexpr std::size_t size = result.size();

                    auto jsonArray = value->as<json_array>();
                    auto &array = jsonArray->array();
                    if (array.size() != size)
                    {
                        throw json_exception("Unexpected array size");
                    }

                    for (std::size_t i = 0; i < size; ++i)
                    {
                        result[i] = json_cast<typename value_type::value_type>(array[i].get());
                    }

                    return result;
                }
            };

            template <typename Ty>
            struct sequence_container_make_json_t
            {
                inline std::shared_ptr<json_value> operator()(const Ty &value) const
                {
                    std::vector<std::shared_ptr<json_value>> array;

                    for (auto &val : value)
                    {
                        array.emplace_back(make_json(val));
                    }

                    return std::make_shared<json_array>(std::move(array));
                }
            };
        }

#pragma endregion



        /*
         * std::vector
         */
#pragma region std::vector Casting

        template <typename Ty, typename Alloc>
        struct json_cast_t<std::vector<Ty, Alloc>> :
            public details::emplace_back_json_cast_t<std::vector<Ty, Alloc>>
        {
        };

        template <typename Ty, typename Alloc>
        struct make_json_t<std::vector<Ty, Alloc>> :
            public details::sequence_container_make_json_t<std::vector<Ty, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::deque
         */
#pragma region std::deque Casting

        template <typename Ty, typename Alloc>
        struct json_cast_t<std::deque<Ty, Alloc>> :
            public details::emplace_back_json_cast_t<std::deque<Ty, Alloc>>
        {
        };

        template <typename Ty, typename Alloc>
        struct make_json_t<std::deque<Ty, Alloc>> :
            public details::sequence_container_make_json_t<std::deque<Ty, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::list
         */
#pragma region std::list Casting

        template <typename Ty, typename Alloc>
        struct json_cast_t<std::list<Ty, Alloc>> :
            public details::emplace_back_json_cast_t<std::list<Ty, Alloc>>
        {
        };

        template <typename Ty, typename Alloc>
        struct make_json_t<std::list<Ty, Alloc>> :
            public details::sequence_container_make_json_t<std::list<Ty, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::forward_list
         */
#pragma region std::forward_list Casting

        template <typename Ty, typename Alloc>
        struct json_cast_t<std::forward_list<Ty, Alloc>> :
            public details::emplace_after_json_cast_t<std::forward_list<Ty, Alloc>>
        {
        };

        template <typename Ty, typename Alloc>
        struct make_json_t<std::forward_list<Ty, Alloc>> :
            public details::sequence_container_make_json_t<std::forward_list<Ty, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::array
         */
#pragma region std::array Casting

        template <typename Ty, std::size_t Size>
        struct json_cast_t<std::array<Ty, Size>> :
            public details::random_access_json_cast_t<std::array<Ty, Size>>
        {
        };

        template <typename Ty, std::size_t Size>
        struct make_json_t<std::array<Ty, Size>> :
            public details::sequence_container_make_json_t<std::array<Ty, Size>>
        {
        };

#pragma endregion



        /*
         * std::set
         */
#pragma region std::set Casting

        template <typename Key, typename Comp, typename Alloc>
        struct json_cast_t<std::set<Key, Comp, Alloc>> :
            public details::emplace_json_cast_t<std::set<Key, Comp, Alloc>>
        {
        };

        template <typename Key, typename Comp, typename Alloc>
        struct make_json_t<std::set<Key, Comp, Alloc>> :
            public details::sequence_container_make_json_t<std::set<Key, Comp, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::multiset
         */
#pragma region std::multiset Casting

        template <typename Key, typename Comp, typename Alloc>
        struct json_cast_t<std::multiset<Key, Comp, Alloc>> :
            public details::emplace_json_cast_t<std::multiset<Key, Comp, Alloc>>
        {
        };

        template <typename Key, typename Comp, typename Alloc>
        struct make_json_t<std::multiset<Key, Comp, Alloc>> :
            public details::sequence_container_make_json_t<std::multiset<Key, Comp, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::unordered_set
         */
#pragma region std::unordered_set Casting

        template <typename Key, typename Hash, typename Equals, typename Alloc>
        struct json_cast_t<std::unordered_set<Key, Hash, Equals, Alloc>> :
            public details::emplace_json_cast_t<std::unordered_set<Key, Hash, Equals, Alloc>>
        {
        };

        template <typename Key, typename Hash, typename Equals, typename Alloc>
        struct make_json_t<std::unordered_set<Key, Hash, Equals, Alloc>> :
            public details::sequence_container_make_json_t<std::unordered_set<Key, Hash, Equals, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::unordered_multiset
         */
#pragma region std::unordered_multiset Casting

        template <typename Key, typename Hash, typename Equals, typename Alloc>
        struct json_cast_t<std::unordered_multiset<Key, Hash, Equals, Alloc>> :
            public details::emplace_json_cast_t<std::unordered_multiset<Key, Hash, Equals, Alloc>>
        {
        };

        template <typename Key, typename Hash, typename Equals, typename Alloc>
        struct make_json_t<std::unordered_multiset<Key, Hash, Equals, Alloc>> :
            public details::sequence_container_make_json_t<std::unordered_multiset<Key, Hash, Equals, Alloc>>
        {
        };

#pragma endregion



        /*
         * std::stack
         */
#pragma region std::stack Casting

        // We follow the pattern of the rest of the container types and assume that the first item in the array is at the
        // "head," which for stacks, is the first item inserted and thus the last item to get pop-ed
        template <typename Ty, typename Container>
        struct json_cast_t<std::stack<Ty, Container>> :
            public details::emplace_json_cast_t<std::stack<Ty, Container>>
        {
        };

        // We purposefully don't provide an overload of make_json for std::stack since it is not iterable, and the only way
        // of iterating it is to remove all elements

#pragma endregion



        /*
         * std::queue
         */
#pragma region std::queue Casting

         // We follow the pattern of the rest of the container types and assume that the first item in the array is at the
         // "head," which for queues, is the first item inserted and thus the first item to get pop-ed
        template <typename Ty, typename Container>
        struct json_cast_t<std::queue<Ty, Container>> :
            public details::emplace_json_cast_t<std::queue<Ty, Container>>
        {
        };

        // We purposefully don't provide an overload of make_json for std::queue since it is not iterable, and the only way
        // of iterating it is to remove all elements

#pragma endregion



        /*
         * std::priority_queue
         */
#pragma region std::priority_queue Casting

        template <typename Ty, typename Container, typename Compare>
        struct json_cast_t<std::priority_queue<Ty, Container, Compare>> :
            public details::emplace_json_cast_t<std::priority_queue<Ty, Container, Compare>>
        {
        };

        // We purposefully don't provide an overload of make_json for std::priority_queue since it is not iterable, and the
        // only way of iterating it is to remove all elements

#pragma endregion



        /*
         * std::map
         */
#pragma region std::map Casting

        template <typename KeyTy, typename ValueTy, typename Comp, typename Alloc>
        struct json_cast_t<std::map<KeyTy, ValueTy, Comp, Alloc>>
        {
            using value_type = std::map<KeyTy, ValueTy, Comp, Alloc>;

            inline value_type operator()(const json_value *value) const
            {
                value_type result;

                // Check the type of the input
                auto jsonArray = dynamic_cast<const json_array *>(value);
                if (jsonArray)
                {
                    auto &array = jsonArray->array();
                    for (auto &pairObj : array)
                    {
                        auto pairArray = pairObj->as<json_array>();
                        auto &pairVector = pairArray->array();
                        if (pairVector.size() != 2)
                        {
                            throw json_exception("Expected only two values in key/value array");
                        }

                        auto key = json_cast<KeyTy>(pairVector[0].get());
                        auto itr = result.find(key);
                        if (itr != result.end())
                        {
                            throw json_exception("Keys are not unique");
                        }

                        result.emplace(
                            std::move(key),
                            json_cast<ValueTy>(pairVector[1].get()));
                    }
                }
                else
                {
                    auto jsonObject = value->as<json_object>();
                    for (auto &pair : *jsonObject)
                    {
                        // json_object guarantees unique-ness in key values
                        // TODO
                        (void)pair;
                        //result.emplace(
                        //    pair.first.c_str(),
                        //    json_cast<ValueTy>(pair.second.get()));
                    }
                }

                return result;
            }
        };

#pragma endregion
    }
}
