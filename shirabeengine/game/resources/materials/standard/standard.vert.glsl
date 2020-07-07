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

    mat4 view_transform     = (graphicsData.cameras[0].view * modelMatrices.world);
    mat3 view_transform_3x3 = mat3(view_transform);

    vec4 position_viewspace  = (view_transform * position);
    vec3 normal_viewspace    = normalize(view_transform_3x3 * vertex_normal);
    vec3 tangent_viewspace   = normalize(view_transform_3x3 * vertex_tangent.xyz);
    vec3 bitangent_viewspace = normalize(cross(normal_viewspace, tangent_viewspace) * vertex_tangent.w);

    gl_Position   = (graphicsData.cameras[0].projection * position_viewspace);
    // gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;

    shader_output.vertex_position  = position_viewspace.xyz;
    shader_output.vertex_normal    = normal_viewspace;
    shader_output.vertex_tangent   = tangent_viewspace;
    shader_output.vertex_bitangent = bitangent_viewspace;
    shader_output.vertex_texcoord  = vertex_texcoord;
    shader_output.vertex_color     = vec4(1.0f, 1.0f, 1.0f, 1.0);

}
