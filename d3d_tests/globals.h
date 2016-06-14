/*
 * Duncan Horn
 *
 * globals.h
 *
 * 
 */
#pragma once

#include <dhorn/d3d/camera.h>
#include <dhorn/d3d11/d3d11_window.h>

namespace globals
{
    extern dhorn::d3d::camera camera;
    extern dhorn::d3d11::d3d11_window window;

    // Shaders
    extern dhorn::com_ptr<ID3D11VertexShader> vertex_shader;
    extern dhorn::com_ptr<ID3D11PixelShader> pixel_shader;

    // Geometry information
    extern dhorn::com_ptr<ID3D11InputLayout> input_layout;
    extern dhorn::com_ptr<ID3D11Buffer> cuboid_vertices;
    extern dhorn::com_ptr<ID3D11Buffer> cuboid_indices;
    extern dhorn::com_ptr<ID3D11Buffer> object_data;

    // Camera controls
    extern bool forward;
    extern bool backward;
    extern bool up;
    extern bool down;
    extern bool left;
    extern bool right;
}
