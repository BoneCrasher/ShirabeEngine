#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"
#include "standard.shared.glsl"

// Input
layout (location = 0)
in struct_vertexData_full shader_input;

// Output
layout (location = 0) out vec4 fragment_color_0;
layout (location = 1) out vec4 fragment_color_1;
layout (location = 2) out vec4 fragment_color_2;
layout (location = 3) out vec4 fragment_color_3;

void main()
{
    struct_instanceData lInstanceData = instanceData.data[instanceIndex];

    sampler2D diffuseTexture     = textures[lInstanceData.textures.diffuse];
    sampler2D specularTexture    = textures[lInstanceData.textures.specular];
    sampler2D normalTextureIndex = textures[lInstanceData.textures.normal];

    vec3 normal_viewspace    = normalize(shader_input.vertex_normal);
    vec3 tangent_viewspace   = normalize(shader_input.vertex_tangent);
    vec3 bitangent_viewspace = normalize(shader_input.vertex_bitangent);

    mat3 tnb = mat3(tangent_viewspace, bitangent_viewspace, normal_viewspace);

    vec4 diffuse = texture(diffuseTexture,     shader_input.vertex_texcoord.xy);
    vec4 normal  = texture(normalTextureIndex, shader_input.vertex_texcoord.xy);

    vec3 normal_unpacked_tangent_space = unpack_normal(normal.xyz);
    vec3 normal_unpacked_viewspace     = tnb * normal_unpacked_tangent_space;

    normal_unpacked_viewspace = normalize(normal_unpacked_viewspace);

    float f_spec     = 0.8f;
    float f_spec_exp = 32.0f;

    fragment_color_0 = vec4(shader_input.vertex_position.xyz,  f_spec);
    fragment_color_1 = vec4(normal_unpacked_viewspace,         f_spec_exp);
    fragment_color_2 = diffuse;
    fragment_color_3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
} 
