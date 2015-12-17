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
     * Conversion helpers (squared, cubed, etc.)
     */
    namespace garbage
    {
        template <typename Ty>
        struct unit_type_base
        {
            using type = Ty;
        };
    }



    /*
     * Unit type
     */
#pragma region Unit Type

    template <typename UnitType, typename Ratio>
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



        template <typename Ty>
        struct unit_traits
        {
            static const bool is_unit = false;
        };

        template <typename UnitType, typename Ratio>
        struct unit_traits<unit<UnitType, Ratio>>
        {
            static const bool is_unit = true;
            using unit_type = UnitType;
            using ratio_type = Ratio;
        };



        template <
            typename From,
            typename To,
            typename Ty,
            typename = std::enable_if<is_ratio<From>::value>::type,
            typename = std::enable_if<is_ratio<To>::value>::type>
        inline constexpr Ty ratio_convert(_In_ const Ty &val)
        {
            using convert_ratio = std::ratio_divide<From, To>;
            return
                ((convert_ratio::num == 1) && (convert_ratio::den == 1))
                ? val
                : (
                    (convert_ratio::den == 1)
                    ? (val * convert_ratio::num)
                    : (
                        (convert_ratio::num == 1)
                        ? (val / convert_ratio::den)
                        : (val * convert_ratio::num / convert_ratio::den)
                      )
                  );
        }
    }



    /*
     * Unit types
     */
#pragma region Unit Types

    namespace unit_type
    {
        template <typename Ty>
        struct length : public garbage::unit_type_base<Ty> {};

        template <typename Ty>
        struct area : public garbage::unit_type_base<Ty> {};

        template <typename Ty>
        struct volume : public garbage::unit_type_base<Ty> {};

        template <typename Ty>
        struct mass : public garbage::unit_type_base<Ty> {};

        template <typename Ty>
        struct time : public garbage::unit_type_base<Ty> {};

        template <typename Ty>
        struct current : public garbage::unit_type_base<Ty> {};
    }

#pragma endregion



    template <typename UnitType, typename Ratio>
    class unit final
    {
        static_assert(garbage::is_ratio<Ratio>::value, "Second template argument must be of type std::ratio");

    public:
        /*
         * Public types
         */
        using value_type = typename UnitType::type;



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

        template <typename OtherRatio>
        unit(_In_ const unit<UnitType, OtherRatio> &other) :
            _value(garbage::ratio_convert<OtherRatio, Ratio>(other.value()))
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

        template <typename OtherRatio>
        unit &operator=(_In_ unit<UnitType, OtherRatio> &other)
        {
            this->_value = garbage::ratio_convert<OtherRatio, Ratio>(other.value());
            return *this;
        }

        // Unary operators
        unit operator+(void) const
        {
            return *this;
        }

        unit operator-(void) const
        {
            return unit(-this->_value);
        }

        unit &operator++(void)
        {
            ++this->_value;
            return *this;
        }

        unit operator++(int /*unused*/)
        {
            return unit(this->_value++);
        }

        unit &operator--(void)
        {
            --this->_value;
            return *this;
        }

        unit operator--(int /*unused*/)
        {
            return unit(this->_value--);
        }

        // Multiplication/Division/Modulous
        unit &operator*=(_In_ const value_type &val)
        {
            this->_value *= val;
            return *this;
        }

        unit &operator/=(_In_ const value_type &val)
        {
            this->_value /= val;
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
     * Value Multiplication/Division/Modulous
     */
    template <typename UnitType, typename Ratio, typename ValueType>
    inline unit<UnitType, Ratio> operator*(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        return unit<UnitType, Ratio>(lhs.value() * rhs);
    }

    template <typename ValueType, typename UnitType, typename Ratio>
    inline unit<UnitType, Ratio> operator*(_In_ const ValueType &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return unit<UnitType, Ratio>(lhs * rhs.value());
    }

    template <typename UnitType, typename Ratio, typename ValueType>
    inline unit<UnitType, Ratio> operator/(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        // Note that it does not make sense to divide a value by a unit; only a unit by a value
        return unit<UnitType, Ratio>(lhs.value() / rhs);
    }

    template <typename UnitType, typename Ratio, typename ValueType>
    inline unit<UnitType, Ratio> operator%(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        // Note that it does not make sense to divide a value by a unit; only a unit by a value
        return unit<UnitType, Ratio>(lhs.value() % rhs);
    }



    /*
     * Operators
     */
    template <typename UnitType, typename Ratio, typename ValueType>
    inline bool operator==(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        return lhs.value() == rhs;
    }

    template <typename ValueType, typename UnitType, typename Ratio>
    inline bool operator==(_In_ const ValueType &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return lhs == rhs.value();
    }

    template <typename UnitType, typename Ratio>
    inline bool operator==(_In_ const unit<UnitType, Ratio> &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return lhs.value() == rhs.value();
    }

    template <typename UnitType, typename Ratio, typename ValueType>
    inline bool operator!=(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        return lhs.value() != rhs;
    }

    template <typename ValueType, typename UnitType, typename Ratio>
    inline bool operator!=(_In_ const ValueType &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return lhs != rhs.value();
    }

    template <typename UnitType, typename Ratio>
    inline bool operator!=(_In_ const unit<UnitType, Ratio> &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return lhs.value() != rhs.value();
    }

    template <typename UnitType, typename Ratio, typename ValueType>
    inline unit<UnitType, Ratio> operator+(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        return unit<UnitType, Ratio>(lhs.value() + rhs);
    }

    template <typename ValueType, typename UnitType, typename Ratio>
    inline unit<UnitType, Ratio> operator+(_In_ const ValueType &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return unit<UnitType, Ratio>(lhs + rhs.value());
    }

    template <typename UnitType, typename Ratio>
    inline unit<UnitType, Ratio> operator+(
        _In_ const unit<UnitType, Ratio> &lhs,
        _In_ const unit<UnitType, Ratio> &rhs)
    {
        return unit<UnitType, Ratio>(lhs.value() + rhs.value());
    }

    template <typename UnitType, typename Ratio, typename ValueType>
    inline unit<UnitType, Ratio> operator-(_In_ const unit<UnitType, Ratio> &lhs, _In_ const ValueType &rhs)
    {
        return unit<UnitType, Ratio>(lhs.value() - rhs);
    }

    template <typename ValueType, typename UnitType, typename Ratio>
    inline unit<UnitType, Ratio> operator-(_In_ const ValueType &lhs, _In_ const unit<UnitType, Ratio> &rhs)
    {
        return unit<UnitType, Ratio>(lhs - rhs.value());
    }

    template <typename UnitType, typename Ratio>
    inline unit<UnitType, Ratio> operator-(
        _In_ const unit<UnitType, Ratio> &lhs,
        _In_ const unit<UnitType, Ratio> &rhs)
    {
        return unit<UnitType, Ratio>(lhs.value() - rhs.value());
    }



    /*
     * unit_cast
     */
    template <
        typename TargetType,
        typename UnitType,
        typename Ratio,
        typename = typename std::enable_if<garbage::unit_traits<TargetType>::is_unit>::type,
        typename = typename std::enable_if<std::is_same<UnitType, typename garbage::unit_traits<TargetType>::unit_type>::value>::type>
    inline constexpr TargetType unit_cast(_In_ const unit<UnitType, Ratio> &val)
    {
        return TargetType(val);
    }

#pragma endregion



#pragma region Type Definitions

    // TODO: Can we get away with not using double...?

    // Length; base is meter
#pragma region Length

    // Metric
    using attometers        = unit<unit_type::length<intmax_t>, std::atto>;
    using femtometers       = unit<unit_type::length<intmax_t>, std::femto>;
    using picometers        = unit<unit_type::length<intmax_t>, std::pico>;
    using nanometers        = unit<unit_type::length<intmax_t>, std::nano>;
    using micrometers       = unit<unit_type::length<intmax_t>, std::micro>;
    using millimeters       = unit<unit_type::length<intmax_t>, std::milli>;
    using centimeters       = unit<unit_type::length<intmax_t>, std::centi>;
    using decimeters        = unit<unit_type::length<intmax_t>, std::deci>;
    using meters            = unit<unit_type::length<intmax_t>, std::ratio<1>>;
    using decameters        = unit<unit_type::length<intmax_t>, std::deca>;
    using hectometers       = unit<unit_type::length<intmax_t>, std::hecto>;
    using kilometers        = unit<unit_type::length<intmax_t>, std::kilo>;
    using megameters        = unit<unit_type::length<intmax_t>, std::mega>;
    using gigameters        = unit<unit_type::length<intmax_t>, std::giga>;
    using terameters        = unit<unit_type::length<intmax_t>, std::tera>;
    using petameters        = unit<unit_type::length<intmax_t>, std::peta>;
    using exameters         = unit<unit_type::length<intmax_t>, std::exa>;

    // Imperial
    using thous             = unit<unit_type::length<intmax_t>, std::ratio<254, 10000000>>;
    using inches            = unit<unit_type::length<intmax_t>, std::ratio<254, 10000>>;
    using feet              = unit<unit_type::length<intmax_t>, std::ratio<3048, 10000>>;
    using chains            = unit<unit_type::length<intmax_t>, std::ratio<201168, 10000>>;
    using furlongs          = unit<unit_type::length<intmax_t>, std::ratio<201168, 1000>>;
    using miles             = unit<unit_type::length<intmax_t>, std::ratio<1609344, 1000>>;
    using leagues           = unit<unit_type::length<intmax_t>, std::ratio<4828032, 1000>>;
    using fathoms           = unit<unit_type::length<intmax_t>, std::ratio<18288, 10000>>;
    using cables            = unit<unit_type::length<intmax_t>, std::ratio<1853184, 10000>>;
    using nautical_miles    = unit<unit_type::length<intmax_t>, std::ratio<1853184, 1000>>;
    using links             = unit<unit_type::length<intmax_t>, std::ratio<201168, 1000000>>;
    using rods              = unit<unit_type::length<intmax_t>, std::ratio<50292, 10000>>;

#pragma endregion



    // Area; base is square meters
#pragma region Area

    // TODO

#pragma endregion



    // Volume; base is meters cubed
#pragma region Volume

    // TODO

#pragma endregion



    // Mass; base is gram (TODO: Should this actually be kilogram?)
#pragma region Mass

    // Metric
    using attograms         = unit<unit_type::mass<intmax_t>, std::atto>;
    using femtograms        = unit<unit_type::mass<intmax_t>, std::femto>;
    using picograms         = unit<unit_type::mass<intmax_t>, std::pico>;
    using nanograms         = unit<unit_type::mass<intmax_t>, std::nano>;
    using micrograms        = unit<unit_type::mass<intmax_t>, std::micro>;
    using milligrams        = unit<unit_type::mass<intmax_t>, std::milli>;
    using centigrams        = unit<unit_type::mass<intmax_t>, std::centi>;
    using decigrams         = unit<unit_type::mass<intmax_t>, std::deci>;
    using grams             = unit<unit_type::mass<intmax_t>, std::ratio<1>>;
    using decagrams         = unit<unit_type::mass<intmax_t>, std::deca>;
    using hectograms        = unit<unit_type::mass<intmax_t>, std::hecto>;
    using kilograms         = unit<unit_type::mass<intmax_t>, std::kilo>;
    using megagrams         = unit<unit_type::mass<intmax_t>, std::mega>;
    using gigagrams         = unit<unit_type::mass<intmax_t>, std::giga>;
    using teragrams         = unit<unit_type::mass<intmax_t>, std::tera>;
    using petagrams         = unit<unit_type::mass<intmax_t>, std::peta>;
    using exagrams          = unit<unit_type::mass<intmax_t>, std::exa>;

    using metric_tonnes     = megagrams;

    // Imperial
    using grains            = unit<unit_type::mass<intmax_t>, std::ratio<6479891LL, 100000000LL>>;
    using drachms           = unit<unit_type::mass<intmax_t>, std::ratio<17718451953125LL, 10000000000000LL>>;
    using ounces            = unit<unit_type::mass<intmax_t>, std::ratio<28349523125LL, 1000000000LL>>;
    using pounds            = unit<unit_type::mass<intmax_t>, std::ratio<45359237LL, 100000LL>>;
    using stones            = unit<unit_type::mass<intmax_t>, std::ratio<635029318LL, 100000LL>>;
    using slugs             = unit<unit_type::mass<intmax_t>, std::ratio<1459390294LL, 100000LL>>;
    using quarters          = unit<unit_type::mass<intmax_t>, std::ratio<1270058636LL, 100000000000LL>>;
    using hundredweights    = unit<unit_type::mass<intmax_t>, std::ratio<5080234544LL, 100000000000LL>>;
    using short_tons        = unit<unit_type::mass<intmax_t>, std::ratio<90718474LL, 100000000LL>>;
    using long_tons         = unit<unit_type::mass<intmax_t>, std::ratio<10160469088LL, 10000000000LL>>;

#pragma endregion



    // Time; base is second
#pragma region Time

    using attoseconds       = unit<unit_type::time<intmax_t>, std::atto>;
    using femtoseconds      = unit<unit_type::time<intmax_t>, std::femto>;
    using picoseconds       = unit<unit_type::time<intmax_t>, std::pico>;
    using nanoseconds       = unit<unit_type::time<intmax_t>, std::nano>;
    using microseconds      = unit<unit_type::time<intmax_t>, std::micro>;
    using milliseconds      = unit<unit_type::time<intmax_t>, std::milli>;
    using centiseconds      = unit<unit_type::time<intmax_t>, std::centi>;
    using deciseconds       = unit<unit_type::time<intmax_t>, std::deci>;
    using seconds           = unit<unit_type::time<intmax_t>, std::ratio<1>>;
    using decaseconds       = unit<unit_type::time<intmax_t>, std::deca>;
    using hectoseconds      = unit<unit_type::time<intmax_t>, std::hecto>;
    using kiloseconds       = unit<unit_type::time<intmax_t>, std::kilo>;
    using megaseconds       = unit<unit_type::time<intmax_t>, std::mega>;
    using gigaseconds       = unit<unit_type::time<intmax_t>, std::giga>;
    using teraseconds       = unit<unit_type::time<intmax_t>, std::tera>;
    using petaseconds       = unit<unit_type::time<intmax_t>, std::peta>;
    using exaseconds        = unit<unit_type::time<intmax_t>, std::exa>;

    using minutes           = unit<unit_type::time<intmax_t>, std::ratio<60, 1>>;
    using hours             = unit<unit_type::time<intmax_t>, std::ratio<3600, 1>>;
    using days              = unit<unit_type::time<intmax_t>, std::ratio<86400, 1>>;
    using weeks             = unit<unit_type::time<intmax_t>, std::ratio<604800, 1>>;
    using fortnights        = unit<unit_type::time<intmax_t>, std::ratio<1209600, 1>>;
    using common_years      = unit<unit_type::time<intmax_t>, std::ratio<31536000, 1>>;
    using leap_years        = unit<unit_type::time<intmax_t>, std::ratio<31622400, 1>>;
    using sidereal_years    = unit<unit_type::time<intmax_t>, std::ratio<31558149504LL, 1000>>;

#pragma endregion



    // Electrical current; base is ampere
#pragma region Current

    using attoamps  = unit<unit_type::current<intmax_t>, std::atto>;
    using femtoamps = unit<unit_type::current<intmax_t>, std::femto>;
    using picoamps  = unit<unit_type::current<intmax_t>, std::pico>;
    using nanoamps  = unit<unit_type::current<intmax_t>, std::nano>;
    using microamps = unit<unit_type::current<intmax_t>, std::micro>;
    using milliamps = unit<unit_type::current<intmax_t>, std::milli>;
    using centiamps = unit<unit_type::current<intmax_t>, std::centi>;
    using deciamps  = unit<unit_type::current<intmax_t>, std::deci>;
    using amps      = unit<unit_type::current<intmax_t>, std::ratio<1>>;
    using amperes   = amps;
    using decaamps  = unit<unit_type::current<intmax_t>, std::deca>;
    using hectoamps = unit<unit_type::current<intmax_t>, std::hecto>;
    using kiloamps  = unit<unit_type::current<intmax_t>, std::kilo>;
    using megaamps  = unit<unit_type::current<intmax_t>, std::mega>;
    using gigaamps  = unit<unit_type::current<intmax_t>, std::giga>;
    using teraamps  = unit<unit_type::current<intmax_t>, std::tera>;
    using petaamps  = unit<unit_type::current<intmax_t>, std::peta>;
    using exaamps   = unit<unit_type::current<intmax_t>, std::exa>;

#pragma endregion

#pragma endregion
}
