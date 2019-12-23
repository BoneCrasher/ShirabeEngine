#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

layout(set = 2, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 2, binding = 1) uniform sampler2D specularReflectanceGlossTexture;
layout(set = 2, binding = 2) uniform sampler2D normalTexture;

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
    fragment_color_0 = vec4(shader_input.vertex_position.xyz, 1.0);
    fragment_color_1 = vec4(shader_input.vertex_normal,       1.0);
    fragment_color_2 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    fragment_color_3 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
} 
