/*
 * Duncan Horn
 *
 * globals.h
 *
 * 
 */
#pragma once

#include <dhorn/d3d.h>
#include <dhorn/d3d/camera.h>
#include <dhorn/d3d/d3d_window.h>

namespace globals
{
    extern dhorn::d3d::d3d_window window;
    extern dhorn::d3d::camera camera;

    // Shaders
    extern dhorn::win32::com_ptr<ID3D11VertexShader> vertex_shader;
    extern dhorn::win32::com_ptr<ID3D11PixelShader> pixel_shader;

    // Geometry information
    extern dhorn::win32::com_ptr<ID3D11InputLayout> input_layout;
    extern dhorn::win32::com_ptr<ID3D11Buffer> cuboid_vertices;
    extern dhorn::win32::com_ptr<ID3D11Buffer> cuboid_indices;
    extern dhorn::win32::com_ptr<ID3D11Buffer> object_data;
}
