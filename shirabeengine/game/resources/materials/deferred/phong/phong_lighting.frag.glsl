#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
layout(input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 2, binding = 3) uniform subpassInput gbuffer3;
layout(input_attachment_index = 4, set = 2, binding = 4) uniform subpassInput depth;

// layout(std140, set = 3, binding = 0)
// uniform struct_screenInfo
// {
//     vec2 extent;
// } screenInfo;

// Input
// layout (location = 0)
// in vec2 texcoord;

// Output
layout (location = 0) out float fragment_light_0;

void main()
{
    vec4 position_specular_intensity = subpassLoad(gbuffer0);
    vec4 normal_specular_exponent    = subpassLoad(gbuffer1);
    vec4 other_0                     = subpassLoad(gbuffer2);
    vec4 other_1                     = subpassLoad(gbuffer3);
    vec4 depth                       = subpassLoad(depth);

    vec3  C = vec3(1.0f, 1.0f, 1.0f);
    vec3  E = vec3(0.0f, 0.0f, 0.0f);
    vec3  L = vec3(0.0f, 0.0f, -1.0f); // normalize(E - position_specular_intensity.xyz);
    vec3  N = normalize(normal_specular_exponent.xyz);
    float f = dot(L, N);

    // fragment_light_0 = vec4( clamp((f * C), 0.0f, 1.0f), 1.0f );
    fragment_light_0 = clamp(f, 0.0f, 1.0f);
} 
