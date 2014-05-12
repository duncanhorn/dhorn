/*
 * vector.h
 *
 * Duncan Horn
 *
 * A C++ wrapper around the DirectX Math vector. It is recommended that the d3d::vector class be
 * used only for storage as well as quick and easy conversion as it is much more likely that the
 * resulting code be much more optimized if the majority of operations work on DirectX::XMVECTOR
 * types. It is for this reason that the return type of most d3d::vector functions be a
 * DirectX::XMVECTOR. In the worst case (unoptimized), one operation on a d3d::vector consists of
 * two loads, one SIMD operation, and one store whereas in the best case (fully optimized), one
 * operation consists of no loads, one SIMD operation, and no store in the case of intermediate
 * values. If many operations need to take place, it is recommended that you cascade these
 * operations together like so:
 *
 *      result = (v1 + v2) - (v3 + v4) * 4;
 *
 * Alternatively, use of the C++11 keyword auto should give roughly the same results:
 *
 *      auto res1 = v1 + v2;
 *      auto res2 = v3 + v4;
 *      res2 = res2 * 4;
 *      auto result = res1 - res2;
 *
 * Of course, performing the above requires that operator+, etc. be defined for the
 * DirectX::XMVECTOR type. If you would not like these definitions for any reason (e.g. you have
 * written your own or are using a separate library that defines them), you can compile with the
 * _DHORN_D3D_VECTOR_NO_OPERATORS define set.
 *
 * In the case of non-operator function calls (e.g. cross_product, length, etc.), the
 * left-hand-side of the operation must be a d3d::vector, so you can either explicitly assign to
 * the relevant type and then perform the operation, or you can add a call to the constructor
 * inline:
 *
 *      float result = vector3(vector3(v1 + v2).cross_product(v3)).length();
 *
 * In the event that you need to perform a more specific operation than offered by a function call
 * or an operator overload, the d3d::vector provides all relevant casts that you should need. For
 * example:
 *
 *      d3d::vector3 v1(...);
 *      bool result = DirectX::XMVector3NearEqual(v1, v2, EPSILON);
 */
#pragma once

namespace dhorn
{
    namespace d3d
    {
        namespace garbage
        {
            template <int _Dim>
            struct vector_traits
            {
            };

            template <>
            struct vector_traits<2>
            {
                static const int dimension = 2;
                using storage_type = DirectX::XMFLOAT2;
                using cross_product_result_type = float;

                /* Functions */
                static inline DirectX::XMVECTOR load(_In_ const storage_type &vector)
                {
                    return DirectX::XMLoadFloat2(&vector);
                }
                static inline void store(_In_ DirectX::FXMVECTOR vector, _Out_ storage_type &dest)
                {
                    DirectX::XMStoreFloat2(&dest, vector);
                }
                static inline DirectX::XMVECTOR normalize(_In_ DirectX::FXMVECTOR vector)
                {
                    return DirectX::XMVector2Normalize(vector);
                }
                static inline bool equals(_In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector2Equal(v1, v2);
                }
                static inline bool not_equals(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector2NotEqual(v1, v2);
                }
                static inline DirectX::XMVECTOR dot(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector2Dot(v1, v2);
                }
                static inline float cross(_In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector2Cross(v1, v2));
                }
                static inline float length(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector2Length(v));
                }
                static inline float length_sq(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(v));
                }
            };

            template <>
            struct vector_traits<3>
            {
                static const int dimension = 3;
                using storage_type = DirectX::XMFLOAT3;
                using cross_product_result_type = DirectX::XMVECTOR;

                /* Functions */
                static inline DirectX::XMVECTOR load(_In_ const storage_type &vector)
                {
                    return DirectX::XMLoadFloat3(&vector);
                }
                static inline void store(_In_ DirectX::FXMVECTOR vector, _Out_ storage_type &dest)
                {
                    DirectX::XMStoreFloat3(&dest, vector);
                }
                static inline DirectX::XMVECTOR normalize(_In_ DirectX::FXMVECTOR vector)
                {
                    return DirectX::XMVector3Normalize(vector);
                }
                static inline bool equals(_In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector3Equal(v1, v2);
                }
                static inline bool not_equals(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector3NotEqual(v1, v2);
                }
                static inline DirectX::XMVECTOR dot(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector3Dot(v1, v2);
                }
                static inline DirectX::XMVECTOR cross(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector3Cross(v1, v2);
                }
                static inline float length(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector3Length(v));
                }
                static inline float length_sq(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(v));
                }
            };

            template <>
            struct vector_traits<4>
            {
                static const int dimension = 4;
                using storage_type = DirectX::XMFLOAT4;
                using cross_product_result_type = DirectX::XMVECTOR;

                /* Functions */
                static inline DirectX::XMVECTOR load(_In_ const storage_type &vector)
                {
                    return DirectX::XMLoadFloat4(&vector);
                }
                static inline void store(_In_ DirectX::FXMVECTOR vector, _Out_ storage_type &dest)
                {
                    DirectX::XMStoreFloat4(&dest, vector);
                }
                static inline DirectX::XMVECTOR normalize(_In_ DirectX::FXMVECTOR vector)
                {
                    return DirectX::XMVector4Normalize(vector);
                }
                static inline bool equals(_In_ DirectX::FXMVECTOR v1, _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector4Equal(v1, v2);
                }
                static inline bool not_equals(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector4NotEqual(v1, v2);
                }
                static inline DirectX::XMVECTOR dot(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2)
                {
                    return DirectX::XMVector4Dot(v1, v2);
                }
                static inline DirectX::XMVECTOR cross(
                    _In_ DirectX::FXMVECTOR v1,
                    _In_ DirectX::FXMVECTOR v2,
                    _In_ DirectX::FXMVECTOR v3)
                {
                    return DirectX::XMVector4Cross(v1, v2, v3);
                }
                static inline float length(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector4Length(v));
                }
                static inline float length_sq(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(v));
                }
            };

            template <int _Dim1, int _Dim2>
            struct vector_result_type
            {
                static const int dimension = (_Dim1 > _Dim2) ? _Dim1 : _Dim2;
                using traits_type = vector_traits<dimension>;
            };
        }

        template <int _Dim = 4>
        class vector
        {
        public:

            /* Type and static variable declaration */
            static const int dimension = _Dim;
            using traits = garbage::vector_traits<_Dim>;
            using storage_type = typename traits::storage_type;

            static const bool has_x = (_Dim >= 1);
            static const bool has_y = (_Dim >= 2);
            static const bool has_z = (_Dim >= 3);
            static const bool has_w = (_Dim >= 4);

        public:
            inline vector(void)
            {
                traits::store(DirectX::g_XMZero, this->_vector);
            }

            inline vector(_In_ const vector &other) :
                _vector(other._vector)
            {
            }

            inline vector(_In_ const storage_type &other) :
                _vector(other)
            {
            }

            inline vector(_In_ DirectX::FXMVECTOR other)
            {
                traits::store(other, this->_vector);
            }



            /*
             * Accessors
             */
            float &x(void)
            {
                return this->_vector.x;
            }

            float &y(void)
            {
                return this->_vector.y;
            }

            float &z(void)
            {
                return this->_vector.z;
            }

            float &w(void)
            {
                return this->_vector.w;
            }



            /*
             * Assignment
             */
            vector &operator=(_In_ const vector &other)
            {
                this->_vector = other._vector;

                return *this;
            }

            vector &operator=(_In_ const storage_type &other)
            {
                this->_vector = other;

                return *this;
            }

            vector &operator=(_In_ DirectX::FXMVECTOR other)
            {
                traits::store(other, this->_vector);
                return *this;
            }



            /*
             * Type Conversion
             */
            inline operator const storage_type &(void) const
            {
                return this->_vector;
            }

            inline operator DirectX::XMVECTOR(void) const
            {
                return traits::load(this->_vector);
            }



            /*
             * Equality operators
             */
            template <int _Dim2>
            inline bool operator==(_In_ const vector<_Dim2> &other) const
            {
                using traits_type = typename garbage::vector_result_type<_Dim, _Dim2>::traits_type;
                return traits_type::equals(*this, other);
            }

            template <int _Dim2>
            inline bool operator!=(_In_ const vector<_Dim2> &other) const
            {
                using traits_type = typename garbage::vector_result_type<_Dim, _Dim2>::traits_type;
                return traits_type::not_equals(*this, other);
            }



            /*
             * Mathematical operations
             */
            inline DirectX::XMVECTOR normalize(void) const
            {
                return traits::normalize(*this);
            }

            inline float dot_product(_In_ const vector &other) const
            {
                // No need to template the dot product function as vector<_Dim> * vector<_Dim2> is
                // going to have zeros in higher resolution spots anyway
                return DirectX::XMVectorGetX(traits::dot(*this, other));
            }

            template <typename... _Args>
            inline typename traits::cross_product_result_type
                cross_product(_In_ _Args... other) const
            {
                return traits::cross(*this, other...);
            }

            inline float length(void) const
            {
                return traits::length(*this);
            }

            inline float length_sq(void) const
            {
                return traits::length_sq(*this);
            }



            /*
             * Operator overloads
             */
            inline DirectX::XMVECTOR operator-(void) const
            {
                return DirectX::XMVectorNegate(*this);
            }


            template <int _Dim2>
            inline DirectX::XMVECTOR operator+(
                _In_ const vector<_Dim2> &other) const
            {
                return DirectX::XMVectorAdd(*this, other);
            }

            inline const vector &operator+=(_In_ const vector &other)
            {
                auto v = DirectX::XMVectorAdd(*this, other);
                traits::store(v, this->_vector);

                return *this;
            }

            inline DirectX::XMVECTOR operator+(_In_ float val) const
            {
                auto v = DirectX::XMVectorSet(val, val, has_z ? val : 0, has_w ? val : 0);
                return *this + v;
            }

            inline vector &operator+=(_In_ float val)
            {
                auto v = DirectX::XMVectorSet(val, val, has_z ? val : 0, has_w ? val : 0);
                auto result = *this + v;
                traits::store(result, this->_vector);

                return *this;
            }

            template <int _Dim2>
            inline DirectX::XMVECTOR operator-(
                _In_ const vector<_Dim2> &other) const
            {
                return DirectX::XMVectorSubtract(*this, other);
            }

            inline const vector &operator-=(_In_ const vector &other)
            {
                auto v = DirectX::XMVectorSubtract(*this, other);
                traits::store(v, this->_vector);

                return *this;
            }

            inline DirectX::XMVECTOR operator-(_In_ float val) const
            {
                auto v = DirectX::XMVectorSet(val, val, has_z ? val : 0, has_w ? val : 0);
                return *this - v;
            }

            inline vector &operator-=(_In_ float val)
            {
                auto v = DirectX::XMVectorSet(val, val, has_z ? val : 0, has_w ? val : 0);
                auto result = *this - v;
                traits::store(result, this->_vector);

                return *this;
            }

        private:

            storage_type _vector;
        };



        /* Type definitions */
        using vector2 = vector<2>;
        using vector3 = vector<3>;
        using vector4 = vector<4>;



        /* Operators */
#pragma region Operators
        template <int _Dim>
        inline DirectX::XMVECTOR operator+(_In_ float lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs + lhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(_In_ float lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector<_Dim>(-rhs) + lhs;
        }



        /* XMFLOAT2 */
        template <int _Dim>
        inline bool operator==(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs == vector2(rhs);
        }

        template <int _Dim>
        inline bool operator==(_In_ const DirectX::XMFLOAT2 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector2(lhs) == rhs;
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs != vector2(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT2 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector2(lhs) != rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs + vector2(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const DirectX::XMFLOAT2 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector2(lhs) + rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs - vector2(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const DirectX::XMFLOAT2 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector2(lhs) - rhs;
        }



        /* XMFLOAT3 */
        template <int _Dim>
        inline bool operator==(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs == vector3(rhs);
        }

        template <int _Dim>
        inline bool operator==(_In_ const DirectX::XMFLOAT3 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector3(lhs) == rhs;
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs != vector3(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT3 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector3(lhs) != rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs + vector3(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const DirectX::XMFLOAT3 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector3(lhs) + rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs - vector3(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const DirectX::XMFLOAT3 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector3(lhs) - rhs;
        }



        /* XMFLOAT4 */
        template <int _Dim>
        inline bool operator==(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs == vector4(rhs);
        }

        template <int _Dim>
        inline bool operator==(_In_ const DirectX::XMFLOAT4 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector4(lhs) == rhs;
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs != vector4(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT4 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return vector4(lhs) != rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs + vector4(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator+(
            _In_ const DirectX::XMFLOAT4 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector4(lhs) + rhs;
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs - vector4(rhs);
        }

        template <int _Dim>
        inline DirectX::XMVECTOR operator-(
            _In_ const DirectX::XMFLOAT4 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return vector4(lhs) - rhs;
        }
#pragma endregion
    }
}



#ifndef _DHORN_D3D_VECTOR_NO_OPERATORS
inline bool operator==(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
{
    return DirectX::XMVector4Equal(lhs, rhs);
}

inline bool operator!=(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
{
    return DirectX::XMVector4NotEqual(lhs, rhs);
}

inline DirectX::XMVECTOR operator-(_In_ DirectX::FXMVECTOR v)
{
    return DirectX::XMVectorNegate(v);
}

inline DirectX::XMVECTOR operator+(
    _In_ DirectX::FXMVECTOR lhs,
    _In_ DirectX::FXMVECTOR rhs)
{
    return DirectX::XMVectorAdd(lhs, rhs);
}

inline DirectX::XMVECTOR operator-(
    _In_ DirectX::FXMVECTOR lhs,
    _In_ DirectX::FXMVECTOR rhs)
{
    return DirectX::XMVectorSubtract(lhs, rhs);
}

inline DirectX::XMVECTOR operator+(_In_ float lhs, _In_ DirectX::FXMVECTOR rhs)
{
    auto v = DirectX::XMVectorSet(lhs, lhs, lhs, lhs);
    return v + rhs;
}

inline DirectX::XMVECTOR operator-(_In_ float lhs, _In_ DirectX::FXMVECTOR rhs)
{
    auto v = DirectX::XMVectorSet(lhs, lhs, lhs, lhs);
    return v - rhs;
}
#endif  /* _DHORN_D3D_VECTOR_NO_OPERATORS */
