/*
 * Duncan Horn
 *
 * math.h
 *
 * Useful math functions
 */
#pragma once

#include <cmath>
#include <type_traits>

namespace dhorn
{
    namespace experimental
    {
        /*
         * Point/Rect structures
         */
#pragma region point/rect

        template <typename Ty>
        struct point
        {
            /*
             * Data
             */
            Ty x;
            Ty y;



            /*
             * Constructor(s)/Destructor
             */
            point() : x{}, y{}
            {
            }

            point(Ty x, Ty y) :
                x(x),
                y(y)
            {
            }



            /*
             * Operators
             */
            point &operator+=(const point &rhs)
            {
                this->x += rhs.x;
                this->y += rhs.y;
                return *this;
            }

            point operator+(const point &rhs) const
            {
                point pt = *this;
                return pt += rhs;
            }

            point &operator-=(const point &rhs)
            {
                this->x -= rhs.x;
                this->y -= rhs.y;
                return *this;
            }

            point operator-(const point &rhs) const
            {
                point pt = *this;
                return pt -= rhs;
            }
        };

        /*
         * Operators
         */
        template <typename LhsTy, typename RhsTy>
        inline auto operator==(const point<LhsTy> &lhs, const point<RhsTy> &rhs) ->
            decltype(LhsTy() == RhsTy())
        {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y);
        }

        template <typename LhsTy, typename RhsTy>
        inline auto operator!=(const point<LhsTy> &lhs, const point<RhsTy> &rhs) ->
            decltype(LhsTy() == RhsTy())
        {
            return !(lhs == rhs);
        }



        template <typename Ty>
        struct size
        {
            /*
             * Data
             */
            Ty width;
            Ty height;



            /*
             * Constructor(s)/Destructor
             */
            size() : width{}, height{}
            {
            }

            size(Ty width, Ty height) :
                width(width),
                height(height)
            {
            }



            /*
             * Operators
             */
            size &operator+=(const size &rhs)
            {
                this->width += rhs.width;
                this->height += rhs.height;
                return *this;
            }

            size operator+(const size &rhs) const
            {
                size result = *this;
                return result += rhs;
            }

            size &operator-=(const size &rhs)
            {
                this->width -= rhs.width;
                this->height -= rhs.height;
                return *this;
            }

            size operator-(const size &rhs) const
            {
                size result = *this;
                return result -= rhs;
            }
        };

        /*
         * Operators
         */
        template <typename LhsTy, typename RhsTy>
        inline auto operator==(const size<LhsTy> &lhs, const size<RhsTy> &rhs) ->
            decltype(LhsTy() == RhsTy())
        {
            return (lhs.width == rhs.width) && (lhs.height == rhs.height);
        }

        template <typename LhsTy, typename RhsTy>
        inline auto operator!=(const size<LhsTy> &lhs, const size<RhsTy> &rhs) ->
            decltype(LhsTy() == RhsTy())
        {
            return !(lhs == rhs);
        }



        template <typename Ty>
        struct rect
        {
            /*
             * Data
             */
            Ty x;
            Ty y;
            Ty width;
            Ty height;



            /*
             * Constructor(s)/Destructor
             */
            rect() : x{}, y{}, width{}, height{}
            {
            }

            rect(Ty width, Ty height) :
                x{}, y{},
                width(width),
                height(height)
            {
            }

            rect(Ty x, Ty y, Ty width, Ty height) :
                x(x),
                y(y),
                width(width),
                height(height)
            {
            }



            /*
             * Operators
             */
            bool operator==(const rect &rhs) const
            {
                return
                    (this->x == rhs.x) &&
                    (this->y == rhs.y) &&
                    (this->width == rhs.width) &&
                    (this->height == rhs.height);
            }

            bool operator!=(const rect &rhs) const
            {
                return !(*this == rhs);
            }

            template <typename PointTy>
            rect &operator+=(const point<PointTy> &rhs)
            {
                this->x += rhs.x;
                this->y += rhs.y;
                return *this;
            }

            template <typename PointTy>
            rect &operator-=(const point<PointTy> &rhs)
            {
                this->x -= rhs.x;
                this->y -= rhs.y;
                return *this;
            }
        };

        /*
         * Operators
         */
        template <typename RectTy, typename PointTy>
        rect<RectTy> operator+(const rect<RectTy> &lhs, const point<PointTy> &rhs)
        {
            rect<RectTy> result = lhs;
            return result += rhs;
        }

        template <typename RectTy, typename PointTy>
        rect<RectTy> operator+(const point<PointTy> &lhs, const rect<RectTy> &rhs)
        {
            rect<RectTy> result = rhs;
            return result += lhs;
        }

        template <typename RectTy, typename PointTy>
        rect<RectTy> operator-(const rect<RectTy> &lhs, const point<PointTy> &rhs)
        {
            rect<RectTy> result = lhs;
            return result -= rhs;
        }

        template <typename RectTy, typename PointTy>
        rect<RectTy> operator-(const point<PointTy> &lhs, const rect<RectTy> &rhs)
        {
            rect<RectTy> result = rhs;
            return result -= lhs;
        }

#pragma endregion



        /*
         * length_squared
         */
#pragma region Length Squared

        template <typename Ty>
        inline Ty length_squared(const Ty &val)
        {
            return (val * val);
        }

        template <typename Ty, typename... Types>
        inline auto length_squared(const Ty &val, const Types &...vals) ->
            typename std::common_type<Ty, Types...>::type
        {
            return (val * val) * length_squared(vals...);
        }

#pragma endregion



        /*
         * length
         */
#pragma region Length

        template <typename... Types>
        auto length(const Types &...vals) ->
            decltype(std::sqrt(length_squared(length_squared(vals...))))
        {
            return std::sqrt(length_squared(vals...));
        }

#pragma endregion
    }
}
