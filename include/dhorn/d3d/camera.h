/*
 * Duncan Horn
 *
 * camera.h
 *
 * 
 */
#pragma once

#include <DirectXMath.h>

#include "d3d_utils.h"

namespace dhorn
{
    namespace d3d
    {
        class camera final
        {
        public:
            /*
             * Constructor(s)/Destructor
             */
            camera(void) :
                _pos(0, 0, 0),
                _forward(0, 0, 1),
                _right(1, 0, 0),
                _up(0, 1, 0),
                _nearZ(1.0f),
                _farZ(1000.0f),
                _verticalFov(DirectX::XM_PIDIV4),
                _aspectRatio(1.0f),
                _viewMatrixDirty(true),
                _projectionMatrixDirty(true),
                _viewProjectionMatrixDirty(true)
            {
            }



            /*
             * Public properties
             */
            DirectX::XMVECTOR position(void) const
            {
                return DirectX::XMLoadFloat3(&this->_pos);
            }

            DirectX::XMVECTOR forward(void) const
            {
                return DirectX::XMLoadFloat3(&this->_forward);
            }

            DirectX::XMVECTOR right(void) const
            {
                return DirectX::XMLoadFloat3(&this->_right);
            }

            DirectX::XMVECTOR up(void) const
            {
                return DirectX::XMLoadFloat3(&this->_up);
            }

            float near_z(void) const
            {
                return this->_nearZ;
            }

            float far_z(void) const
            {
                return this->_farZ;
            }

            float vertical_fov(void) const
            {
                return this->_verticalFov;
            }

            float aspect_ratio(void) const
            {
                return this->_aspectRatio;
            }



            /*
             * Matrices
             */
            DirectX::XMMATRIX view_matrix(void)
            {
                return ViewMatrix();
            }

            DirectX::XMMATRIX projection_matrix(void)
            {
                return ProjectionMatrix();
            }

            DirectX::XMMATRIX view_projection_matrix(void)
            {
                return ViewProjectionMatrix();
            }



            /*
             * Projection Matrix
             */
            void configure_frustum(
                _In_ float nearZ,
                _In_ float farZ,
                _In_ float verticalFov,
                _In_ float aspectRatioWidthDivHeight)
            {
                this->_nearZ = nearZ;
                this->_farZ = farZ;
                this->_verticalFov = verticalFov;
                this->_aspectRatio = aspectRatioWidthDivHeight;

                this->InvalidateProjectionMatrix();
            }

            void set_near_z(_In_ float z)
            {
                this->_nearZ = z;
                this->InvalidateProjectionMatrix();
            }

            void set_far_z(_In_ float z)
            {
                this->_farZ = z;
                this->InvalidateProjectionMatrix();
            }

            void set_vertical_fov(_In_ float verticalFov)
            {
                this->_verticalFov = verticalFov;
                this->InvalidateProjectionMatrix();
            }

            void set_aspect_ratio(_In_ float aspectRatio)
            {
                this->_aspectRatio = aspectRatio;
                this->InvalidateProjectionMatrix();
            }



            /*
             * Rotation
             */
            void XM_CALLCONV look_at(_In_ DirectX::FXMVECTOR targetPoint, _In_ DirectX::FXMVECTOR worldUp)
            {
                using namespace DirectX;

                auto forward = XMVector3Normalize(XMVectorSubtract(targetPoint, position()));
                auto right = XMVector3Normalize(XMVector3Cross(worldUp, forward));
                auto up = XMVector3Cross(forward, right);

                XMStoreFloat3(&this->_forward, forward);
                XMStoreFloat3(&this->_right, right);
                XMStoreFloat3(&this->_up, up);
                this->InvalidateViewMatrix();
            }

            void XM_CALLCONV rotate(_In_ DirectX::FXMVECTOR axis, _In_ float angle)
            {
                // angle is in units of radians
                auto transform = DirectX::XMMatrixRotationAxis(axis, angle);
                ApplyRotation(transform);
            }

            void rotate_x(_In_ float angle)
            {
                // angle is in units of radians
                auto transform = DirectX::XMMatrixRotationX(angle);
                ApplyRotation(transform);
            }

            void rotate_y(_In_ float angle)
            {
                // angle is in units of radians
                auto transform = DirectX::XMMatrixRotationY(angle);
                ApplyRotation(transform);
            }

            void rotate_z(_In_ float angle)
            {
                // angle is in units of radians
                auto transform = DirectX::XMMatrixRotationZ(angle);
                ApplyRotation(transform);
            }

            void pitch(_In_ float angle)
            {
                using namespace DirectX;

                // angle is in units of radians
                auto transform = XMMatrixRotationAxis(right(), angle);
                auto forward = XMVector3TransformNormal(this->forward(), transform);
                auto up = XMVector3TransformNormal(this->up(), transform);

                XMStoreFloat3(&this->_forward, forward);
                XMStoreFloat3(&this->_up, up);
                this->InvalidateViewMatrix();
            }

            void yaw(_In_ float angle)
            {
                using namespace DirectX;

                // angle is in units of radians
                auto transform = XMMatrixRotationAxis(this->up(), angle);
                auto forward = XMVector3TransformNormal(this->forward(), transform);
                auto right = XMVector3TransformNormal(this->up(), transform);

                XMStoreFloat3(&this->_forward, forward);
                XMStoreFloat3(&this->_right, right);
                this->InvalidateViewMatrix();
            }

            void roll(_In_ float angle)
            {
                using namespace DirectX;

                // angle is in units of radians
                auto transform = XMMatrixRotationAxis(forward(), angle);
                auto right = XMVector3TransformNormal(this->right(), transform);
                auto up = XMVector3TransformNormal(this->up(), transform);

                XMStoreFloat3(&this->_right, right);
                XMStoreFloat3(&this->_up, up);
                this->InvalidateViewMatrix();
            }



            /*
             * Translation
             */
            void XM_CALLCONV set_position(_In_ DirectX::FXMVECTOR pos)
            {
                DirectX::XMStoreFloat3(&this->_pos, pos);
                this->InvalidateViewMatrix();
            }

            void XM_CALLCONV translate(_In_ DirectX::FXMVECTOR delta)
            {
                auto pos = this->position();
                pos = DirectX::XMVectorAdd(pos, delta);

                DirectX::XMStoreFloat3(&this->_pos, pos);
                this->InvalidateViewMatrix();
            }

            void translate_forward(_In_ float distance)
            {
                auto pos = this->position();
                auto forward = this->forward();
                forward = DirectX::XMVectorScale(forward, distance);
                pos = DirectX::XMVectorAdd(pos, forward);

                DirectX::XMStoreFloat3(&this->_pos, pos);
                this->InvalidateViewMatrix();
            }

            void translate_right(_In_ float distance)
            {
                auto pos = this->position();
                auto right = this->right();
                right = DirectX::XMVectorScale(right, distance);
                pos = DirectX::XMVectorAdd(pos, right);

                DirectX::XMStoreFloat3(&this->_pos, pos);
                this->InvalidateViewMatrix();
            }

            void translate_up(_In_ float distance)
            {
                auto pos = this->position();
                auto up = this->up();
                up = DirectX::XMVectorScale(up, distance);
                pos = DirectX::XMVectorAdd(pos, up);

                DirectX::XMStoreFloat3(&this->_pos, pos);
                this->InvalidateViewMatrix();
            }



        private:

            inline void XM_CALLCONV ApplyRotation(_In_ DirectX::FXMMATRIX transform)
            {
                using namespace DirectX;

                XMStoreFloat3(&this->_forward, XMVector3TransformNormal(this->forward(), transform));
                XMStoreFloat3(&this->_right, XMVector3TransformNormal(this->right(), transform));
                XMStoreFloat3(&this->_up, XMVector3TransformNormal(this->up(), transform));

                this->InvalidateViewMatrix();
            }

            inline void InvalidateViewMatrix(void)
            {
                this->_viewMatrixDirty = true;
                this->_viewProjectionMatrixDirty = true;
            }

            inline void InvalidateProjectionMatrix(void)
            {
                this->_projectionMatrixDirty = true;
                this->_viewProjectionMatrixDirty = true;
            }

            inline DirectX::XMMATRIX ViewMatrix(void)
            {
                using namespace DirectX;
                if (this->_viewMatrixDirty)
                {
                    // Keep everything orthogonal to each other
                    auto forward = XMVector3Normalize(this->forward());
                    auto up = XMVector3Normalize(XMVector3Cross(forward, right()));
                    auto right = XMVector3Cross(up, forward);
                    XMStoreFloat3(&this->_forward, forward);
                    XMStoreFloat3(&this->_right, right);
                    XMStoreFloat3(&this->_up, up);

                    auto viewMatrix = create_view_matrix(right, up, forward, this->position());
                    XMStoreFloat4x4(&this->_viewMatrix, viewMatrix);

                    this->_viewMatrixDirty = false;
                    return viewMatrix;
                }

                return XMLoadFloat4x4(&this->_viewMatrix);
            }

            inline DirectX::XMMATRIX ProjectionMatrix(void)
            {
                using namespace DirectX;
                if (this->_projectionMatrixDirty)
                {
                    auto proj = XMMatrixPerspectiveFovLH(
                        this->_verticalFov,
                        this->_aspectRatio,
                        this->_nearZ,
                        this->_farZ);
                    XMStoreFloat4x4(&this->_projectionMatrix, proj);

                    this->_projectionMatrixDirty = false;
                    return proj;
                }

                return XMLoadFloat4x4(&this->_projectionMatrix);
            }

            inline DirectX::XMMATRIX ViewProjectionMatrix(void)
            {
                using namespace DirectX;
                if (this->_viewProjectionMatrixDirty)
                {
                    auto viewProj = ViewMatrix() * ProjectionMatrix();
                    XMStoreFloat4x4(&this->_viewProjectionMatrix, viewProj);

                    this->_viewProjectionMatrixDirty = false;
                    return viewProj;
                }

                return XMLoadFloat4x4(&this->_viewProjectionMatrix);
            }



            // Camera geometrical properties
            DirectX::XMFLOAT3 _pos;
            DirectX::XMFLOAT3 _forward;
            DirectX::XMFLOAT3 _right;
            DirectX::XMFLOAT3 _up;

            // Frustum properties
            float _nearZ;
            float _farZ;
            float _verticalFov;
            float _aspectRatio;

            // Cached data
            bool _viewMatrixDirty;
            DirectX::XMFLOAT4X4 _viewMatrix;
            bool _projectionMatrixDirty;
            DirectX::XMFLOAT4X4 _projectionMatrix;
            bool _viewProjectionMatrixDirty;
            DirectX::XMFLOAT4X4 _viewProjectionMatrix;
        };
    }
}
