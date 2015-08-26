/*
 * Duncan Horn
 *
 * units.h
 *
 * 
 */
#pragma once

#include <limits>
#include <ratio>
#include <type_traits>

namespace dhorn
{
    /*
     * Unit types
     */
    enum class unit_type
    {
        length      = 1,
        area        = 2,
        volume      = 3,
        mass        = 4,
        time        = 5,
        current     = 6,
        temperature = 7,
        quantity    = 8,
        luminosity  = 9,
    };



    /*
     * Conversion helpers (squared, cubed, etc.)
     */
    namespace garbage
    {
        template <typename Ty, size_t Power>
        struct power
        {
            static_assert(Power != 0, "Power of zero is not supported");

            inline constexpr Ty operator()(_In_ const Ty &val)
            {
                return val * ((Power == 1) ? 1 : power<Power - 1>(val));
            }
        };



        template <unit_type Type, typename Ty>
        struct unit_traits;

        template <typename Ty>
        struct unit_traits<unit_type::length, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::area, Ty>
        {
            using convert_type = power<Ty, 2>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::volume, Ty>
        {
            using convert_type = power<Ty, 3>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::mass, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::time, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::current, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::temperature, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::quantity, Ty>
        {
            using convert_type = power<Ty, 1>;
        };

        template <typename Ty>
        struct unit_traits<unit_type::luminosity, Ty>
        {
            using convert_type = power<Ty, 1>;
        };
    }



    /*
     * Unit type
     */
#pragma region Unit Type

    template <unit_type Unit, typename Ty, typename Ratio>
    class unit;

    // Helpers
    namespace garbage
    {
        template <typename Ty>
        struct is_ratio : std::false_type
        {
        };

        template <intmax_t Num, intmax_t Den>
        struct is_ratio<std::ratio<Num, Den>> : std::true_type
        {
        };



        template <typename Lhs, typename Rhs>
        struct unit_result_type;

        template <unit_type Unit, typename Ty, typename Ratio, typename Rhs>
        struct unit_result_type<unit<Unit, Ty, Ratio>, Rhs>
        {
            using type = unit<Unit, typename std::common_type<Ty, Rhs>::type, Ratio>;
        };
    }



    template <unit_type Unit, typename Ty, typename Ratio>
    class unit final
    {
        static_assert(garbage::is_ratio<Ratio>::value, "Second template argument must be of type std::ratio");

        /*
         * Private types
         */
        using Traits = garbage::unit_traits<Unit, Ty>;



    public:
        /*
         * Public types
         */
        using value_type = typename Ty;
        static const unit_type type = Unit;



        /*
         * Constructor(s)/Destructor
         */
        unit(void) :
            _value{}
        {
        }

        unit(_In_ value_type val) :
            _value(std::move(val))
        {
        }

        // Default copy/move
        unit(_In_ const unit &) = default;
        unit(_Inout_ unit &&) = default;
        unit &operator=(_In_ const unit &) = default;
        unit &operator=(_Inout_ unit &&) = default;



        /*
         * Operators
         */
        unit &operator=(_In_ const value_type &val)
        {
            this->_value = val;
            return *this;
        }

        // Sign
        unit operator+(void) const
        {
            return *this;
        }

        unit operator-(void) const
        {
            return unit(-this->_value);
        }

        // Addition
        unit &operator++(void)
        {
            ++this->_value;
            return *this;
        }

        unit operator++(int /*unused*/)
        {
            return unit(this->_value++);
        }

        unit &operator+=(_In_ const unit &rhs)
        {
            this->_value += rhs._value;
            return *this;
        }

        unit &operator+=(_In_ const value_type &val)
        {
            this->_value += val;
            return *this;
        }

        // Subtraction
        unit &operator--(void)
        {
            --this->_value;
            return *this;
        }

        unit operator--(int /*unused*/)
        {
            return unit(this->_value--);
        }

        unit &operator-=(_In_ const unit &rhs)
        {
            this->_value -= rhs._value;
            return *this;
        }

        unit &operator-=(_In_ const value_type &val)
        {
            this->_value -= val;
            return *this;
        }

        // Multiplication
        unit &operator*=(_In_ const value_type &val)
        {
            this->_value *= val;
            return *this;
        }

        // Division/modulous
        unit &operator/=(_In_ const value_type &val)
        {
            this->_value /= val;
            return *this;
        }

        unit &operator%=(_In_ const unit &rhs)
        {
            this->_value %= rhs._value;
            return *this;
        }

        unit &operator%=(_In_ const value_type &val)
        {
            this->_value %= val;
            return *this;
        }



        /*
         * Public functions
         */
        value_type value(void) const
        {
            return this->_value;
        }

        static constexpr value_type zero(void)
        {
            return value_type{};
        }

        static constexpr value_type min(void)
        {
            return std::numeric_limits<value_type>::lowest();
        }

        static constexpr value_type max(void)
        {
            return std::numeric_limits<value_type>::max();
        }



    private:

        value_type _value;
    };



    /*
     * Operators
     */

    // Addition
    //template <
    //    unit_type Unit,
    //    typename Ty1, typename Ratio1,
    //    typename Ty2, typename Ratio2>
    //unit<Unit, Ratio> operator+(_In_ const unit<Unit, Ratio> &lhs, _In_ const unit<Unit, Ratio> &rhs)
    //{
    //    return unit<Unit, Ratio>(lhs) += rhs;
    //}

    //template <typename Unit, typename Ratio, typename Ty>
    //unit<Unit, Ratio> operator+(_In_ const unit<Unit, Ratio> &lhs, _In_ Ty rhs)
    //{
    //    return unit<Unit, Ratio>(lhs) += rhs;
    //}

#pragma endregion
}
