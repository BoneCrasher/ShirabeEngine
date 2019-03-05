#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

//
// Uniform Buffer to hold the camera matrices
//
layout (std140, set = 2, binding = 0)
uniform CameraMatrices
{
    mat4 view;
    mat4 projection;
}
cameraMatrices;

//
// Model specific matrices.
//
layout (std140, set = 3, binding = 0)
uniform ModelMatrices
{
    mat4 world;
    mat4 inverseTransposeWorld;
}
modelMatrices;

//
// Input description
//
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texcoord;

// layout (location = 0) in vec3 vertex_position;
// layout (location = 1) in vec4 vertex_normal;
// layout (location = 5) in mat4 vertex_tangent[3];
// layout (location = 17) in mat4x2 vertex_texcoord;

//
// Vertex shader output
//
layout (location = 0)
out struct_Output
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec2 vertex_texcoord;
} shader_output;

void main()
{
    gl_Position = vec4(0.0, 0.0, 0.0, 1.0); // vec4(vertex_position, 1.0);
}