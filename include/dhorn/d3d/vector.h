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
        template <int _Dim>
        class vector;

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
                using cross_product_result_type = vector<3>;

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
                using cross_product_result_type = vector<4>;

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

        public:
            vector(void)
            {
                traits::store(DirectX::g_XMZero, this->_vector);
            }

            vector(_In_ const storage_type &other)
            {
                auto v = traits::load(other);
                traits::store(v, this->_vector);
            }

            vector(_In_ DirectX::FXMVECTOR other)
            {
                traits::store(other, this->_vector);
            }



            /*
             * Assignment
             */
            vector &operator=(_In_ const storage_type &other)
            {
                return (*this = traits::load(other));
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

            inline bool operator==(_In_ const vector<4> &other) const
            {
                using traits_type = typename garbage::vector_traits<4>;
                return traits_type::equals(*this, other);
            }

            template <int _Dim2>
            inline bool operator!=(_In_ const vector<_Dim2> &other) const
            {
                return !(*this == other);
            }

            inline bool operator!=(_In_ const vector<4> &other) const
            {
                return !(*this == other);
            }



            /*
             * Mathematical operations
             */
            inline vector normalize(void) const
            {
                auto v = traits::load(this->_vector);
                return traits::normalize(v);
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
            inline vector operator-(void) const
            {
                return DirectX::XMVectorNegate(*this);
            }

            template <int _Dim2>
            inline vector<garbage::vector_result_type<_Dim, _Dim2>::dimension> operator+(
                _In_ const vector<_Dim2> &other) const
            {
                return DirectX::XMVectorAdd(*this, other);
            }

            inline vector<4> operator+(_In_ const vector<4> &other) const
            {
                /* If anyone tries to perform vector + XMVECTOR, then it is impossible to
                   statically determine the resulting dimension. Therefore, use a dimension of 4 */
                return DirectX::XMVectorAdd(*this, other);
            }

            inline const vector &operator+=(_In_ const vector &other)
            {
                *this = *this + other;
                return *this;
            }

            template <int _Dim2>
            inline vector<garbage::vector_result_type<_Dim, _Dim2>::dimension> operator-(
                _In_ const vector<_Dim2> &other) const
            {
                return DirectX::XMVectorSubtract(*this, other);
            }

            inline vector<4> operator-(_In_ const vector<4> &other) const
            {
                /* If anyone tries to perform vector - XMVECTOR, then it is impossible to
                statically determine the resulting dimension. Therefore, use a dimension of 4 */
                return DirectX::XMVectorSubtract(*this, other);
            }

            inline const vector &operator-=(_In_ const vector &other)
            {
                *this = *this - other;
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
        template <int _Dim>
        inline bool operator==(_In_ DirectX::FXMVECTOR lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs == lhs;
        }

        template <int _Dim>
        inline bool operator!=(_In_ DirectX::FXMVECTOR lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs != lhs;
        }

        template <int _Dim>
        inline vector4 operator+(_In_ DirectX::FXMVECTOR lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs + lhs;
        }

        template <int _Dim>
        inline vector4 operator-(_In_ DirectX::FXMVECTOR lhs, _In_ const vector<_Dim> &rhs)
        {
            return DirectX::XMVectorSubtract(lhs, rhs);
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
            return rhs == vector2(lhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs != vector2(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT2 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs != vector2(lhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 2>::dimension> operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT2 &rhs)
        {
            return lhs + vector2(rhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 2>::dimension> operator+(
            _In_ const DirectX::XMFLOAT2 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return rhs + vector2(lhs);
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
            return rhs == vector3(lhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs != vector3(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT3 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs != vector3(lhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 3>::dimension> operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT3 &rhs)
        {
            return lhs + vector3(rhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 3>::dimension> operator+(
            _In_ const DirectX::XMFLOAT3 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return rhs + vector3(lhs);
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
            return rhs == vector4(lhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const vector<_Dim> &lhs, _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs != vector4(rhs);
        }

        template <int _Dim>
        inline bool operator!=(_In_ const DirectX::XMFLOAT4 &lhs, _In_ const vector<_Dim> &rhs)
        {
            return rhs != vector4(lhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 4>::dimension> operator+(
            _In_ const vector<_Dim> &lhs,
            _In_ const DirectX::XMFLOAT4 &rhs)
        {
            return lhs + vector4(rhs);
        }

        template <int _Dim>
        inline vector<garbage::vector_result_type<_Dim, 4>::dimension> operator+(
            _In_ const DirectX::XMFLOAT4 &lhs,
            _In_ const vector<_Dim> &rhs)
        {
            return rhs + vector4(lhs);
        }
    }
}
