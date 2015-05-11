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
        bool operator==(_In_ const point &rhs) const
        {
            return (this->x == rhs.x) && (this->y == rhs.y);
        }

        bool operator!=(_In_ const point &rhs) const
        {
            return !(*this == rhs);
        }

        point &operator+=(_In_ const point &rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        point operator+(_In_ const point &rhs) const
        {
            point pt = *this;
            return pt += rhs;
        }

        point &operator-=(_In_ const point &rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }

        point operator-(_In_ const point &rhs) const
        {
            point pt = *this;
            return pt -= rhs;
        }
    };



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
        bool operator==(_In_ const rect &rhs) const
        {
            return
                (this->x == rhs.x) &&
                (this->y == rhs.y) &&
                (this->width == rhs.width) &&
                (this->height == rhs.height);
        }

        bool operator!=(_In_ const rect &rhs) const
        {
            return !(*this == rhs);
        }

        template <typename PointTy>
        rect &operator+=(_In_ const point<PointTy> &rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        template <typename PointTy>
        rect &operator-=(_In_ const point<PointTy> &rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }
    };

    template <typename RectTy, typename PointTy>
    rect<RectTy> operator+(_In_ const rect<RectTy> &lhs, _In_ const point<PointTy> &rhs)
    {
        rect<RectTy> result = lhs;
        return result += rhs;
    }

    template <typename RectTy, typename PointTy>
    rect<RectTy> operator+(_In_ const point<PointTy> &lhs, _In_ const rect<RectTy> &rhs)
    {
        rect<RectTy> result = rhs;
        return result += lhs;
    }

    template <typename RectTy, typename PointTy>
    rect<RectTy> operator-(_In_ const rect<RectTy> &lhs, _In_ const point<PointTy> &rhs)
    {
        rect<RectTy> result = lhs;
        return result -= rhs;
    }

    template <typename RectTy, typename PointTy>
    rect<RectTy> operator-(_In_ const point<PointTy> &lhs, _In_ const rect<RectTy> &rhs)
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
    inline Ty length_squared(_In_ Ty val)
    {
        return (val * val);
    }

    template <typename Ty, typename... Types>
    inline auto length_squared(_In_ Ty val, _In_ Types... vals) ->
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
    auto length(_In_ Types... vals) ->
        decltype(sqrt(length_squared(length_squared(vals...))))
    {
        return std::sqrt(length_squared(vals...));
    }

#pragma endregion
}
