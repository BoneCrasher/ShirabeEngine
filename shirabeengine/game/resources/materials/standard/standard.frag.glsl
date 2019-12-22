#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
// layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput gbuffer0;
// layout(input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput gbuffer1;
// layout(input_attachment_index = 2, set = 1, binding = 2) uniform subpassInput gbuffer2;
// layout(input_attachment_index = 3, set = 1, binding = 3) uniform subpassInput gbuffer3;

// layout(set = 3, binding = 0) uniform sampler2D diffuseTexture;
// layout(set = 3, binding = 1) uniform sampler2D specularReflectanceGlossTexture;
// layout(set = 3, binding = 2) uniform sampler2D normalTexture;

// Input
layout (location = 0)
in struct_vertexData_full shader_input;

// Output
layout (location = 0) out vec4 fragment_color_0;
layout (location = 1) out vec4 fragment_color_1;
layout (location = 2) out vec4 fragment_color_2;

void main()
{
    vec3 L = normalize(shader_input.vertex_tangent - shader_input.vertex_position); // Hack
    vec3 N = normalize(shader_input.vertex_normal);
    float f = dot(N, L);

    fragment_color_0 = clamp(f, 0.0f, 1.0f) * shader_input.vertex_color;
    fragment_color_1 = vec4(0.5f * (shader_input.vertex_normal + 1.0f), 1.0);
    fragment_color_2 = clamp(f, 0.0f, 1.0f) * shader_input.vertex_color;
} 
