/*
 * Duncan Horn
 *
 * d3d_vector.h
 *
 * A wrapper around the XMVECTOR and XMFLOAT4 structures. These are simply meant for convenience when dealing with
 * vectors that are member variables since we must continuously convert from XMVECTOR <--> XMFLOAT4 whenever we wish to
 * perform a mathematical operation and store the result(s). Note that the d3d_vector wraps the XMFLOAT4 structure as
 * opposed to the XMFLOAT3 structure. This is to support homogeneous coordinates without the need to define separate
 * structures (e.g. d3d_point) and an exponentially increasing number of mathematical operations to allow all of such
 * types to inter-operate together. The d3d_vector type exposes the public function is_point which will return true if
 * the 'w' component is equal to one. There are also several asserts sprinkled about that will validate that the 'w'
 * component never becomes any value other than zero or one. Also note that various operations don't make sense for
 * points - like cross product - so we assert there as well.
 */
#pragma once

#include "garbage/dx_vector_base.h"

namespace dhorn
{
    namespace d3d
    {
        class d3d_vector final :
            public garbage::dx_vector_base<DirectX::XMFLOAT4>
        {
            using MyBase = garbage::dx_vector_base<DirectX::XMFLOAT4>;

        public:
            /*
             * Constructor(s)/Destructor (inherit from base)
             */
            using MyBase::MyBase;



            /*
             * Assignment
             */
            d3d_vector &operator=(_In_ d3d_vector &) = default;

            d3d_vector &operator=(_In_ const value_type &val)
            {
                MyBase::operator=(val);
                return *this;
            }

            d3d_vector &operator=(_In_ DirectX::FXMVECTOR v)
            {
                MyBase::operator=(v);
                return *this;
            }
        };
    }
}
