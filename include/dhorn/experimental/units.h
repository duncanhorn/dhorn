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
    namespace experimental
    {
        /*
         * Conversion helpers (squared, cubed, etc.)
         */
        namespace details
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
        namespace details
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
            inline constexpr Ty ratio_convert(const Ty &val)
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
            struct length : public details::unit_type_base<Ty> {};

            template <typename Ty>
            struct area : public details::unit_type_base<Ty> {};

            template <typename Ty>
            struct volume : public details::unit_type_base<Ty> {};

            template <typename Ty>
            struct mass : public details::unit_type_base<Ty> {};

            template <typename Ty>
            struct time : public details::unit_type_base<Ty> {};

            template <typename Ty>
            struct current : public details::unit_type_base<Ty> {};
        }

#pragma endregion



        template <typename UnitType, typename Ratio>
        class unit final
        {
            static_assert(details::is_ratio<Ratio>::value, "Second template argument must be of type std::ratio");

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

            constexpr explicit unit(value_type val) :
                _value(std::move(val))
            {
            }

            template <typename OtherRatio>
            constexpr unit(const unit<UnitType, OtherRatio> &other) :
                _value(details::ratio_convert<OtherRatio, Ratio>(other.value()))
            {
            }

            // Default copy/move
            unit(const unit &) = default;
            unit(unit &&) = default;
            unit &operator=(const unit &) = default;
            unit &operator=(unit &&) = default;



            /*
             * Operators
             */
            unit &operator=(const value_type &val)
            {
                this->_value = val;
                return *this;
            }

            template <typename OtherRatio>
            unit &operator=(const unit<UnitType, OtherRatio> &other)
            {
                this->_value = details::ratio_convert<OtherRatio, Ratio>(other.value());
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
            unit &operator*=(const value_type &val)
            {
                this->_value *= val;
                return *this;
            }

            unit &operator/=(const value_type &val)
            {
                this->_value /= val;
                return *this;
            }

            unit &operator%=(const value_type &val)
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
        inline unit<UnitType, Ratio> operator*(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            return unit<UnitType, Ratio>(lhs.value() * rhs);
        }

        template <typename ValueType, typename UnitType, typename Ratio>
        inline unit<UnitType, Ratio> operator*(const ValueType &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return unit<UnitType, Ratio>(lhs * rhs.value());
        }

        template <typename UnitType, typename Ratio, typename ValueType>
        inline unit<UnitType, Ratio> operator/(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            // Note that it does not make sense to divide a value by a unit; only a unit by a value
            return unit<UnitType, Ratio>(lhs.value() / rhs);
        }

        template <typename UnitType, typename Ratio, typename ValueType>
        inline unit<UnitType, Ratio> operator%(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            // Note that it does not make sense to divide a value by a unit; only a unit by a value
            return unit<UnitType, Ratio>(lhs.value() % rhs);
        }



        /*
         * Operators
         */
        template <typename UnitType, typename Ratio, typename ValueType>
        inline bool operator==(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            return lhs.value() == rhs;
        }

        template <typename ValueType, typename UnitType, typename Ratio>
        inline bool operator==(const ValueType &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return lhs == rhs.value();
        }

        template <typename UnitType, typename Ratio>
        inline bool operator==(const unit<UnitType, Ratio> &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return lhs.value() == rhs.value();
        }

        template <typename UnitType, typename Ratio, typename ValueType>
        inline bool operator!=(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            return lhs.value() != rhs;
        }

        template <typename ValueType, typename UnitType, typename Ratio>
        inline bool operator!=(const ValueType &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return lhs != rhs.value();
        }

        template <typename UnitType, typename Ratio>
        inline bool operator!=(const unit<UnitType, Ratio> &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return lhs.value() != rhs.value();
        }

        template <typename UnitType, typename Ratio, typename ValueType>
        inline unit<UnitType, Ratio> operator+(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            return unit<UnitType, Ratio>(lhs.value() + rhs);
        }

        template <typename ValueType, typename UnitType, typename Ratio>
        inline unit<UnitType, Ratio> operator+(const ValueType &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return unit<UnitType, Ratio>(lhs + rhs.value());
        }

        template <typename UnitType, typename Ratio>
        inline unit<UnitType, Ratio> operator+(const unit<UnitType, Ratio> &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return unit<UnitType, Ratio>(lhs.value() + rhs.value());
        }

        template <typename UnitType, typename Ratio, typename ValueType>
        inline unit<UnitType, Ratio> operator-(const unit<UnitType, Ratio> &lhs, const ValueType &rhs)
        {
            return unit<UnitType, Ratio>(lhs.value() - rhs);
        }

        template <typename ValueType, typename UnitType, typename Ratio>
        inline unit<UnitType, Ratio> operator-(const ValueType &lhs, const unit<UnitType, Ratio> &rhs)
        {
            return unit<UnitType, Ratio>(lhs - rhs.value());
        }

        template <typename UnitType, typename Ratio>
        inline unit<UnitType, Ratio> operator-(const unit<UnitType, Ratio> &lhs, const unit<UnitType, Ratio> &rhs)
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
            typename = typename std::enable_if<details::unit_traits<TargetType>::is_unit>::type,
            typename = typename std::enable_if<std::is_same<UnitType, typename details::unit_traits<TargetType>::unit_type>::value>::type>
        inline constexpr TargetType unit_cast(const unit<UnitType, Ratio> &val)
        {
            return TargetType(val);
        }

#pragma endregion



#pragma region Type Definitions

        // Length; base is meter
#pragma region Length

        // Metric
        template <typename Ty> using attometers_t       = unit<unit_type::length<Ty>, std::atto>;
        template <typename Ty> using femtometers_t      = unit<unit_type::length<Ty>, std::femto>;
        template <typename Ty> using picometers_t       = unit<unit_type::length<Ty>, std::pico>;
        template <typename Ty> using nanometers_t       = unit<unit_type::length<Ty>, std::nano>;
        template <typename Ty> using micrometers_t      = unit<unit_type::length<Ty>, std::micro>;
        template <typename Ty> using millimeters_t      = unit<unit_type::length<Ty>, std::milli>;
        template <typename Ty> using centimeters_t      = unit<unit_type::length<Ty>, std::centi>;
        template <typename Ty> using decimeters_t       = unit<unit_type::length<Ty>, std::deci>;
        template <typename Ty> using meters_t           = unit<unit_type::length<Ty>, std::ratio<1>>;
        template <typename Ty> using decameters_t       = unit<unit_type::length<Ty>, std::deca>;
        template <typename Ty> using hectometers_t      = unit<unit_type::length<Ty>, std::hecto>;
        template <typename Ty> using kilometers_t       = unit<unit_type::length<Ty>, std::kilo>;
        template <typename Ty> using megameters_t       = unit<unit_type::length<Ty>, std::mega>;
        template <typename Ty> using gigameters_t       = unit<unit_type::length<Ty>, std::giga>;
        template <typename Ty> using terameters_t       = unit<unit_type::length<Ty>, std::tera>;
        template <typename Ty> using petameters_t       = unit<unit_type::length<Ty>, std::peta>;
        template <typename Ty> using exameters_t        = unit<unit_type::length<Ty>, std::exa>;

        // Imperial
        template <typename Ty> using thous_t            = unit<unit_type::length<Ty>, std::ratio<254, 10000000>>;
        template <typename Ty> using inches_t           = unit<unit_type::length<Ty>, std::ratio<254, 10000>>;
        template <typename Ty> using feet_t             = unit<unit_type::length<Ty>, std::ratio<3048, 10000>>;
        template <typename Ty> using yards_t            = unit<unit_type::length<Ty>, std::ratio<9144, 10000>>;
        template <typename Ty> using chains_t           = unit<unit_type::length<Ty>, std::ratio<201168, 10000>>;
        template <typename Ty> using furlongs_t         = unit<unit_type::length<Ty>, std::ratio<201168, 1000>>;
        template <typename Ty> using miles_t            = unit<unit_type::length<Ty>, std::ratio<1609344, 1000>>;
        template <typename Ty> using leagues_t          = unit<unit_type::length<Ty>, std::ratio<4828032, 1000>>;
        template <typename Ty> using fathoms_t          = unit<unit_type::length<Ty>, std::ratio<18288, 10000>>;
        template <typename Ty> using cables_t           = unit<unit_type::length<Ty>, std::ratio<1853184, 10000>>;
        template <typename Ty> using nautical_miles_t   = unit<unit_type::length<Ty>, std::ratio<1853184, 1000>>;
        template <typename Ty> using links_t            = unit<unit_type::length<Ty>, std::ratio<201168, 1000000>>;
        template <typename Ty> using rods_t             = unit<unit_type::length<Ty>, std::ratio<50292, 10000>>;



        // Metric
        using attometers        = attometers_t<intmax_t>;
        using femtometers       = femtometers_t<intmax_t>;
        using picometers        = picometers_t<intmax_t>;
        using nanometers        = nanometers_t<intmax_t>;
        using micrometers       = micrometers_t<intmax_t>;
        using millimeters       = millimeters_t<intmax_t>;
        using centimeters       = centimeters_t<intmax_t>;
        using decimeters        = decimeters_t<intmax_t>;
        using meters            = meters_t<intmax_t>;
        using decameters        = decameters_t<intmax_t>;
        using hectometers       = hectometers_t<intmax_t>;
        using kilometers        = kilometers_t<intmax_t>;
        using megameters        = megameters_t<intmax_t>;
        using gigameters        = gigameters_t<intmax_t>;
        using terameters        = terameters_t<intmax_t>;
        using petameters        = petameters_t<intmax_t>;
        using exameters         = exameters_t<intmax_t>;

        // Imperial
        using thous             = thous_t<intmax_t>;
        using inches            = inches_t<intmax_t>;
        using feet              = feet_t<intmax_t>;
        using yards             = yards_t<intmax_t>;
        using chains            = chains_t<intmax_t>;
        using furlongs          = furlongs_t<intmax_t>;
        using miles             = miles_t<intmax_t>;
        using leagues           = leagues_t<intmax_t>;
        using fathoms           = fathoms_t<intmax_t>;
        using cables            = cables_t<intmax_t>;
        using nautical_miles    = nautical_miles_t<intmax_t>;
        using links             = links_t<intmax_t>;
        using rods              = rods_t<intmax_t>;

        inline namespace literals
        {
            inline namespace unit_literals
            {
                // Metric
                inline constexpr nanometers operator ""_nm(unsigned long long value)
                {
                    return nanometers(value);
                }

                inline constexpr nanometers_t<long double> operator ""_nm(long double value)
                {
                    return nanometers_t<long double>(value);
                }

                inline constexpr micrometers operator ""_um(unsigned long long value)
                {
                    return micrometers(value);
                }

                inline constexpr micrometers_t<long double> operator ""_um(long double value)
                {
                    return micrometers_t<long double>(value);
                }

                inline constexpr millimeters operator ""_mm(unsigned long long value)
                {
                    return millimeters(value);
                }

                inline constexpr millimeters_t<long double> operator ""_mm(long double value)
                {
                    return millimeters_t<long double>(value);
                }

                inline constexpr centimeters operator ""_cm(unsigned long long value)
                {
                    return centimeters(value);
                }

                inline constexpr centimeters_t<long double> operator ""_cm(long double value)
                {
                    return centimeters_t<long double>(value);
                }

                inline constexpr meters operator ""_m(unsigned long long value)
                {
                    return meters(value);
                }

                inline constexpr meters_t<long double> operator ""_m(long double value)
                {
                    return meters_t<long double>(value);
                }

                inline constexpr kilometers operator ""_km(unsigned long long value)
                {
                    return kilometers(value);
                }

                inline constexpr kilometers_t<long double> operator ""_km(long double value)
                {
                    return kilometers_t<long double>(value);
                }

                // Imperial
                inline constexpr inches operator ""_in(unsigned long long value)
                {
                    return inches(value);
                }

                inline constexpr inches_t<long double> operator ""_in(long double value)
                {
                    return inches_t<long double>(value);
                }

                inline constexpr feet operator ""_ft(unsigned long long value)
                {
                    return feet(value);
                }

                inline constexpr feet_t<long double> operator ""_ft(long double value)
                {
                    return feet_t<long double>(value);
                }

                inline constexpr yards operator ""_yd(unsigned long long value)
                {
                    return yards(value);
                }

                inline constexpr yards_t<long double> operator ""_yd(long double value)
                {
                    return yards_t<long double>(value);
                }

                inline constexpr miles operator ""_mi(unsigned long long value)
                {
                    return miles(value);
                }

                inline constexpr miles_t<long double> operator ""_mi(long double value)
                {
                    return miles_t<long double>(value);
                }
            }
        }

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
        template <typename Ty> using attograms_t        = unit<unit_type::mass<Ty>, std::atto>;
        template <typename Ty> using femtograms_t       = unit<unit_type::mass<Ty>, std::femto>;
        template <typename Ty> using picograms_t        = unit<unit_type::mass<Ty>, std::pico>;
        template <typename Ty> using nanograms_t        = unit<unit_type::mass<Ty>, std::nano>;
        template <typename Ty> using micrograms_t       = unit<unit_type::mass<Ty>, std::micro>;
        template <typename Ty> using milligrams_t       = unit<unit_type::mass<Ty>, std::milli>;
        template <typename Ty> using centigrams_t       = unit<unit_type::mass<Ty>, std::centi>;
        template <typename Ty> using decigrams_t        = unit<unit_type::mass<Ty>, std::deci>;
        template <typename Ty> using grams_t            = unit<unit_type::mass<Ty>, std::ratio<1>>;
        template <typename Ty> using decagrams_t        = unit<unit_type::mass<Ty>, std::deca>;
        template <typename Ty> using hectograms_t       = unit<unit_type::mass<Ty>, std::hecto>;
        template <typename Ty> using kilograms_t        = unit<unit_type::mass<Ty>, std::kilo>;
        template <typename Ty> using megagrams_t        = unit<unit_type::mass<Ty>, std::mega>;
        template <typename Ty> using gigagrams_t        = unit<unit_type::mass<Ty>, std::giga>;
        template <typename Ty> using teragrams_t        = unit<unit_type::mass<Ty>, std::tera>;
        template <typename Ty> using petagrams_t        = unit<unit_type::mass<Ty>, std::peta>;
        template <typename Ty> using exagrams_t         = unit<unit_type::mass<Ty>, std::exa>;

        template <typename Ty> using metric_tonnes_t    = megagrams_t<Ty>;

        // Imperial
        template <typename Ty> using grains_t           = unit<unit_type::mass<Ty>, std::ratio<6479891LL, 100000000LL>>;
        template <typename Ty> using drachms_t          = unit<unit_type::mass<Ty>, std::ratio<17718451953125LL, 10000000000000LL>>;
        template <typename Ty> using ounces_t           = unit<unit_type::mass<Ty>, std::ratio<28349523125LL, 1000000000LL>>;
        template <typename Ty> using pounds_t           = unit<unit_type::mass<Ty>, std::ratio<45359237LL, 100000LL>>;
        template <typename Ty> using stones_t           = unit<unit_type::mass<Ty>, std::ratio<635029318LL, 100000LL>>;
        template <typename Ty> using slugs_t            = unit<unit_type::mass<Ty>, std::ratio<1459390294LL, 100000LL>>;
        template <typename Ty> using quarters_t         = unit<unit_type::mass<Ty>, std::ratio<1270058636LL, 100000000000LL>>;
        template <typename Ty> using hundredweights_t   = unit<unit_type::mass<Ty>, std::ratio<5080234544LL, 100000000000LL>>;
        template <typename Ty> using short_tons_t       = unit<unit_type::mass<Ty>, std::ratio<90718474LL, 100000000LL>>;
        template <typename Ty> using long_tons_t        = unit<unit_type::mass<Ty>, std::ratio<10160469088LL, 10000000000LL>>;



        // Metric
        using attograms         = attograms_t<intmax_t>;
        using femtograms        = femtograms_t<intmax_t>;
        using picograms         = picograms_t<intmax_t>;
        using nanograms         = nanograms_t<intmax_t>;
        using micrograms        = micrograms_t<intmax_t>;
        using milligrams        = milligrams_t<intmax_t>;
        using centigrams        = centigrams_t<intmax_t>;
        using decigrams         = decigrams_t<intmax_t>;
        using grams             = grams_t<intmax_t>;
        using decagrams         = decagrams_t<intmax_t>;
        using hectograms        = hectograms_t<intmax_t>;
        using kilograms         = kilograms_t<intmax_t>;
        using megagrams         = megagrams_t<intmax_t>;
        using gigagrams         = gigagrams_t<intmax_t>;
        using teragrams         = teragrams_t<intmax_t>;
        using petagrams         = petagrams_t<intmax_t>;
        using exagrams          = exagrams_t<intmax_t>;

        using metric_tonnes     = metric_tonnes_t<intmax_t>;

        // Imperial
        using grains            = grains_t<intmax_t>;
        using drachms           = drachms_t<intmax_t>;
        using ounces            = ounces_t<intmax_t>;
        using pounds            = pounds_t<intmax_t>;
        using stones            = stones_t<intmax_t>;
        using slugs             = slugs_t<intmax_t>;
        using quarters          = quarters_t<intmax_t>;
        using hundredweights    = hundredweights_t<intmax_t>;
        using short_tons        = short_tons_t<intmax_t>;
        using long_tons         = long_tons_t<intmax_t>;

        inline namespace literals
        {
            inline namespace unit_literals
            {
                // Metric
                inline constexpr micrograms operator ""_ug(unsigned long long value)
                {
                    return micrograms(value);
                }

                inline constexpr micrograms_t<long double> operator ""_ug(long double value)
                {
                    return micrograms_t<long double>(value);
                }

                inline constexpr milligrams operator ""_mg(unsigned long long value)
                {
                    return milligrams(value);
                }

                inline constexpr milligrams_t<long double> operator ""_mg(long double value)
                {
                    return milligrams_t<long double>(value);
                }

                inline constexpr grams operator ""_g(unsigned long long value)
                {
                    return grams(value);
                }

                inline constexpr grams_t<long double> operator ""_g(long double value)
                {
                    return grams_t<long double>(value);
                }

                inline constexpr kilograms operator ""_kg(unsigned long long value)
                {
                    return kilograms(value);
                }

                inline constexpr kilograms_t<long double> operator ""_kg(long double value)
                {
                    return kilograms_t<long double>(value);
                }

                // Imperial
                inline constexpr ounces operator ""_oz(unsigned long long value)
                {
                    return ounces(value);
                }

                inline constexpr ounces_t<long double> operator ""_oz(long double value)
                {
                    return ounces_t<long double>(value);
                }

                inline constexpr pounds operator ""_lb(unsigned long long value)
                {
                    return pounds(value);
                }

                inline constexpr pounds operator ""_lbs(unsigned long long value)
                {
                    return pounds(value);
                }

                inline constexpr pounds_t<long double> operator ""_lb(long double value)
                {
                    return pounds_t<long double>(value);
                }

                inline constexpr pounds_t<long double> operator ""_lbs(long double value)
                {
                    return pounds_t<long double>(value);
                }
            }
        }

#pragma endregion



    // Time; base is second
#pragma region Time

        template <typename Ty> using attoseconds_t      = unit<unit_type::time<Ty>, std::atto>;
        template <typename Ty> using femtoseconds_t     = unit<unit_type::time<Ty>, std::femto>;
        template <typename Ty> using picoseconds_t      = unit<unit_type::time<Ty>, std::pico>;
        template <typename Ty> using nanoseconds_t      = unit<unit_type::time<Ty>, std::nano>;
        template <typename Ty> using microseconds_t     = unit<unit_type::time<Ty>, std::micro>;
        template <typename Ty> using milliseconds_t     = unit<unit_type::time<Ty>, std::milli>;
        template <typename Ty> using centiseconds_t     = unit<unit_type::time<Ty>, std::centi>;
        template <typename Ty> using deciseconds_t      = unit<unit_type::time<Ty>, std::deci>;
        template <typename Ty> using seconds_t          = unit<unit_type::time<Ty>, std::ratio<1>>;
        template <typename Ty> using decaseconds_t      = unit<unit_type::time<Ty>, std::deca>;
        template <typename Ty> using hectoseconds_t     = unit<unit_type::time<Ty>, std::hecto>;
        template <typename Ty> using kiloseconds_t      = unit<unit_type::time<Ty>, std::kilo>;
        template <typename Ty> using megaseconds_t      = unit<unit_type::time<Ty>, std::mega>;
        template <typename Ty> using gigaseconds_t      = unit<unit_type::time<Ty>, std::giga>;
        template <typename Ty> using teraseconds_t      = unit<unit_type::time<Ty>, std::tera>;
        template <typename Ty> using petaseconds_t      = unit<unit_type::time<Ty>, std::peta>;
        template <typename Ty> using exaseconds_t       = unit<unit_type::time<Ty>, std::exa>;

        template <typename Ty> using minutes_t          = unit<unit_type::time<Ty>, std::ratio<60, 1>>;
        template <typename Ty> using hours_t            = unit<unit_type::time<Ty>, std::ratio<3600, 1>>;
        template <typename Ty> using days_t             = unit<unit_type::time<Ty>, std::ratio<86400, 1>>;
        template <typename Ty> using weeks_t            = unit<unit_type::time<Ty>, std::ratio<604800, 1>>;
        template <typename Ty> using fortnights_t       = unit<unit_type::time<Ty>, std::ratio<1209600, 1>>;
        template <typename Ty> using common_years_t     = unit<unit_type::time<Ty>, std::ratio<31536000, 1>>;
        template <typename Ty> using leap_years_t       = unit<unit_type::time<Ty>, std::ratio<31622400, 1>>;
        template <typename Ty> using sidereal_years_t   = unit<unit_type::time<Ty>, std::ratio<31558149504LL, 1000>>;



        using attoseconds       = attoseconds_t<intmax_t>;
        using femtoseconds      = femtoseconds_t<intmax_t>;
        using picoseconds       = picoseconds_t<intmax_t>;
        using nanoseconds       = nanoseconds_t<intmax_t>;
        using microseconds      = microseconds_t<intmax_t>;
        using milliseconds      = milliseconds_t<intmax_t>;
        using centiseconds      = centiseconds_t<intmax_t>;
        using deciseconds       = deciseconds_t<intmax_t>;
        using seconds           = seconds_t<intmax_t>;
        using decaseconds       = decaseconds_t<intmax_t>;
        using hectoseconds      = hectoseconds_t<intmax_t>;
        using kiloseconds       = kiloseconds_t<intmax_t>;
        using megaseconds       = megaseconds_t<intmax_t>;
        using gigaseconds       = gigaseconds_t<intmax_t>;
        using teraseconds       = teraseconds_t<intmax_t>;
        using petaseconds       = petaseconds_t<intmax_t>;
        using exaseconds        = exaseconds_t<intmax_t>;

        using minutes           = minutes_t<intmax_t>;
        using hours             = hours_t<intmax_t>;
        using days              = days_t<intmax_t>;
        using weeks             = weeks_t<intmax_t>;
        using fortnights        = fortnights_t<intmax_t>;
        using common_years      = common_years_t<intmax_t>;
        using leap_years        = leap_years_t<intmax_t>;
        using sidereal_years    = sidereal_years_t<intmax_t>;

        inline namespace literals
        {
            inline namespace unit_literals
            {
                inline constexpr picoseconds operator ""_ps(unsigned long long value)
                {
                    return picoseconds(value);
                }

                inline constexpr picoseconds_t<long double> operator ""_ps(long double value)
                {
                    return picoseconds_t<long double>(value);
                }

                inline constexpr nanoseconds operator ""_ns(unsigned long long value)
                {
                    return nanoseconds(value);
                }

                inline constexpr nanoseconds_t<long double> operator ""_ns(long double value)
                {
                    return nanoseconds_t<long double>(value);
                }

                inline constexpr microseconds operator ""_us(unsigned long long value)
                {
                    return microseconds(value);
                }

                inline constexpr microseconds_t<long double> operator ""_us(long double value)
                {
                    return microseconds_t<long double>(value);
                }

                inline constexpr milliseconds operator ""_ms(unsigned long long value)
                {
                    return milliseconds(value);
                }

                inline constexpr milliseconds_t<long double> operator ""_ms(long double value)
                {
                    return milliseconds_t<long double>(value);
                }

                inline constexpr seconds operator ""_s(unsigned long long value)
                {
                    return seconds(value);
                }

                inline constexpr seconds_t<long double> operator ""_s(long double value)
                {
                    return seconds_t<long double>(value);
                }

                inline constexpr minutes operator ""_min(unsigned long long value)
                {
                    return minutes(value);
                }

                inline constexpr minutes_t<long double> operator ""_min(long double value)
                {
                    return minutes_t<long double>(value);
                }

                inline constexpr hours operator ""_h(unsigned long long value)
                {
                    return hours(value);
                }

                inline constexpr hours_t<long double> operator ""_h(long double value)
                {
                    return hours_t<long double>(value);
                }
            }
        }

#pragma endregion



    // Electrical current; base is ampere
#pragma region Current

        template <typename Ty> using attoamps_t         = unit<unit_type::current<Ty>, std::atto>;
        template <typename Ty> using femtoamps_t        = unit<unit_type::current<Ty>, std::femto>;
        template <typename Ty> using picoamps_t         = unit<unit_type::current<Ty>, std::pico>;
        template <typename Ty> using nanoamps_t         = unit<unit_type::current<Ty>, std::nano>;
        template <typename Ty> using microamps_t        = unit<unit_type::current<Ty>, std::micro>;
        template <typename Ty> using milliamps_t        = unit<unit_type::current<Ty>, std::milli>;
        template <typename Ty> using centiamps_t        = unit<unit_type::current<Ty>, std::centi>;
        template <typename Ty> using deciamps_t         = unit<unit_type::current<Ty>, std::deci>;
        template <typename Ty> using amps_t             = unit<unit_type::current<Ty>, std::ratio<1>>;
        template <typename Ty> using amperes_t          = amps_t<Ty>;
        template <typename Ty> using decaamps_t         = unit<unit_type::current<Ty>, std::deca>;
        template <typename Ty> using hectoamps_t        = unit<unit_type::current<Ty>, std::hecto>;
        template <typename Ty> using kiloamps_t         = unit<unit_type::current<Ty>, std::kilo>;
        template <typename Ty> using megaamps_t         = unit<unit_type::current<Ty>, std::mega>;
        template <typename Ty> using gigaamps_t         = unit<unit_type::current<Ty>, std::giga>;
        template <typename Ty> using teraamps_t         = unit<unit_type::current<Ty>, std::tera>;
        template <typename Ty> using petaamps_t         = unit<unit_type::current<Ty>, std::peta>;
        template <typename Ty> using exaamps_t          = unit<unit_type::current<Ty>, std::exa>;



        using attoamps          = attoamps_t<intmax_t>;
        using femtoamps         = femtoamps_t<intmax_t>;
        using picoamps          = picoamps_t<intmax_t>;
        using nanoamps          = nanoamps_t<intmax_t>;
        using microamps         = microamps_t<intmax_t>;
        using milliamps         = milliamps_t<intmax_t>;
        using centiamps         = centiamps_t<intmax_t>;
        using deciamps          = deciamps_t<intmax_t>;
        using amps              = amps_t<intmax_t>;
        using amperes           = amperes_t<intmax_t>;
        using decaamps          = decaamps_t<intmax_t>;
        using hectoamps         = hectoamps_t<intmax_t>;
        using kiloamps          = kiloamps_t<intmax_t>;
        using megaamps          = megaamps_t<intmax_t>;
        using gigaamps          = gigaamps_t<intmax_t>;
        using teraamps          = teraamps_t<intmax_t>;
        using petaamps          = petaamps_t<intmax_t>;
        using exaamps           = exaamps_t<intmax_t>;

        inline namespace literals
        {
            inline namespace unit_literals
            {
                inline constexpr picoamps operator ""_pA(unsigned long long value)
                {
                    return picoamps(value);
                }

                inline constexpr picoamps_t<long double> operator ""_pA(long double value)
                {
                    return picoamps_t<long double>(value);
                }

                inline constexpr nanoamps operator ""_nA(unsigned long long value)
                {
                    return nanoamps(value);
                }

                inline constexpr nanoamps_t<long double> operator ""_nA(long double value)
                {
                    return nanoamps_t<long double>(value);
                }

                inline constexpr microamps operator ""_uA(unsigned long long value)
                {
                    return microamps(value);
                }

                inline constexpr microamps_t<long double> operator ""_uA(long double value)
                {
                    return microamps_t<long double>(value);
                }

                inline constexpr milliamps operator ""_mA(unsigned long long value)
                {
                    return milliamps(value);
                }

                inline constexpr milliamps_t<long double> operator ""_mA(long double value)
                {
                    return milliamps_t<long double>(value);
                }

                inline constexpr amps operator ""_A(unsigned long long value)
                {
                    return amps(value);
                }

                inline constexpr amps_t<long double> operator ""_A(long double value)
                {
                    return amps_t<long double>(value);
                }
            }
        }

#pragma endregion

#pragma endregion
    }
}
