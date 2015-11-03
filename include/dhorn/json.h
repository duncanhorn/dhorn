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



 
 
 The base type is json_object which, at its heart, is just a tree. Iterating over a json_object will
 * give key/value pairs of the type std::pair<dhorn::utf8_string, dhorn::json_object>.
 *
 * json_object instances can be converted to C++ objects using the json_cast function where Ty is the desired C++ type.
 *
 *      json_object obj = get_object();
 *      auto myFoo = json_cast<foo>(obj);
 *
 * Since a json_object need not represent another object (e.g. it could be an array
 *
 * Similarly, C++ objects can be converted to json objects using the make_json function that can be overloaded for any
 * user-defined type. Note that several STL types (e.g. std::vector, etc.) already have.
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



    namespace garbage
    {

    }



    /*
     * json_type
     */
    enum class json_type
    {
        object  = 0,
        array   = 1,
        number  = 2,
        string  = 3,
        boolean = 4,
        null    = 5,
    };



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
        using size_type = typename container_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        json_object()
        {
        }

        json_object(_In_ const json_object &other)
        {
            // TODO
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
                // TODO
            }

            return *this;
        }

        json_object &operator=(_Inout_ json_object &&) = default;



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

        void swap(_Inout_ json_object& other)
        {
            this->_tree.swap(other._tree);
        }



    private:

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
        using size_type = typename container_type::size_type;



        /*
         * Constructor(s)/Destructor
         */
        json_array()
        {
        }

        json_array(_In_ const json_array &other)
        {
            // TODO
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
                // TODO
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



    private:

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
        json_number(_In_ const json_number &) = default;
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
        json_string(_In_ const json_string &) = default;
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
}
