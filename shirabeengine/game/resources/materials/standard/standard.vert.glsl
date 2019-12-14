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
    mat3 view3x3  = mat3(graphicsData.primaryCamera.view);

    vec4 position_viewspace  = (graphicsData.primaryCamera.view * position);
    vec3 normal_viewspace    = (view3x3 * vertex_normal);
    vec3 tangent_viewspace   = (view3x3 * vertex_tangent.xyz);
    vec3 bitangent_viewspace = normalize(cross(normal_viewspace, tangent_viewspace));

    //gl_Position = vec4(vertex_position.xyz, 1.0);
    gl_Position = (graphicsData.primaryCamera.projection * position_viewspace);
    //gl_Position.x *= modelMatrices.scale[0];

    shader_output.vertex_position = position_viewspace.xyz;
    shader_output.vertex_normal   = normal_viewspace;
    shader_output.vertex_tangent  = tangent_viewspace;
    shader_output.vertex_texcoord = vertex_texcoord;
    shader_output.vertex_color    = vec4(clamp(gl_Position.xyz * 5, 0.0, 1.0), 1.0);

}
