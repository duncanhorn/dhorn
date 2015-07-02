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

namespace dhorn
{
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
    }



    /*
     * Unit types
     */
#pragma region Unit Types

    template <typename Ty>
    struct length
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct area
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 2>;
    };

    template <typename Ty>
    struct volume
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 3>;
    };

    template <typename Ty>
    struct mass
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct time
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct current
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct temperature
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct quantity // TODO: rename? Example unit is mole
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

    template <typename Ty>
    struct luminosity
    {
        using value_type = Ty;
        using convert_type = garbage::power<Ty, 1>;
    };

#pragma endregion



    /*
     * Unit type
     */
#pragma region Unit Type

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
    }



    template <typename Unit, typename Ratio>
    class unit final
    {
        static_assert(garbage::is_ratio<Ratio>::value, "Second template argument must be of type std::ratio");

    public:
        /*
         * Public types
         */
        using value_type = typename Unit::value_type;



        /*
         * Constructor(s)/Destructor
         */
        unit(void) :
            _value{}
        {
        }

        // Default copy/move
        unit(_In_ const unit &) = default;
        unit(_Inout_ unit &&) = default;
        unit &operator=(_In_ const unit &) = default;
        unit &operator=(_Inout_ unit &&) = default;

        unit(_In_ value_type val) :
            _value(std::move(val))
        {
        }



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

#pragma endregion
}
