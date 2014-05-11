/*
 * vector.h
 *
 * Duncan Horn
 *
 * A C++ wrapper around the DirectX Math vector
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

            inline vector(_In_ const vector &other)
            {
                auto v = traits::load(other);
                traits::store(v, this->_vector);
            }

            inline vector(_In_ const storage_type &other)
            {
                auto v = traits::load(other);
                traits::store(v, this->_vector);
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
                auto v = traits::load(other._vector);
                traits::store(v, this->_vector);

                return *this;
            }

            vector &operator=(_In_ const storage_type &other)
            {
                auto v = traits::load(other);
                traits::store(v, this->_vector);

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

inline DirectX::XMVECTOR operator+(_In_ float lhs, _In_ DirectX::XMVECTOR rhs)
{
    auto v = DirectX::XMVectorSet(lhs, lhs, lhs, lhs);
    return v + rhs;
}

inline DirectX::XMVECTOR operator-(_In_ float lhs, _In_ DirectX::XMVECTOR rhs)
{
    auto v = DirectX::XMVectorSet(lhs, lhs, lhs, lhs);
    return v - rhs;
}
#endif  /* _DHORN_D3D_VECTOR_NO_OPERATORS */
