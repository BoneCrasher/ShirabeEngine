#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

//
// Model specific matrices.
//
layout (std140, set = 2, binding = 0)
uniform struct_modelMatrices
{
    mat4  world;
    mat4  inverseTransposeWorld;
    float scale[2];
} modelMatrices;

//
// Input description
//
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec4 vertex_tangent;
layout (location = 3) in vec2 vertex_texcoord;

//
// Vertex shader output
//
layout (location = 0)
out struct_vertexData_full shader_output;

void main()
{
    vec4 position = vec4(vertex_position.xyz, 1.0);

    position.x *= modelMatrices.scale[0] * 4;
    position.y *= modelMatrices.scale[1] * 4;
    gl_Position = position;

    shader_output.vertex_position = gl_Position.xyz;
    shader_output.vertex_normal   = vec3(1.0, 0.0,  0.0);
    shader_output.vertex_tangent  = vec3(0.0, 0.0, -1.0);
    shader_output.vertex_texcoord = vec2(gl_Position.xy);
    shader_output.vertex_color    = vec4(gl_Position.xyz, 1.0);

}
