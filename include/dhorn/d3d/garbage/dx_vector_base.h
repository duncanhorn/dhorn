/*
 * Duncan Horn
 *
 * dx_vector_base.h
 *
 * Shared base class for the d3d_vector and d2d_vector types
 */
#pragma once

#include <DirectXMath.h>

namespace dhorn
{
    namespace d3d
    {
        namespace garbage
        {
            /*
             * dx_vector_traits types
             */
            template <typename StorageType>
            struct dx_vector_traits;

            template <>
            struct dx_vector_traits<DirectX::XMFLOAT3>
            {
                using value_type = DirectX::XMFLOAT3;

                inline void store(_Out_ value_type *data, _In_ DirectX::FXMVECTOR v)
                {
                    DirectX::XMStoreFloat3(data, v);
                }

                inline DirectX::XMVECTOR load(_In_ const value_type &val)
                {
                    return DirectX::XMLoadFloat3(&val);
                }

                inline bool is_point(_In_ const value_type &val)
                {
                    return val.z == 1.0f;
                }

                inline bool is_point(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetZ(v) == 1.0f;
                }

                inline bool equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    return DirectX::XMVector3Equal(lhs, rhs);
                }

                inline bool not_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    return DirectX::XMVector3NotEqual(lhs, rhs);
                }

                inline bool less(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector2Less(lhs, rhs);
                }

                inline bool less_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector2LessOrEqual(lhs, rhs);
                }

                inline bool greater(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector2Greater(lhs, rhs);
                }

                inline bool greater_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector2GreaterOrEqual(lhs, rhs);
                }

                inline DirectX::XMVECTOR length(_In_ DirectX::FXMVECTOR v)
                {
                    assert(!is_point(v));
                    return DirectX::XMVector2Length(v);
                }

                inline DirectX::XMVECTOR length_squared(_In_ DirectX::FXMVECTOR v)
                {
                    assert(!is_point(v));
                    return DirectX::XMVector2LengthSq(v);
                }

                inline void verify(_In_ const value_type &val)
                {
                    assert((val.z == 0.0f) || (val.z == 1.0f));
                }
            };

            template <>
            struct dx_vector_traits<DirectX::XMFLOAT4>
            {
                using value_type = DirectX::XMFLOAT4;

                inline void store(_Out_ value_type *data, _In_ DirectX::FXMVECTOR v)
                {
                    DirectX::XMStoreFloat4(data, v);
                }

                inline DirectX::XMVECTOR load(_In_ const value_type &val)
                {
                    return DirectX::XMLoadFloat4(&val);
                }

                inline bool is_point(_In_ const value_type &val)
                {
                    return val.w == 1.0f;
                }

                inline bool is_point(_In_ DirectX::FXMVECTOR v)
                {
                    return DirectX::XMVectorGetW(v) == 1.0f;
                }

                inline bool equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    return DirectX::XMVector4Equal(lhs, rhs);
                }

                inline bool not_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    return DirectX::XMVector4NotEqual(lhs, rhs);
                }

                inline bool less(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector3Less(lhs, rhs);
                }

                inline bool less_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector3LessOrEqual(lhs, rhs);
                }

                inline bool greater(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector3Greater(lhs, rhs);
                }

                inline bool greater_equal(_In_ DirectX::FXMVECTOR lhs, _In_ DirectX::FXMVECTOR rhs)
                {
                    assert(!is_point(lhs));
                    assert(!is_point(rhs));
                    return DirectX::XMVector3GreaterOrEqual(lhs, rhs);
                }

                inline DirectX::XMVECTOR length(_In_ DirectX::FXMVECTOR v)
                {
                    assert(!is_point(v));
                    return DirectX::XMVector3Length(v);
                }

                inline DirectX::XMVECTOR length_squared(_In_ DirectX::FXMVECTOR v)
                {
                    assert(!is_point(v));
                    return DirectX::XMVector3LengthSq(v);
                }

                inline void verify(_In_ const value_type &val)
                {
                    assert((val.w == 0.0f) || (val.w == 1.0f));
                }
            };



            /*
             * dx_vector_base base class
             */
#pragma region dx_vector_base

            template <typename StorageType>
            class dx_vector_base
            {
            protected:

                using TraitsType = dx_vector_traits<StorageType>;

            public:
                /*
                 * Public Types
                 */
                using value_type = typename TraitsType::value_type;



                /*
                 * Constructor(s)/Destructor
                 */
                dx_vector_base(void) :
                    _vector{}
                {
                }

                dx_vector_base(_In_ const dx_vector_base &) = default;

                dx_vector_base(_In_ const value_type &val) :
                    _vector(val)
                {
                    TraitsType::verify(this->_vector);
                }

                dx_vector_base(_In_ DirectX::FXMVECTOR v)
                {
                    this->Assign(v);
                }



                /*
                 * Assignment
                 */
                dx_vector_base &operator=(_In_ const dx_vector_base &) = default;

                dx_vector_base &operator=(_In_ const value_type &val)
                {
                    this->_vector = val;
                    TraitsType::verify(this->_vector);

                    return *this;
                }

                dx_vector_base &operator=(_In_ DirectX::FXMVECTOR v)
                {
                    this->Assign(v);
                    return *this;
                }



                /*
                 * Public Functions
                 */
                inline bool is_point(void) const
                {
                    return TraitsType::is_point(this->_vector);
                }

                inline float length(void) const
                {
                    return DirectX::XMVectorGetX(TraitsType::length(*this));
                }

                inline float length_squared(void) const
                {
                    return DirectX::XMVectorGetX(TraitsType::length_squared(*this));
                }



                /*
                 * Implicit Type Conversion
                 */
                inline operator const StorageType &(void) const
                {
                    return this->_vector;
                }

                inline operator DirectX::XMVECTOR(void) const
                {
                    return TraitsType::load(this->_vector);
                }



                /*
                 * Mathematical Operators
                 */
                inline dx_vector_base operator+=(_In_ dx_vector_base &rhs)
                {
                    return *this += static_cast<DirectX::XMVECTOR>(rhs);
                }

                inline dx_vector_base operator+=(_In_ DirectX::FXMVECTOR rhs)
                {
                    *this = static_cast<DirectX::XMVECTOR>(*this) + rhs;
                    TraitsType::verify(this->_vector);

                    return *this;
                }

                inline dx_vector_base operator-=(_In_ dx_vector_base &rhs)
                {
                    return *this -= static_cast<DirectX::XMVECTOR>(rhs);
                }

                inline dx_vector_base operator-=(_In_ DirectX::FXMVECTOR rhs)
                {
                    *this = static_cast<DirectX::XMVECTOR>(*this) - rhs;
                    TraitsType::verify(this->_vector);

                    return *this;
                }



            protected:

                inline void Assign(_In_ DirectX::FXMVECTOR v)
                {
                    TraitsType::store(&this->_vector, v);
                    TraitsType::verify(this->_vector);
                }

                StorageType _vector;
            };



            /*
             * dx_vector_base Mathematical Operations
             */
            template <typename VectorType>
            inline DirectX::XMVECTOR operator+(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return static_cast<DirectX::XMVECTOR>(lhs) + static_cast<DirectX::XMVECTOR>(rhs);
            }

            template <typename VectorType>
            inline DirectX::XMVECTOR operator+(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                return static_cast<DirectX::XMVECTOR>(lhs) + rhs;
            }

            template <typename VectorType>
            inline DirectX::XMVECTOR operator+(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                return lhs + static_cast<DirectX::XMVECTOR>(rhs);
            }

            template <typename VectorType>
            inline DirectX::XMVECTOR operator-(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return static_cast<DirectX::XMVECTOR>(lhs) - static_cast<DirectX::XMVECTOR>(rhs);
            }

            template <typename VectorType>
            inline DirectX::XMVECTOR operator-(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                return static_cast<DirectX::XMVECTOR>(lhs) - rhs;
            }

            template <typename VectorType>
            inline DirectX::XMVECTOR operator-(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                return lhs - static_cast<DirectX::XMVECTOR>(rhs);
            }



            /*
             * dx_vector_base Logical Operations
             */

            // operator==
            template <typename VectorType>
            inline bool operator==(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::equal(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator==(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                return DirectX::XMVector4Equal(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator==(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                return DirectX::XMVector4Equal(lhs, rhs);
            }

            // operator!=
            template <typename VectorType>
            inline bool operator!=(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::not_equal(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator!=(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                return DirectX::XMVector4NotEqual(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator!=(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                return DirectX::XMVector4NotEqual(lhs, rhs);
            }

            // operator<
            template <typename VectorType>
            inline bool operator<(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::less(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator<(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                assert(!lhs.is_point());
                return DirectX::XMVector4Less(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator<(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                assert(!rhs.is_point());
                return DirectX::XMVector4Less(lhs, rhs);
            }

            // operator<=
            template <typename VectorType>
            inline bool operator<=(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::less_equal(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator<=(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                assert(!lhs.is_point());
                return DirectX::XMVector4LessOrEqual(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator<=(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                assert(!rhs.is_point());
                return DirectX::XMVector4LessOrEqual(lhs, rhs);
            }

            // operator>
            template <typename VectorType>
            inline bool operator>(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::greater(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator>(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                assert(!lhs.is_point());
                return DirectX::XMVector4Greater(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator>(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                assert(!rhs.is_point());
                return DirectX::XMVector4Greater(lhs, rhs);
            }

            // operator>=
            template <typename VectorType>
            inline bool operator>=(
                _In_ const dx_vector_base<VectorType> &lhs,
                _In_ const dx_vector_base<VectorType> &rhs)
            {
                return dx_vector_traits<VectorType>::greater_equal(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator>=(_In_ const dx_vector_base<VectorType> &lhs, _In_ DirectX::FXMVECTOR rhs)
            {
                assert(!lhs.is_point());
                return DirectX::XMVector4GreaterOrEqual(lhs, rhs);
            }

            template <typename VectorType>
            inline bool operator>=(_In_ DirectX::FXMVECTOR lhs, _In_ const dx_vector_base<VectorType> &rhs)
            {
                assert(!rhs.is_point());
                return DirectX::XMVector4GreaterOrEqual(lhs, rhs);
            }

#pragma endregion
        }
    }
}
