#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
layout(input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 2, binding = 3) uniform subpassInput gbuffer3;
layout(input_attachment_index = 4, set = 2, binding = 4) uniform subpassInput depth;
layout(input_attachment_index = 5, set = 2, binding = 5) uniform subpassInput lightAccumulation;

// layout(std140, set = 3, binding = 0)
// uniform struct_screenInfo
// {
//     vec2 extent;
// } screenInfo;

// Input
// layout (location = 0)
// in vec2 texcoord;

// Output
layout (location = 0) out vec4 fragment_final_0;

void main()
{
    vec4  position_specular_intensity = subpassLoad(gbuffer0);
    vec4  normal_specular_exponent    = subpassLoad(gbuffer1);
    vec4  diffuse                     = subpassLoad(gbuffer2);
    vec4  other_1                     = subpassLoad(gbuffer3);
    vec4  depth                       = subpassLoad(depth);
    vec4  lightAccumulation           = subpassLoad(lightAccumulation);

    vec3 final = clamp((lightAccumulation.a * diffuse.rgb) + lightAccumulation.rgb, 0.0f, 1.0f);
    // vec3 final = clamp(lightAccumulation.rgb, 0.0f, 1.0f);
    fragment_final_0 = vec4(final, 1.0f); // Just forward...
} 
