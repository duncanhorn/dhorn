/*
 * Duncan Horn
 *
 * shapes.h
 *
 * Helpers for creating geometry of different shapes. Note that each generation function produces two lists: one of
 * vertices and one of indices. The list of vertices uses the dhorn::d3d::vertex type for representing the vertices
 * which contains a 3D point and a 3D normal vector. Note that some shapes may have 2+ vertices with the same position,
 * but with different normals (e.g. a cuboid).
 *
 * All shapes assume the unit vectors i, j, and k as the right, up, and forward vectors respectively positioned along
 * the negative z-axis for their terminology. I.e. the "front" face of a cuboid would be the rectangle that is parallel
 * to the x-y plane positioned with negative z-coordinates. Similarly, the "top" face of a cuboid would be the
 * rectangle that is parallel to the x-z plane positioned with positive y-coordinates. Thus, the "width" would refer to
 * the length along the x-axis, the "height" would refer to the length along the y-axis, and the "depth" would refer to
 * the length along the z-axis.
 */
#pragma once

#include <vector>

#include <DirectXMath.h>

namespace dhorn
{
    namespace d3d
    {
        /*
         * vertex structure
         */
        struct vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 normal;
        };



        /*
         * cuboid
         */
        void cuboid(
            _In_ float width,
            _In_ float height,
            _In_ float depth,
            _Inout_ std::vector<vertex> &vertices,
            _Inout_ std::vector<unsigned> &indices)
        {
            vertices.clear();
            indices.clear();

            float x = width * 0.5f;
            float y = height * 0.5f;
            float z = depth * 0.5f;

            /*
             * Fill in the vertices
             */
            vertices.resize(24);
            size_t i = 0;

            // Front face
            DirectX::XMFLOAT3 normal = { 0, 0, -1 };
            vertices[i++] = { {  x,  y, -z }, normal };
            vertices[i++] = { {  x, -y, -z }, normal };
            vertices[i++] = { { -x, -y, -z }, normal };
            vertices[i++] = { { -x,  y, -z }, normal };

            // Back face
            normal = { 0, 0, 1 };
            vertices[i++] = { {  x,  y, z }, normal };
            vertices[i++] = { { -x,  y, z }, normal };
            vertices[i++] = { { -x, -y, z }, normal };
            vertices[i++] = { {  x, -y, z }, normal };

            // Top face
            normal = { 0, 1, 0 };
            vertices[i++] = { {  x, y,  z }, normal };
            vertices[i++] = { {  x, y, -z }, normal };
            vertices[i++] = { { -x, y, -z }, normal };
            vertices[i++] = { { -x, y,  z }, normal };

            // Bottom face
            normal = { 0, -1, 0 };
            vertices[i++] = { {  x, -y,  z }, normal };
            vertices[i++] = { { -x, -y,  z }, normal };
            vertices[i++] = { { -x, -y, -z }, normal };
            vertices[i++] = { {  x, -y, -z }, normal };

            // Right face
            normal = { 1, 0, 0 };
            vertices[i++] = { { x,  y,  z }, normal };
            vertices[i++] = { { x, -y,  z }, normal };
            vertices[i++] = { { x, -y, -z }, normal };
            vertices[i++] = { { x,  y, -z }, normal };

            // Left face
            normal = { -1, 0, 0 };
            vertices[i++] = { { -x,  y,  z }, normal };
            vertices[i++] = { { -x,  y, -z }, normal };
            vertices[i++] = { { -x, -y, -z }, normal };
            vertices[i++] = { { -x, -y,  z }, normal };
            assert(i == vertices.size());

            /*
             * Fill in the indices
             */
            indices.resize(36);
            i = 0;

            for (size_t j = 0; j < 6; ++j)
            {
                size_t offset = j * 4;

                // The vertices should be ordered in a clockwise fashion
                indices[i++] = offset;
                indices[i++] = offset + 1;
                indices[i++] = offset + 2;

                indices[i++] = offset + 2;
                indices[i++] = offset + 3;
                indices[i++] = offset;
            }
            assert(i == indices.size());
        }
    }
}
