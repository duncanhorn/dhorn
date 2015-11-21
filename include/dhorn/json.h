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
 * json_cast<std::vector<...>> requires that the json_value be a json_array, json_cast<std::map<...>> requires that the
 * json_value be a json_array of json_objects that represent pairs, etc.).
 *
 * Similarly, C++ objects can be converted to json values using the make_json function that can be overloaded for any
 * user-defined type:
 *
 *      foo myFoo = get_foo();
 *      std::shared_ptr<json_value> value = make_json(myFoo);
 *
 * And of course, the make_json function is already overloaded for a set of standard types. In general, these do the
 * reverse of what json_cast does for these types.
 */
#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "utf_string.h"

namespace dhorn
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



    namespace garbage
    {
        std::shared_ptr<json_value> copy_json_value(_In_ const std::shared_ptr<json_value> &other);
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



        /*
         * Constructor(s)/Destructor
         */
        json_object()
        {
        }

        json_object(_In_ const json_object &other)
        {
            this->copy(other);
        }

        json_object(_Inout_ json_object &&) = default;



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
        json_object &operator=(_In_ const json_object &other)
        {
            if (&other != this)
            {
                this->copy(other);
            }

            return *this;
        }

        json_object &operator=(_Inout_ json_object &&) = default;

        std::shared_ptr<json_value> operator[](_In_ const utf8_string &str)
        {
            auto itr = this->_tree.find(str);

            if (itr != std::end(this->_tree))
            {
                return itr->second;
            }

            // Does not exist
            return nullptr;
        }



        /*
         * Functions
         */
        void clear()
        {
            this->_tree.clear();
        }

        bool empty() const noexcept
        {
            return this->_tree.empty();
        }

        size_type size() const noexcept
        {
            return this->_tree.size();
        }

        void swap(_Inout_ json_object &other)
        {
            this->_tree.swap(other._tree);
        }



    private:

        void copy(_In_ const json_object &other)
        {
            this->_tree.clear();

            for (auto &pair : other._tree)
            {
                this->_tree.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(pair.first),
                    std::forward_as_tuple(garbage::copy_json_value(pair.second)));
            }
        }

        container_type _tree;
    };



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

        json_array(_In_ const json_array &other)
        {
            this->copy(other);
        }

        json_array(_Inout_ json_array &&) = default;



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
        json_array &operator=(_In_ const json_array &other)
        {
            if (&other != this)
            {
                this->copy(other);
            }

            return *this;
        }

        json_array &operator=(_Inout_ json_array &&) = default;

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

        void swap(_Inout_ json_array &other)
        {
            this->_array.swap(other._array);
        }



    private:

        void copy(_In_ const json_array &other)
        {
            this->_array.clear();

            for (auto &value : other._array)
            {
                this->_array.emplace_back(garbage::copy_json_value(value));
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
        json_number(_In_ utf8_string str) :
            _str(std::move(str))
        {
        }

        // Default copy/move
        json_number(const json_number &) = default;
        json_number(_Inout_ json_number &&) = default;

        json_number &operator=(_In_ const json_number &) = default;
        json_number &operator=(_Inout_ json_number &&) = default;



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

        void swap(_Inout_ json_number &other)
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
        json_string(_In_ utf8_string str) :
            _str(std::move(str))
        {
        }

        // Default copy/move
        json_string(const json_string &) = default;
        json_string(_Inout_ json_string &&) = default;

        json_string &operator=(_In_ const json_string &) = default;
        json_string &operator=(_Inout_ json_string &&) = default;



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

        void swap(_Inout_ json_string &other)
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
        json_boolean(_In_ bool value) :
            _value(value)
        {
        }

        // Default copy/move
        json_boolean(const json_boolean &) = default;
        json_boolean(_Inout_ json_boolean &&) = default;

        json_boolean &operator=(_In_ const json_boolean &) = default;
        json_boolean &operator=(_Inout_ json_boolean &&) = default;



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

        void swap(_Inout_ json_boolean &other)
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
        json_null(_Inout_ json_null &&) = default;

        json_null &operator=(_In_ const json_null &) = default;
        json_null &operator=(_Inout_ json_null &&) = default;



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

        void swap(_Inout_ json_boolean &)
        {
        }
    };



    namespace garbage
    {
        std::shared_ptr<json_value> copy_json_value(_In_ const std::shared_ptr<json_value> &other)
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
    template <typename Ty>
    struct json_cast_t
    {
        inline Ty operator()(_In_ const json_value *value);
    };

    template <typename Ty, typename CastTy = json_cast_t<Ty>>
    inline Ty json_cast(_In_ const json_value *value)
    {
        return CastTy()(value);
    }

    template <typename Ty, typename CastTy>
    inline Ty json_cast(_In_ const json_value *value, _In_ const CastTy &cast)
    {
        return cast(value);
    }



    /*
     * make_json
     */
    template <typename Ty>
    struct make_json_t
    {
        inline std::shared_ptr<json_value> make_json(_In_ const Ty &);
    };

    template <typename Ty, typename MakeTy = make_json_t<Ty>>
    inline std::shared_ptr<json_value> make_json(_In_ const Ty &value)
    {
        return MakeTy()(value);
    }

    template <typename Ty, typename MakeTy>
    inline std::shared_ptr<json_value> make_json(_In_ const Ty &value, _In_ const MakeTy &make)
    {
        return make(value);
    }

#pragma endregion



    /*
     * std::basic_string (currently don't support std::wstring)
     */
#pragma region

    template <typename CharT>
    struct json_cast_t<std::basic_string<CharT>>
    {
        inline std::basic_string<CharT> operator()(_In_ const json_value *value)
        {
            auto jsonString = value->as<json_string>();
            utf_string<CharT> utfString = jsonString->str();
            return utfString.c_str();
        }
    };

    template <typename CharT>
    struct make_json_t<std::basic_string<CharT>>
    {
        inline std::shared_ptr<json_value> operator()(_In_ const std::basic_string<CharT> &value)
        {
            utf_string<CharT> utfString = value.c_str();
            return std::make_shared<json_string>(utfString);
        }
    };

#pragma endregion
}
