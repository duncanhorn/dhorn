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
        template <typename Ty, size_t Power>
        struct unit_power
        {
            static_assert(Power != 0, "Power of zero is not supported");

            using type = Ty;

            // TODO: Due to new naming, give this function a name
            inline constexpr Ty operator()(_In_ const Ty &val) const
            {
                return val * ((Power == 1) ? 1 : unit_power<Power - 1>(val));
            }
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



        template <typename Lhs, typename Rhs>
        struct unit_result_type;

        template <typename UnitType1, typename Ratio1, typename UnitType2, typename Ratio2>
        struct unit_result_type<unit<UnitType1, Ratio1>, unit<UnitType2, Ratio2>>
        {
        private:
            // The result of arithmetic operation op(val1, val2)
            //      val1 = x * (n1 / d1)
            //      val2 = y * (n2 / d2)
            // is going to have a resulting ratio of type std::ratio<n', d'> where
            //      n' = GCF(n1 * d2, n2 * d1)
            //      d' = d1 * d2
            //  and a resulting value of
            //      op(x * n1 * d2, y * n2 * d1) / n'
            //
            // Note the candidates for integer overflow:
            //      n1 * d2
            //      n2 * d1
            //      d1 * d2
            //
            // The last one (d1 * d2) is particularly worrisome in cases where both units are very "small" (think
            // adding picometers to picometers). The other two aren't as worrisome (since adding gigameters to
            // picometers will almost certainly overflow in units of picometers), but we should still give the correct
            // ratio for the result type and instead let the overflow happen when the operation takes place. E.g. if
            // we are adding zero gigameters to 10 picometers, we would expect no overflow to take place. To accomplish
            // this, we need to separate numerators and denominators out so that we can deal with and reduce each
            // separately
            using Num1 = std::ratio<Ratio1::num, 1>;
            using Num2 = std::ratio<Ratio2::num, 1>;
            using Den1 = std::ratio<1, Ratio1::den>;
            using Den2 = std::ratio<1, Ratio2::den>;

            using DenRatio1 = std::ratio_divide<Den1, Den2>;
            using DenRatio2 = std::ratio_divide<Den2, Den1>;




            // Attempt to reduce the denominator of each ratio. Note that if the numerator of one is very large and
            // the denominator of the other is very large, we will likely have integer overflow, but even if we were
            // able to avoid it here, the final result would more than likely have the same issue
            using Den1Ratio = std::ratio<1, Ratio1::den>;
            using Den2Ratio = std::ratio<1, Ratio2::den>;






            using Unit1 = unit<UnitType1, Ratio1>;
            using Unit2 = unit<UnitType2, Ratio2>;

            using ValueType1 = typename UnitType1::type;
            using ValueType2 = typename UnitType2::type;
            using CommonValueType = typename std::common_type<ValueType1, ValueType2>::type;

        public:
            // TODO: using type = unit<UnitType, typename std::common_type<Ty, Rhs>::type, Ratio>;
        };
    }



    /*
     * Unit types
     */
#pragma region Unit Types

    namespace unit_type
    {
        template <typename Ty>
        struct length : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct area : public garbage::unit_power<Ty, 2> {};

        template <typename Ty>
        struct volume : public garbage::unit_power<Ty, 3> {};

        template <typename Ty>
        struct mass : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct time : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct current : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct temperature : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct quantity : public garbage::unit_power<Ty, 1> {};

        template <typename Ty>
        struct luminosity : public garbage::unit_power<Ty, 1> {};
    }

#pragma endregion



    // NOTE: Arithmetic operations can very easily overflow if the GCF of the denominators is small compared to the
    // two denominators. Therefore, it is highly recommended that only powers of ten are used
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

        // Default copy/move
        unit(_In_ const unit &) = default;
        unit(_Inout_ unit &&) = default;
        unit &operator=(_In_ const unit &) = default;
        unit &operator=(_Inout_ unit &&) = default;



        /*
         * Operators
         */
        unit &operator=(_In_ const value_type val)
        {
            this->_value = std::move(val);
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
        using target_ratio = typename garbage::unit_traits<TargetType>::ratio_type;
        using convert_ratio = std::ratio_divide<Ratio, target_ratio>;

        return
            ((convert_ratio::num == 1) && (convert_ratio::den == 1))
            ? TargetType(val.value())
            : (
                (convert_ratio::den == 1)
                ? TargetType(val.value() * convert_ratio::num)
                : (
                    (convert_ratio::num == 1)
                    ? TargetType(val.value() / convert_ratio::den)
                    : TargetType(val.value() * convert_ratio::num / convert_ratio::den)
                )
            );
    }

#pragma endregion



#pragma region Type Definitions

    // TODO: Can we get away with not using double...?

    // Length; base is meter
#pragma region Length

    // SI units
    using attometers    = unit<unit_type::length<intmax_t>, std::atto>;
    using femtometers   = unit<unit_type::length<intmax_t>, std::femto>;
    using picometers    = unit<unit_type::length<intmax_t>, std::pico>;
    using nanometers    = unit<unit_type::length<intmax_t>, std::nano>;
    using micrometers   = unit<unit_type::length<intmax_t>, std::micro>;
    using millimeters   = unit<unit_type::length<intmax_t>, std::milli>;
    using centimeters   = unit<unit_type::length<intmax_t>, std::centi>;
    using decimeters    = unit<unit_type::length<intmax_t>, std::deci>;
    using meters        = unit<unit_type::length<intmax_t>, std::ratio<1>>;
    using decameters    = unit<unit_type::length<intmax_t>, std::deca>;
    using hectometers   = unit<unit_type::length<intmax_t>, std::hecto>;
    using kilometers    = unit<unit_type::length<intmax_t>, std::kilo>;
    using megameters    = unit<unit_type::length<intmax_t>, std::mega>;
    using gigameters    = unit<unit_type::length<intmax_t>, std::giga>;
    using terameters    = unit<unit_type::length<intmax_t>, std::tera>;
    using petameters    = unit<unit_type::length<intmax_t>, std::peta>;
    using exameters     = unit<unit_type::length<intmax_t>, std::exa>;

    // TODO: Imperial

#pragma endregion

    //enum class unit_type
    //{
    //    length = 1,
    //    area = 2,
    //    volume = 3,
    //    mass = 4,
    //    time = 5,
    //    current = 6,
    //    temperature = 7,
    //    quantity = 8,
    //    luminosity = 9,
    //};

#pragma endregion
}
