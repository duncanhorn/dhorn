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
                static_assert(false, "ERROR: DirectX only supports, 2, 3, and 4 dimensional "
                    "vectors");
            };

            template <>
            struct vector_traits<2>
            {
                static const int dimension = 2;
                using storage_type = DirectX::XMFLOAT2;

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
                    auto result = DirectX::XMVector2Equal(v1, v2);
                }
            };

            template <>
            struct vector_traits<3>
            {
                static const int dimension = 3;
                using storage_type = DirectX::XMFLOAT3;

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
                    auto result = DirectX::XMVector3Equal(v1, v2);
                }
            };

            template <>
            struct vector_traits<4>
            {
                static const int dimension = 4;
                using storage_type = DirectX::XMFLOAT4;

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
                    auto result = DirectX::XMVector4Equal(v1, v2);
                }
            };

            template <int _Dim1, int _Dim2>
            struct vector_result_type
            {
                static const int dimension =
                    (_Dim1 > _Dim2) ? vector_traits<_Dim1> : vector_traits<_Dim2>;
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
            using storage_type = traits::storage_type;

        public:
            vector(void)
            {
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
            inline bool operator==(_In_ const vector &other) const
            {
                return traits::equals(*this, other);
            }

            inline bool operator!=(_In_ const vector &other) const
            {
                return !(*this == other);
            }



            /*
             * Mathematical operations
             */
            inline vector normalize(void) const
            {
                return traits::normalize(this->_vector);
            }

            inline FLOAT dot_product(_In_ const vector &other) const
            {
                // TODO
            }

            inline vector cross_product(_In_ const vector &other) const
            {
                // TODO
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
                _In_ const vector &other<_Dim2>) const
            {
                return DirectX::XMVectorAdd(*this, other);
            }

            inline const vector &operator+=(_In_ const vector &other)
            {
                *this = *this + other;
                return *this;
            }

            template <int _Dim2>
            inline vector<garbage::vector_result_type<_Dim, _Dim2>::dimension> operator-(
                _In_ const vector &other<_Dim2>) const
            {
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
    }
}
