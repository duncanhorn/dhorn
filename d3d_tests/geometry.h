/*
 * Duncan Horn
 *
 * geometry.h
 */
#pragma once

#include <DirectXMath.h>

struct vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
};
