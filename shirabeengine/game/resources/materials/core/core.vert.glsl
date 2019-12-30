#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

//
// Vertex shader output
//
layout (location = 0)
out struct_vertexData_full shader_output;

void main()
{
    shader_output.vertex_color    = vec4(0.0, 0.0,  0.0, 10);
    shader_output.vertex_position = vec3(0.0, 0.0,  0.0);
    shader_output.vertex_normal   = vec3(1.0, 0.0,  0.0);
    shader_output.vertex_tangent  = vec3(0.0, 0.0, -1.0);
    shader_output.vertex_tangent  = vec3(1.0, 0.0,  0.0);
    shader_output.vertex_texcoord = vec2(0.0, 0.0);
}
